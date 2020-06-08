
#include <gcc_4_7_2/specialization/rtl_specialization.hh>

#include <unordered_map>

#include <graph_abstractions/cfg_interface.hh>


namespace distillery {

    template<>
    StatementInterface<RtlSpecialization>::StatementInterface(const rtx &stmt1)
            :stmt(stmt1) {}

    template<>
    BlockInterface<RtlSpecialization>::BlockInterface(const basic_block &bb1) {
        rtx stmt;
        FOR_BB_INSNS(bb1, stmt) {
            statements.emplace_back(new StatementInterface<RtlSpecialization>(stmt));
        }
    }

    template<>
    CfgInterface<RtlSpecialization>::CfgInterface(::function &fun) {
        this->fun_name = get_name(fun.decl);

        std::unordered_map<basic_block, BlockInterface<RtlSpecialization> *> corresponding_interface;

        basic_block bb = nullptr;
        FOR_ALL_BB_FN(bb, &fun) {

            blocks.emplace_back(new BlockInterface<RtlSpecialization>(bb));
            blocks.back()->set_block_id(bb->index);
            corresponding_interface[bb] = blocks.back().get();
        }
        FOR_ALL_BB_FN(bb, &fun) {

            edge edge1 = nullptr;
            edge_iterator edge_itr;
            FOR_EACH_EDGE(edge1, edge_itr, bb->succs) {
                if(corresponding_interface.find(edge1->dest) == corresponding_interface.end()) continue;
                if(not edge1->dest) continue;

                corresponding_interface[bb]->add_successor(corresponding_interface[edge1->dest]);
            }

            FOR_EACH_EDGE(edge1, edge_itr, bb->preds) {
                if(corresponding_interface.find(edge1->src) == corresponding_interface.end()) continue;
                if(not edge1->src) continue;

                corresponding_interface[bb]->add_predecessor(corresponding_interface[edge1->src]);
            }
        }

        this->m_entry = corresponding_interface[fun.cfg->x_entry_block_ptr];
        this->m_exit = corresponding_interface[fun.cfg->x_exit_block_ptr];
    }

    template <>
    void StatementInterface<RtlSpecialization>::dump(FILE *out) const {
        print_rtl_single(out, get());
    }

    template <>
    std::ostream& operator<<(std::ostream& out, const StatementInterface<RtlSpecialization> &stmt) {
        return out;
    }
}