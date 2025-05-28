use std::io;
use std::io::Write;

use clap::{command, Parser};
use color_eyre::eyre;
use const_format::formatcp;
use rand::rngs::StdRng;
use rand::{Rng, SeedableRng};
use tracing_subscriber::{fmt::format::FmtSpan, EnvFilter};

use acir::{native_types::WitnessStack, FieldElement};
use bn254_blackbox_solver::Bn254BlackBoxSolver;
use brillig::BinaryFieldOp;
use brillig::Opcode as BrilligOpcode;
use nargo::foreign_calls::{
    layers, transcript::ReplayForeignCallExecutor, DefaultForeignCallBuilder,
};
use noir_artifact_cli::commands::execute_cmd;
use noir_artifact_cli::commands::execute_cmd::ExecuteCommand;
use noir_artifact_cli::execution;
use noir_artifact_cli::execution::{ExecutionResults, ReturnValues};
use noir_artifact_cli::{errors::CliError, Artifact};
use noirc_abi::input_parser::InputValue;
use noirc_abi::InputMap;
use noirc_driver::CompiledProgram;

use std::path::Path;

use acvm::BlackBoxFunctionSolver;
use nargo::{foreign_calls::ForeignCallExecutor, NargoError};
use noir_artifact_cli::fs::{inputs::read_inputs_from_file, witness::save_witness_to_dir};

const PKG_VERSION: &str = env!("CARGO_PKG_VERSION");
static VERSION_STRING: &str = formatcp!("version = {}\n", PKG_VERSION,);

/// Execute a circuit and return the output witnesses.
pub fn run_circuit_and_get_witnesses<B, E>(
    circuit: &CompiledProgram,
    blackbox_solver: &B,
    foreign_call_executor: &mut E,
    input_map: &InputMap,
    expected_return: Option<InputValue>,
) -> Result<ExecutionResults, CliError>
where
    B: BlackBoxFunctionSolver<FieldElement>,
    E: ForeignCallExecutor<FieldElement>,
{
    let initial_witness = circuit.abi.encode(&input_map, None)?;

    let witness_stack = nargo::ops::execute_program(
        &circuit.program,
        initial_witness,
        blackbox_solver,
        foreign_call_executor,
    )?;

    let main_witness = &witness_stack
        .peek()
        .expect("Should have at least one witness on the stack")
        .witness;

    let (_, actual_return) = circuit.abi.decode(main_witness)?;

    Ok(ExecutionResults {
        witness_stack,
        return_values: ReturnValues {
            actual_return,
            expected_return,
        },
    })
}

/// Execute a circuit and return the output witnesses.
fn execute(
    circuit: &CompiledProgram,
    args: &ExecuteCommand,
    input_map: &InputMap,
    expected_return: Option<InputValue>,
) -> Result<ExecutionResults, CliError> {
    // Build a custom foreign call executor that replays the Oracle transcript,
    // and use it as a base for the default executor. Using it as the innermost rather
    // than top layer so that any extra `print` added for debugging is handled by the
    // default, rather than trying to match it to the transcript.
    let transcript_executor = match args.oracle_file {
        Some(ref path) => layers::Either::Left(ReplayForeignCallExecutor::from_file(path)?),
        None => layers::Either::Right(layers::Empty),
    };

    let mut foreign_call_executor = DefaultForeignCallBuilder {
        output: std::io::stdout(),
        enable_mocks: false,
        resolver_url: args.oracle_resolver.clone(),
        root_path: None,
        package_name: None,
    }
    .build_with_base(transcript_executor);

    let blackbox_solver = Bn254BlackBoxSolver(args.pedantic_solving);

    run_circuit_and_get_witnesses(
        circuit,
        &blackbox_solver,
        &mut foreign_call_executor,
        input_map,
        expected_return,
    )
}

