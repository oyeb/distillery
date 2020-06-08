//
// Created by ajeesh on 4/10/20.
//

#ifndef RTL_TRANSLATION_VALIDATION_RTL_TO_Z3_HH
#define RTL_TRANSLATION_VALIDATION_RTL_TO_Z3_HH

#include <z3++.h>

#include <gcc_4_7_2/program_analysis/block_validation_analysis/block_validation_analysis.hh>


namespace distillery {
	class RtlToZ3 {
	public:
		static z3::expr convert(const rtx &stmt, ValidationContext &validation_context, bool is_first) {
			return process_use(stmt, validation_context, is_first);
		}
		static z3::expr process_use(const rtx insn, ValidationContext &validation_context, bool is_first) {
			// z3::sort m_int_array_sort = 
			//         validation_context.ctx.array_sort(
			//                 validation_context.ctx.int_sort(),
			//                 validation_context.ctx.int_sort()
			//                 );
			std::string name_cfg;
			if(is_first){
				name_cfg = "src";
			}
			else{
				name_cfg = "dest";
			}

			switch (GET_CODE(insn)) {
				case INSN_LIST: {
					z3::expr z3_expr_head = validation_context.ctx.bool_val(true);
					z3::expr z3_expr_tail = validation_context.ctx.bool_val(true);

					auto head = XEXP(insn, 0);
					auto tail = XEXP(insn, 1);

					if(head) {
						z3_expr_head = process_use(head, validation_context, is_first);
					}

					if(tail) {
						z3_expr_tail = process_use(tail, validation_context, is_first);
					}

					return (z3_expr_head && z3_expr_tail);
				}
				case INSN: {
					if (auto sub_exp = XEXP(insn, 4)) {
						return process_use(sub_exp, validation_context, is_first);
					} else {
						print_rtl_single(stderr, insn);
						throw distillery::Exception(std::string("Empty expression in rtl INSN"));
					}
				}
				case JUMP_INSN: {
					if (auto sub_exp = XEXP(insn, 4)) {
						return process_use(sub_exp, validation_context, is_first);
					}
					else {
						throw distillery::Exception(std::string("No sub_exp in jmp"));
					}
				}
				case CODE_LABEL: {
					return validation_context.ctx.bool_val(true);
				}
				case NOTE: {
					return validation_context.ctx.bool_val(true);
				}
				case PARALLEL: {
					z3::expr z3_expr_cumulative = z3::expr(validation_context.ctx.bool_val(true));
					if(XVEC(insn, 0)) {
						int len = XVECLEN(insn, 0);
						for(int i = 0; i < len; ++i) {
							z3_expr_cumulative = z3_expr_cumulative && process_use(XVECEXP(insn, 0, i), validation_context, is_first);
						}
					}
					return z3_expr_cumulative;
				}
				case SET: {
					z3::expr true_cond(validation_context.ctx), false_cond(validation_context.ctx);
					auto lhs = XEXP(insn, 0);
					if(GET_CODE(lhs) == PC){
						std::tie(true_cond, false_cond) = get_conditions_of_jmp(XEXP(insn, 1), validation_context, is_first);
						validation_context.dest_condition.push_back(true_cond);
						validation_context.dest_condition.push_back(false_cond);
					}

					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					z3::expr z3_expr_lhs = process_def(lhs, validation_context, is_first);

					if(GET_CODE(lhs) == REG && REGNO(lhs) == 0) {
						if(is_first){
							validation_context.src_return_expr = z3_expr_lhs;
						}
						else{
							validation_context.dest_return_expr = z3_expr_lhs;
						}
						validation_context.define_return=true;
					}

					z3::expr ret(validation_context.ctx);
					// if(not validation_context.temp_define) {
					//     ret = z3::store
					//                   (
					//                           validation_context.ctx.constant(
					//                                   (is_first + std::string("_int_memory_") +
					//                                    std::to_string(validation_context.dest_int_mem_no)).c_str(),
					//                                   m_int_array_sort),
					//                           z3_expr_lhs,
					//                           z3_expr_rhs
					//                   )
					//           ==
					//           (
					//                   validation_context.ctx.constant(
					//                           (is_first + std::string("_int_memory_") +
					//                            std::to_string(validation_context.dest_int_mem_no + 1)).c_str(),
					//                           m_int_array_sort)
					//           );
					//     validation_context.dest_int_mem_no++;
					// } else {
					ret = (z3_expr_lhs == z3_expr_rhs);
					// }
					return ret;
				}
				case USE:
					return validation_context.ctx.bool_val(true);
				case CLOBBER: {
					process_def(XEXP(insn, 0), validation_context, is_first);
					// if(not validation_context.temp_define) {
					//     validation_context.dest_int_mem_no++;
					// }
					return z3::expr(validation_context.ctx.bool_val(true));
				}
				case CONST_INT:
					return z3::expr(validation_context.ctx.int_val(XINT(insn, 0)));
				case PC:
				{
					std::string variable;
					if(is_first){
						if(validation_context.src_temp_variables.find("pc_src")==validation_context.src_temp_variables.end()){
							validation_context.src_temp_variables.insert("pc_src");
							validation_context.src_version_no["pc_src"]=0;
							variable = variable + "_0";
						}
						else{
							variable = "pc_" + name_cfg + "_" + std::to_string(validation_context.src_version_no["pc_src"]);
						}
					}
					else{
						if(validation_context.dest_temp_variables.find("pc_dest")==validation_context.dest_temp_variables.end()){
							validation_context.dest_temp_variables.insert("pc_dest");
							validation_context.dest_version_no["pc_dest"]=0;
							variable = variable + "_0";
						}
						else{
							variable = "pc_" + name_cfg + "_" + std::to_string(validation_context.dest_version_no["pc_dest"]);
						}
					}
					// return z3::select(validation_context.ctx.constant((is_first + std::string("_int_memory_") + std::to_string(validation_context.dest_int_mem_no)).c_str(),
					//                                     m_int_array_sort),
					//                   validation_context.ctx.int_const(variable.c_str()));
					return validation_context.ctx.int_const(variable.c_str());
				}
				case REG:
				{
					std::string variable;
					unsigned version;

					// if (REG_USERVAR_P(insn))
					// {
					//     version = 0;
					//     variable = "t_" + is_first + "_" + std::to_string(version);
					// }
					// else
					// {
					variable = "r_" + name_cfg + "_" + std::to_string(REGNO(insn));
					if(is_first){
						if(validation_context.src_temp_variables.find(variable)==validation_context.src_temp_variables.end()){
							validation_context.src_temp_variables.insert(variable);
							validation_context.src_version_no[variable]=0;
							variable = variable + "_0";
						}
						else{
							variable = variable + "_" + std::to_string(validation_context.src_version_no["pc_src"]);
						}
						// variable = variable + "_" + std::to_string(validation_context.src_version_no[variable]);
					}
					else{
						if(validation_context.dest_temp_variables.find(variable)==validation_context.dest_temp_variables.end()){
							validation_context.dest_temp_variables.insert(variable);
							validation_context.dest_version_no[variable]=0;
							variable = variable + "_0";
						}
						else{
							variable = variable + "_" + std::to_string(validation_context.dest_version_no[variable]);
						}
						// variable = variable + "_" + std::to_string(validation_context.dest_version_no[variable]);
					}
					// validation_context.dest_temp_variables.insert(variable);
					return validation_context.ctx.int_const(variable.c_str());
					// }

					// return z3::select(validation_context.ctx.constant((is_first + std::string("_int_memory_") + std::to_string(validation_context.dest_int_mem_no)).c_str(),
					//                                     m_int_array_sort),
					//                   validation_context.ctx.int_const(variable.c_str()));
				}
				case MEM:
				{
					if(auto gimple_node = MEM_EXPR(insn))
					{

						std::string variable = get_name(gimple_node);
						if(is_first){
							if(validation_context.src_temp_variables.find(variable)==validation_context.src_temp_variables.end()){
								validation_context.src_temp_variables.insert(variable);
								validation_context.src_version_no[variable]=0;
								variable = variable + "_0";
							}
							else{
								variable = variable + "_" + std::to_string(validation_context.src_version_no[variable]);
							}
							// variable = variable + "_" + std::to_string(validation_context.src_version_no[variable]);
						}
						else{
							if(validation_context.dest_temp_variables.find(variable)==validation_context.dest_temp_variables.end()){
								validation_context.dest_temp_variables.insert(variable);
								validation_context.dest_version_no[variable]=0;
								variable = variable + "_0";
							}
							else{
								variable = variable + "_" + std::to_string(validation_context.dest_version_no[variable]);
							}
						}
						// validation_context.dest_temp_variables.insert(variable);
						return validation_context.ctx.int_const(variable.c_str());
						// return z3::select(validation_context.ctx.constant((is_first + std::string("_int_memory_") + std::to_string(validation_context.dest_int_mem_no)).c_str(),
						//                                     m_int_array_sort),
						//                   validation_context.ctx.int_const(variable.c_str()));
					} else if(auto symbol_ref = SYMBOL_REF_CONSTANT(insn)) {

						
					}
					assert(false);
				}
					break;
				case LABEL_REF:
				{
					std::string label = std::string("label_") + name_cfg + std::string("_") + std::to_string(XINT(XEXP(insn, 0), 5));
					if(is_first){
						label = label + "_" + std::to_string(validation_context.src_version_no[label]);
					}
					else{
						label = label + "_" + std::to_string(validation_context.dest_version_no[label]);
					}
					return validation_context.ctx.int_const(label.c_str());
				}
				case IF_THEN_ELSE:
				{
					z3::expr z3_cond_expr = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_true_branch = process_use(XEXP(insn, 1), validation_context, is_first);
					z3::expr z3_false_branch = process_use(XEXP(insn, 2), validation_context, is_first);

					return z3::ite(z3_cond_expr, z3_true_branch, z3_false_branch);
				}
				case COMPARE:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs - z3_expr_rhs;

				}
				case PLUS:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs + z3_expr_rhs;
				}
				case MINUS:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs - z3_expr_rhs;
				}
				case MULT:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs * z3_expr_rhs;
				}
				case DIV:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs / z3_expr_rhs;
				}
				case NE:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs != z3_expr_rhs;
				}
				case EQ:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs == z3_expr_rhs;
				}
				case GE:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs >= z3_expr_rhs;
				}
				case GT:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs > z3_expr_rhs;
				}
				case LE:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs <= z3_expr_rhs;
				}
				case LT:
				{
					z3::expr z3_expr_lhs = process_use(XEXP(insn, 0), validation_context, is_first);
					z3::expr z3_expr_rhs = process_use(XEXP(insn, 1), validation_context, is_first);
					return z3_expr_lhs < z3_expr_rhs;
				}
				case CALL_INSN:
				{
					auto arg_list = XEXP(insn, 8);
					if(is_first){
						validation_context.src_function_call_arguments.emplace_back();
						while(arg_list) {
							auto arg = XEXP(arg_list, 0);
							arg_list = XEXP(arg_list, 1);
							auto arg_z3 = process_use(arg, validation_context, is_first);
							validation_context.src_function_call_arguments.back().push_back(arg_z3);
						}

						for(auto &v : validation_context.src_function_call_arguments.back()) {
							std::cout << v << " ";
						}
						std::cout << std::endl;

						return process_use(XEXP(insn, 4), validation_context, is_first);
					}
					else{
						validation_context.dest_function_call_arguments.emplace_back();
						while(arg_list) {
							auto arg = XEXP(arg_list, 0);
							arg_list = XEXP(arg_list, 1);
							auto arg_z3 = process_use(arg, validation_context, is_first);
							validation_context.dest_function_call_arguments.back().push_back(arg_z3);
						}

						for(auto &v : validation_context.dest_function_call_arguments.back()) {
							std::cout << v << " ";
						}
						std::cout << std::endl;

						return process_use(XEXP(insn, 4), validation_context, is_first);
					}
				}
				case CALL:
				{
					auto fn = XEXP(insn, 0);
//                    CALL_EXPR_FN()
					return validation_context.ctx.bool_val(true);
				}

			}
			throw distillery::Exception(std::string("Node not supported : ") + GET_RTX_NAME(GET_CODE(insn)));

		}

		static z3::expr process_def(const rtx &exp, ValidationContext &validation_context, bool is_first) {
			validation_context.temp_define = false;
			std::string variable;
			unsigned version;
			std::string name_cfg;
			if(is_first){
				name_cfg = "src";
			}
			else{
				name_cfg = "dest";
			}

			if(GET_CODE(exp) == REG) {
				// if(MEM_P(exp)) {
				//     auto node = MEM_EXPR(exp);

				//     version = 0;
				//     variable = "t_" + is_first + "_" + std::to_string(version);
				// }
				// else {
				variable = "r_" + name_cfg + "_" + std::to_string(REGNO(exp));
				if(is_first){
					if(validation_context.src_temp_variables.find(variable) == validation_context.src_temp_variables.end()){
						validation_context.src_temp_variables.insert(variable);
						validation_context.src_version_no[variable]=0;
						variable = variable + "_0";
					}
					else{
						validation_context.src_version_no[variable]++;
						variable = variable + "_" + std::to_string(validation_context.src_version_no[variable]);
					}
				}
				else{
					if(validation_context.dest_temp_variables.find(variable) == validation_context.dest_temp_variables.end()){
						validation_context.dest_temp_variables.insert(variable);
						validation_context.dest_version_no[variable]=0;
						variable = variable + "_0";
					}
					else{
						validation_context.dest_version_no[variable]++;
						variable = variable + "_" + std::to_string(validation_context.dest_version_no[variable]);
					}
				}
				// validation_context.dest_temp_variables.insert(variable);
				// validation_context.temp_define  = true;
				return validation_context.ctx.int_const(variable.c_str());
				// }
			}
			else if(GET_CODE(exp) == MEM){
				if(tree gimple_node = MEM_EXPR(exp)){
					variable = get_name(gimple_node);
				}
				else{
					assert(false);
				}
			}
			else if(GET_CODE(exp) == PC){
				variable = "pc_" + name_cfg;
				// validation_context.temp_define = true;
				// validation_context.dest_temp_variables.insert(variable);
				// return validation_context.ctx.int_const(variable.c_str());
			}
			else
			{
				assert(false);
			}
			if(is_first){
				if(validation_context.src_temp_variables.find(variable) == validation_context.src_temp_variables.end()){
					validation_context.src_temp_variables.insert(variable);
					validation_context.src_version_no[variable]=0;
					variable = variable + "_0";
				}
				else{
					validation_context.src_version_no[variable]++;
					variable = variable + "_" + std::to_string(validation_context.src_version_no[variable]);
				}
			}
			else{
				if(validation_context.dest_temp_variables.find(variable) == validation_context.dest_temp_variables.end()){
					validation_context.dest_temp_variables.insert(variable);
					validation_context.dest_version_no[variable]=0;
					variable = variable + "_0";
				}
				else{
					validation_context.dest_version_no[variable]++;
					variable = variable + "_" + std::to_string(validation_context.dest_version_no[variable]);
				}
			}
			return validation_context.ctx.int_const(variable.c_str());
		}

		static std::pair<z3::expr, z3::expr> get_conditions_of_jmp(const rtx insn, ValidationContext &validation_context, bool is_first) {
			std::vector<z3::expr> result;
			if(GET_CODE(insn) == IF_THEN_ELSE) {
				return {process_use(XEXP(insn, 1), validation_context, is_first), process_use(XEXP(insn, 2), validation_context, is_first)};
			} else if(GET_CODE(insn) == LABEL_REF) {
				return {process_use(insn, validation_context, is_first), z3::expr(validation_context.ctx)};
			}
			print_rtl_single(stderr, insn);
			throw distillery::Exception("assignment to pc with unsupported type");
		}

	};
}


#endif //RTL_TRANSLATION_VALIDATION_RTL_TO_Z3_HH
