
#ifndef RTL_TRANSLATION_VALIDATION_LIVENESS_UPWARD_EXPOSED_USES_HH
#define RTL_TRANSLATION_VALIDATION_LIVENESS_UPWARD_EXPOSED_USES_HH

#include <pre-compiled-header.hh>

#include <unordered_set>
#include <string>

#include <program_analysis/backward_analysis.hh>

namespace distillery {

	struct StatementSet{
		std::unordered_set<std::string> live_variables;
		std::map<std::string, std::string> post_to_pre_mapping;


		bool operator==(const StatementSet &other) const {
			return (other.live_variables==live_variables) && (other.post_to_pre_mapping==post_to_pre_mapping);
		}

		void erase(std::string var){
			live_variables.erase(var);
			post_to_pre_mapping.erase(var);
		}

		void insert(std::string var){
			live_variables.insert(var);
		}

		void insert(std::string var, std::string var2){
			live_variables.insert(var);
			post_to_pre_mapping[var]=var2;
		}
	};

	template <typename GraphType>
	class LivenessAnalysis : public BackwardAnalysis<GraphType, StatementSet> {
		using Value = StatementSet;
		using Statement = typename GraphType::Statement;


	public:
		std::unique_ptr<Value> copy(const StatementSet &value) override {
			return std::unique_ptr<Value>(new Value(value));
		}

		std::unique_ptr<Value>
		meet(const StatementSet &value_1, const StatementSet &value_2) override {
			auto result = std::unique_ptr<Value>(new Value());

			for(auto &str : value_1.live_variables) {
				result->live_variables.insert(str);
			}
			for(auto &str : value_2.live_variables) {
				result->live_variables.insert(str);
			}
			for(std::map<std::string,std::string>::const_iterator it = value_1.post_to_pre_mapping.begin();it!=value_1.post_to_pre_mapping.end();it++){
				std::string s1, s2;
				s1 = it->first;
				s2 = it->second;
				if(value_2.post_to_pre_mapping.find(s1)!=value_2.post_to_pre_mapping.end() && value_2.post_to_pre_mapping.at(s1)==s2){
					result->post_to_pre_mapping[s1]=s2;
				}
				else if(value_2.post_to_pre_mapping.find(s1)==value_2.post_to_pre_mapping.end()){
					result->post_to_pre_mapping[s1]=s2;
				}
			}
			for(std::map<std::string,std::string>::const_iterator it=value_2.post_to_pre_mapping.begin();it!=value_2.post_to_pre_mapping.end();it++){
				std::string s1, s2;
				s1 = it->first;
				s2 = it->second;
				if(value_1.post_to_pre_mapping.find(s1)==value_1.post_to_pre_mapping.end()){
					result->post_to_pre_mapping[s1]=s2;
				}
			}

			return result;
		}

		std::unique_ptr<Value> boundary_value() override {
			return std::unique_ptr<Value>(new Value);
		}

		std::unique_ptr<Value> topValue() override {
			return std::unique_ptr<Value>(new Value);
		}

		bool is_equal(const StatementSet &value_1, const StatementSet &value_2) override {
			return value_1 == value_2;
		}

		std::unique_ptr<Value>
		post_statement(const Statement &statement, const StatementSet &value) {
			return post_statement_helper(statement, value, 1);
		}

		static std::unique_ptr<Value>
		post_statement_helper(const Statement &statement, const StatementSet &value, const bool is_first);


	};

}


#endif //RTL_TRANSLATION_VALIDATION_LIVENESS_UPWARD_EXPOSED_USES_HH
