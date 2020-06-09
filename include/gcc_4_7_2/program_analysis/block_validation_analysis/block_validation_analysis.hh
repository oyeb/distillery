//
// Created by ajeesh on 3/31/20.
//

#ifndef RTL_TRANSLATION_VALIDATION_BLOCK_VALIDATION_HH
#define RTL_TRANSLATION_VALIDATION_BLOCK_VALIDATION_HH

#include <pre-compiled-header.hh>

#include <unordered_set>
#include <string>

#include <program_analysis/data_flow_object.hh>
#include <program_analysis/forward_analysis.hh>
#include <utils/exceptions.hh>
#include <gcc_4_7_2/program_analysis/block_validation_analysis/validation_context.hh>
#include <gcc_4_7_2/program_analysis/block_validation_analysis/block_validation_analysis.hh>
#include <managers/manager.hh>

namespace distillery {

	struct EqualityGroup {
		bool top{false};
		std::set<std::set<std::string>> groups{};

		EqualityGroup() = default;

		explicit EqualityGroup(bool is_top):top(is_top){}

		bool operator==(const EqualityGroup &other) const {
			return (top == other.top) && (groups == other.groups);
		}
	};

	template <typename GraphType>
	class Validation : public ForwardAnalysis<GraphType, EqualityGroup> {
		using FAnalysis = ForwardAnalysis<GraphType, EqualityGroup>;
		using Value = EqualityGroup;
		using Cfg = typename GraphType::Cfg;
		using Block = typename GraphType::Block;
		using Statement = typename GraphType::Statement;

	public:
		std::unique_ptr<Value> copy(const Value &value) override {
			return std::unique_ptr<Value>(new Value(value));
		}

		std::unique_ptr<Value> meet(const Value &value_1, const Value &value_2) override {
			if(value_1.top) return std::unique_ptr<Value>(new Value(value_2));
			if(value_2.top) return std::unique_ptr<Value>(new Value(value_1));

			auto result = std::unique_ptr<Value>(new Value);

			std::unordered_map<std::string, unsigned> groups;
			unsigned group_no = 1;
			for(const auto &group : value_1.groups) {
				for(const auto &var : group) {
					groups[var] = group_no;
				}
				group_no++;
			}

			for(auto group1 : value_2.groups) {
				for(const auto &var1 : group1) {
					if(groups.find(var1) == groups.end()) {
						group1.erase(var1);
						continue;
					}

					std::set<std::string> current_equality_set;
					current_equality_set.insert(var1);
					for(const auto &var2 : group1) {
						if(groups.find(var2) == groups.end()) {
							continue;
						}

						if(groups[var1] == groups[var2]) {
							current_equality_set.insert(var2);
						}
					}

					if(current_equality_set.size() > 1) {
						result->groups.insert(current_equality_set);
					}

					for(const auto &var : current_equality_set) {
						group1.erase(var);
					}
				}
			}
			return result;
		}

		std::unique_ptr<Value> boundary_value() override {
			return std::unique_ptr<Value>(new Value);
		}

		std::unique_ptr<Value> post_statement(const Statement &statement, const Value &value) override {
			throw distillery::Exception("Doing Post on Statement in validation");
		}

		std::unique_ptr<Value> topValue() override {
			return std::unique_ptr<Value>(new Value(true));
		}

		bool is_equal(const Value &value_1, const Value &value_2) override {
			return value_1 == value_2;
		}

