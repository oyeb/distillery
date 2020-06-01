#ifndef DISTILLERY_PROGRAM_ANALYSIS_HH
#define DISTILLERY_PROGRAM_ANALYSIS_HH

#include <memory>
#include <map>

#include <program_analysis/data_flow_object.hh>
#include <graph_abstractions/cfg_interface.hh>


namespace distillery {
  template <
    typename GraphType,
    typename Value
    >
  class ProgramAnalysis {

    using Cfg = typename GraphType::Cfg;
    using Block = typename GraphType::Block;
    using Statement = typename GraphType::Statement;

  public:
    virtual std::unique_ptr<Value> copy(const Value &value) = 0;

    virtual std::unique_ptr<Value> meet(const Value &value_1, const Value &value_2) = 0;

    virtual std::unique_ptr<Value> boundary_value(const Cfg &cfg) = 0;

    virtual std::unique_ptr<Value> topValue() = 0;

    virtual std::unique_ptr<DataFlowObject<GraphType, Value>> doAnalysis(const Cfg &cfg) = 0;

    virtual std::unique_ptr<DataFlowObject<GraphType, Value>> doAnalysisFromValue(const Cfg &cfg, const Value &value) = 0;

    virtual std::unique_ptr<Value> post(const Block &block, const Value &value) = 0;

    virtual std::unique_ptr<Value> post_statement(const Statement &statement, const Value &value) = 0;

    virtual bool is_equal(const Value &value_1, const Value &value_2) = 0;
  };
}

#endif //DISTILLERY_PROGRAM_ANALYSIS_HH
