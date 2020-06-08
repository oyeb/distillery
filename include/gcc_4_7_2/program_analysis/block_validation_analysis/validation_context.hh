#ifndef RTL_TRANSLATION_VALIDATION_VALIDATION_CONTEXT_HH
#define RTL_TRANSLATION_VALIDATION_VALIDATION_CONTEXT_HH

#include <pre-compiled-header.hh>

#include <unordered_set>
#include <string>

#include <z3++.h>

namespace distillery {
	/**
	 * Information required from parsing a Block
	 */

	struct ValidationContext {
		z3::context ctx{};
		std::unordered_set<std::string> src_temp_variables{}, dest_temp_variables{};
		std::map<std::string, int> src_version_no, dest_version_no;
		std::vector<std::vector<z3::expr>> src_function_call_arguments{}, dest_function_call_arguments{};
		z3::expr src_return_expr{ctx}, dest_return_expr{ctx};
		std::vector<z3::expr> src_condition{ctx}, dest_condition{ctx};
		bool temp_define{false};
		bool define_return{false};
		std::string getNameRemoveVersion(std::string var){
			std::string result = var;
			while(result.back()!='_'){
				result.pop_back();
			}
			result.pop_back();
			return result;
		}
	};

}
#endif //RTL_TRANSLATION_VALIDATION_VALIDATION_CONTEXT_HH