pub fn zkfuzz_run(
    args: ExecuteCommand,
    num_generation: usize,
    rng: &mut StdRng,
) -> Result<(), CliError> {
    let artifact = Artifact::read_from_file(&args.artifact_path)?;
    let artifact_name = args
        .artifact_path
        .file_stem()
        .and_then(|s| s.to_str())
        .unwrap_or_default();

    if let Artifact::Program(program) = artifact {
        let mut circuit: CompiledProgram = program.into();
        let (initial_input_map, _initial_expected_return) =
            read_inputs_from_file(&args.prover_file, &circuit.abi)?;
        let mut input_map_keys: Vec<_> = initial_input_map.keys().collect();
        input_map_keys.sort();

        for i in 0..num_generation {
            print!("\r\x1b[2K🧬 Generation: {}/{}", i, num_generation);

            // ------------ Generating random inputs ------------------------ //
            let mut mutated_input_map = initial_input_map.clone();
            let name = input_map_keys[rng.gen_range(0..input_map_keys.len())];
            if let Some(InputValue::Field(_)) = mutated_input_map.get(name) {
                mutated_input_map.insert(
                    name.clone(),
                    InputValue::Field(FieldElement::from(rng.random::<u64>())),
                );
            }

            // ------------ Executing the original circuit ------------------ //
            let original_result = match execute(&circuit, &args, &mutated_input_map, None) {
                Ok(results) => results.return_values.actual_return,
                Err(e) => {
                    if let CliError::CircuitExecutionError(ref err) = e {
                        execution::show_diagnostic(&circuit, err);
                    }
                    None
                }
            };
            let original_unconstrained_functions = circuit.program.unconstrained_functions.clone();

            // ------------ Mutating unconstrained functions ---------------- //

            let mut mutated_unconstrained_functions = original_unconstrained_functions.clone();

            let func_idx: usize = rng.gen_range(0..original_unconstrained_functions.len());
            let instr_pos: usize =
                rng.gen_range(0..original_unconstrained_functions[func_idx].bytecode.len());

            match mutated_unconstrained_functions[func_idx].bytecode[instr_pos] {
                BrilligOpcode::BinaryFieldOp {
                    destination,
                    op: _,
                    lhs,
                    rhs: _,
                } => {
                    mutated_unconstrained_functions[func_idx].bytecode[instr_pos] =
                        BrilligOpcode::BinaryFieldOp {
                            destination,
                            op: BinaryFieldOp::Sub,
                            lhs: lhs.clone(),
                            rhs: lhs.clone(),
                        };
                }
                _ => {}
            }
            circuit.program.unconstrained_functions = mutated_unconstrained_functions;

            // ----------- Executing the mutated circuit -------------------- //

            let mutated_result = match execute(&circuit, &args, &mutated_input_map, None) {
                Ok(results) => results.return_values.actual_return,
                Err(e) => {
                    if let CliError::CircuitExecutionError(ref err) = e {
                        execution::show_diagnostic(&circuit, err);
                    }
                    None
                }
            };

            match (&original_result, &mutated_result) {
                (Some(v), Some(u)) => {
                    if v != u {
                        print!("\r\x1b[2K🧬 Generation: {}/{}", i, num_generation);
                        io::stdout().flush().unwrap();
                        println!("  Under-Constrained");
                        println!("      Original Return Value: {:?}", v);
                        println!("       Mutated Return Value: {:?}", u);
                        return Ok(());
                    }
                }
                (_, _) => {}
            }
        }
    }

    Ok(())
}

#[derive(Parser, Debug)]
#[command(name="noir-execute", author, version=VERSION_STRING, about, long_about = None)]
struct AExecutorCli {
    #[command(flatten)]
    command: execute_cmd::ExecuteCommand,
}

pub fn start_cli() -> eyre::Result<()> {
    let AExecutorCli { command } = AExecutorCli::parse();
    let mut rng = StdRng::seed_from_u64(12);
    zkfuzz_run(command, 1000, &mut rng);

    Ok(())
}

fn main() {
    tracing_subscriber::fmt()
        .with_span_events(FmtSpan::ACTIVE)
        .with_writer(std::io::stderr)
        .with_ansi(true)
        .with_env_filter(EnvFilter::from_env("NOIR_LOG"))
        .init();

    if let Err(e) = start_cli() {
        eprintln!("{e:?}");
        std::process::exit(1);
    }
}
