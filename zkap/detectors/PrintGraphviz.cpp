#include "ProtocolFlowGraph.hpp"

void printGraphviz(PFGraph* graph) {
    std::stringstream sb;
    sb << "digraph "
       << "\"" << graph->getName() << "\""
       << " {\n";
    sb << "graph [fontname=\"Arial\", fontsize=12, bgcolor=\"#f9f9f9\"];\n";
    sb << "node [shape=rectangle, style=\"filled,rounded\", fontname=\"Arial\", fontsize=10, penwidth=1.5];\n";
    sb << "edge [fontname=\"Arial\", fontsize=10, arrowsize=0.8, penwidth=1.2];\n";

    for (auto p : graph->nodes) {
        auto n = p.second;
        sb << "\"<<" << nodeTypeEnumToAbbr(n->type) << ">>\n" << n->name;
        std::string fillcolor, fontcolor, shape;
        switch (n->type) {
            case PFGNodeType::Argument:
                fillcolor = "#889aa4";
                fontcolor = "#ffffff";
                shape = "ellipse";
                break;
            case PFGNodeType::ComponentInput:
                fillcolor = "#ca9a8a";
                fontcolor = "#ffffff";
                shape = "box";
                break;
            case PFGNodeType::ComponentOutput:
                fillcolor = "#bccd81";
                fontcolor = "#000000";
                shape = "box";
                break;
            case PFGNodeType::InputSignal:
                fillcolor = "#c7aaf6";
                fontcolor = "#000000";
                shape = "ellipse";
                break;
            case PFGNodeType::IntermediateSignal:
                fillcolor = "#f8edfc";
                fontcolor = "#000000";
                shape = "ellipse";
                break;
            case PFGNodeType::OutputSignal:
                fillcolor = "#d0fbe1";
                fontcolor = "#000000";
                shape = "ellipse";
                break;
            case PFGNodeType::Expression:
                fillcolor = "#cccccc";
                fontcolor = "#000000";
                shape = "diamond";
                break;
            case PFGNodeType::Constant:
                fillcolor = "#000000";
                fontcolor = "#ffffff";
                shape = "hexagon";
                break;
            case PFGNodeType::Component:
                fillcolor = "#000000";
                fontcolor = "#ffffff";
                shape = "hexagon";
                break;
            case PFGNodeType::Variable:
                fillcolor = "#000000";
                fontcolor = "#ffffff";
                shape = "hexagon";
                break;
        }
        // sb << "\" [color=\"" << color << "\"];\n";
        sb << "\" [fillcolor=\"" << fillcolor << "\", fontcolor=\"" << fontcolor << "\", shape=\"" << shape << "\", style=\"filled\"];\n";
    }

    for (auto p : graph->edges) {
        auto e = p.second;
        auto left = e->from;
        auto right = e->to;
        // left
        sb << "\"<<" << nodeTypeEnumToAbbr(left->type) << ">>\n"
           << left->name << "\" -> ";
        // right
        sb << "\"<<" << nodeTypeEnumToAbbr(right->type) << ">>\n"
           << right->name << "\" ";
        //
        if (e->type == PFGEdgeType::Constraint) {
            sb << "[label=\"" << edgeTypeEnumToAbbr(e->type) << "\", dir=none, color=\"black:invis:black\", style=\"dashed\"];\n";
        } else if (e->type == PFGEdgeType::Assignment) {
            sb << "[label=\"" << edgeTypeEnumToAbbr(e->type) << "\", color=\"darkgreen\", arrowhead=\"vee\"];\n";
        }
    }
    sb << "}\n";
    std::cerr << sb.str();
}

namespace {
struct PrintGraphviz : public ModulePass {
    static char ID;

    PrintGraphviz() : ModulePass(ID) {}

    bool runOnModule(Module& M) override {
        auto graphs = initDetectedGraphs(M, true, true);
        for (auto g : graphs) {
            printGraphviz(g);
        }
        return false;
    };
};
}  // namespace

char PrintGraphviz::ID = 0;
static RegisterPass<PrintGraphviz> X("PrintGraphviz",
                                     "Print the graph in dot format.", false,
                                     false);