		std::unique_ptr<Value> post(const Block &block, const Value &equality_groups) override {

			ValidationContext context{};
			z3::context &z3_ctx = context.ctx;
			z3::solver z3_solver(context.ctx);
			// z3::sort m_int_array_sort = z3_ctx.array_sort(z3_ctx.int_sort(), z3_ctx.int_sort());

			for(const auto &group : equality_groups.groups) {
				for(const auto &var : group) {
					TV_RTL_LOGGER.LOG(INFO) << var << " ";
				}
			}

			for(const auto &group : equality_groups.groups) {
				for(const auto &var : group) {
					z3_solver.add(
							context.ctx.int_const((var + "_0").c_str()) ==
							context.ctx.int_const((*(group.begin()) + "_0").c_str())
							);
				}
			}

			// z3_solver.add(
			//         z3_ctx.constant("src_int_memory_0", m_int_array_sort) ==
			//         z3_ctx.constant("dest_int_memory_0", m_int_array_sort)
			//         );

			for(const auto &stmt : block) {
				z3_solver.add(convert_to_z3_expr(*stmt, context));
			}

			z3_solver.push();
			// auto src_final_int_mem = std::string("src_int_memory_") + std::to_string(context.src_int_mem_no);
			// auto dest_final_int_mem = std::string("dest_int_memory_") + std::to_string(context.dest_int_mem_no);
			// z3_solver.add(
			//         z3_ctx.constant(src_final_int_mem.c_str(), m_int_array_sort) !=
			//         z3_ctx.constant(dest_final_int_mem.c_str(), m_int_array_sort)
			//         );
			TV_RTL_LOGGER.LOG(INFO) << "Solver" << std::endl;
			if(context.define_return){
				z3_solver.add(context.src_return_expr != context.dest_return_expr);
				TV_RTL_LOGGER.LOG(INFO) << "Sat Check" << std::endl;
				TV_RTL_LOGGER.LOG() << z3_solver << std::endl;
				if(z3_solver.check() != z3::unsat) {
					// throw distillery::Exception("Validation Failed");
					std::cout<<"Validation Failed"<<std::endl;
				}
			}
			else{
				TV_RTL_LOGGER.LOG() << z3_solver << std::endl;
			}
			z3_solver.pop();
			auto result = std::unique_ptr<EqualityGroup>(new EqualityGroup);
			{
				std::set<std::string> eq_set;
				// eq_set.insert(context.src_temp_variables.begin(), context.src_temp_variables.end());
				for(const auto &var : context.src_temp_variables){
					eq_set.insert(var + "_" + std::to_string(context.src_version_no[var]));
				}
				// eq_set.insert(context.dest_temp_variables.begin(), context.dest_temp_variables.end());
				for(const auto &var : context.dest_temp_variables){
					eq_set.insert(var + "_" + std::to_string(context.dest_version_no[var]));
				}
				result->groups.insert(eq_set);
			}
			z3_solver.push();
			while(z3_solver.check() == z3::sat) {
				auto model = z3_solver.get_model();
				TV_RTL_LOGGER.LOG() << model << std::endl;

				std::map<std::pair<std::string, unsigned>, std::set<std::string>> common_groups;
				unsigned group_no = 0;
				for (const auto &eq_set : result->groups) {
					for (const auto &var : eq_set) {
						auto eval_result = model.eval(z3_ctx.int_const(var.c_str())).to_string();
						common_groups[{eval_result, group_no}].insert(var);
					}
				}

				result->groups.clear();

				for (const auto &p : common_groups) {
					if (p.second.size() <= 1) continue;
					std::set<std::string> group;
					group.insert(p.second.begin(), p.second.end());
					result->groups.insert(std::move(group));
				}

				z3::expr constr = z3_ctx.bool_val(false);
				for(const auto &eq : result->groups) {
					z3::expr acc = z3_ctx.bool_val(true);
					// TV_RTL_LOGGER.LOG() << "eqqqqq{ ";
					for(const auto &v : eq) {
						// TV_RTL_LOGGER.LOG() << v << " ";
						acc = acc && (z3_ctx.int_const(v.c_str()) == z3_ctx.int_const(eq.begin()->c_str()));
					}
					// TV_RTL_LOGGER.LOG() << " }" << std::endl;
					constr = constr || (not acc);
				}
				z3_solver.add(constr);
				// TV_RTL_LOGGER.LOG() << z3_solver << std::endl;

			}
			z3_solver.pop();
			auto ret = std::unique_ptr<EqualityGroup>(new EqualityGroup);
			for(const auto &eq : result->groups) {
				std::set<std::string> gp;
				for(const auto &v : eq) {
					gp.insert(context.getNameRemoveVersion(v));
				}
				ret->groups.insert(std::move(gp));
			}
			result.reset();
			TV_RTL_LOGGER.LOG() << "sets: ";
			for(const auto &gp : ret->groups){
				TV_RTL_LOGGER.LOG() << "{ ";
				for(const auto &var : gp){
					TV_RTL_LOGGER.LOG() << var << " ";
				}
				TV_RTL_LOGGER.LOG() << " }" << std::endl;
			}
			TV_RTL_LOGGER.LOG() << std::endl;
			return ret;
		}

		static z3::expr convert_to_z3_expr(const Statement &stmt, ValidationContext &ctx);
	};
}

#endif //RTL_TRANSLATION_VALIDATION_BLOCK_VALIDATION_HH
