
#include <graph_abstractions/merge_cfg.hh>

#include <gcc_4_7_2/specialization/rtl_specialization.hh>


namespace distillery {

    using RtlRtlSpecialization = MergedGraphType<InterfaceWrapper<RtlSpecialization>, InterfaceWrapper<RtlSpecialization>>;

    template <>
    StatementInterface<RtlRtlSpecialization>::StatementInterface(const RtlRtlSpecialization ::Statement &stmt1):stmt(stmt1) {}

    template <>
    BlockInterface<RtlRtlSpecialization>::BlockInterface(const RtlRtlSpecialization::Block &block) {
        block_no = block.first->block_id();
        for(auto &stmt : *(block.first)) {
            statements.emplace_back(new StatementInterface<RtlRtlSpecialization>(RtlRtlSpecialization::Statement(stmt.get())));
        }

        for(auto &stmt : *(block.second)) {
            statements.emplace_back(new StatementInterface<RtlRtlSpecialization>(RtlRtlSpecialization::Statement(stmt.get(), true)));
        }
    }

    template <>
    CfgInterface<RtlRtlSpecialization>::CfgInterface(RtlRtlSpecialization::Cfg &cfg) {
        this->fun_name = cfg.first->get_function_name();

        std::unordered_map<BlockInterface<RtlSpecialization>*, BlockInterface<RtlRtlSpecialization>*> m_map;
        auto src_iter = cfg.first->begin();
        auto dest_iter = cfg.second->begin();
        while(src_iter != cfg.first->end()) {
//
//            std::pair<BlockInterface<GimpleSpecialization>*, BlockInterface<RtlSpecialization>*> s =
//                    {&(**src_iter), &(**dest_iter)};
//            std::unique_ptr<Specialization

            blocks.emplace_back(new BlockInterface<RtlRtlSpecialization>({&(**src_iter), &(**dest_iter)}));
            m_map[&(**src_iter)] = blocks.back().get();
            src_iter++;
            dest_iter++;
        }

        for(auto &block : *cfg.first) {
            for(auto *succ : block->successors()) {
                m_map[block.get()]->add_successor(m_map[succ]);
            }
            for(auto *pred : block->predecessors()) {
                m_map[block.get()]->add_predecessor(m_map[pred]);
            }
        }
        for(auto &block : blocks) {
            if(block->predecessors().size() == 0) {
                m_entry = block.get();
            }

            if(block->successors().size() == 0) {
                m_exit = block.get();
            }
        }
    }

    template <>
    void StatementInterface<RtlRtlSpecialization>::dump(FILE *out) const {
        // if(this->get().m_is_first) {
        //     print_gimple_stmt(out, this->get().first->get(), 0, 0);
        // } else {
            print_rtl_single(out, this->get().second->get());
        // }
    }
}