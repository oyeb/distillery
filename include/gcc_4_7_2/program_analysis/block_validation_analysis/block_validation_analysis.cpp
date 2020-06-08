
#include <gcc_4_7_2/program_analysis/block_validation_analysis/block_validation_analysis.hh>
#include <graph_abstractions/cfg_interface.hh>
#include <gcc_4_7_2/specialization/rtl_specialization.hh>
#include <graph_abstractions/merge_cfg.hh>
#include <gcc_4_7_2/program_analysis/block_validation_analysis/rtl_to_z3.hh>


namespace distillery {
    using RtlRtlSpecialization =
            InterfaceWrapper<MergedGraphType<InterfaceWrapper<RtlSpecialization>, InterfaceWrapper<RtlSpecialization>>>;

    template <>
    z3::expr Validation<RtlRtlSpecialization>::convert_to_z3_expr(const Statement &statement, ValidationContext &validation_context) {
        z3::expr result(validation_context.ctx);
        if(statement.get().is_first()) {
            auto stmt = statement.get().first;
            result = RtlToZ3::convert(stmt->get(), validation_context, 1);
        } else {
            auto stmt = statement.get().second;
            result = RtlToZ3::convert(stmt->get(), validation_context, 0);
        }
        return result;
    }
}