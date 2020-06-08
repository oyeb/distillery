
#include <gcc_4_7_2/program_analysis/liveness_analysis/liveness.hh>
#include <gcc_4_7_2/specialization/rtl_specialization.hh>
#include <graph_abstractions/merge_cfg.hh>

namespace distillery {
	using RtlRtlSpecialization = InterfaceWrapper<MergedGraphType<InterfaceWrapper<RtlSpecialization>, InterfaceWrapper<RtlSpecialization>>>;

	template<>
	std::unique_ptr<StatementSet>
			LivenessAnalysis<RtlRtlSpecialization>::post_statement_helper(
					const RtlRtlSpecialization::Statement &statement,
					const StatementSet &value,
					const bool is_first) {

		std::unique_ptr<StatementSet> result;
		if(statement.get().is_first()) {
			auto stmt = statement.get().first;
			result =  LivenessAnalysis<InterfaceWrapper<RtlSpecialization>>::post_statement_helper(*stmt, value, 1);
		} else {
			auto stmt = statement.get().second;
			result = LivenessAnalysis<InterfaceWrapper<RtlSpecialization>>::post_statement_helper(*stmt, value, 0);
		}
		return result;
	}
}