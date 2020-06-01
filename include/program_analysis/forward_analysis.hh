#ifndef DISTILLERY_FORWARD_ANALYSIS_HH
#define DISTILLERY_FORWARD_ANALYSIS_HH

#include <program_analysis/program_analysis.hh>
#include <memory>
#include <queue>
#include <map>

namespace distillery {
  template <
    typename GraphType,
    typename Value
    >
  class ForwardAnalysis : public ProgramAnalysis<GraphType, Value> {
  public:
    using Cfg = typename GraphType::Cfg;
    using Block = typename GraphType::Block;
    using Statement = typename GraphType::Statement;

    std::unique_ptr<DataFlowObject<GraphType, Value>> doAnalysisFromValue(const Cfg &cfg, const Value &boundary_value) override {
      auto result = std::unique_ptr<DataFlowObject<GraphType, Value>>(new DataFlowObject<GraphType, Value>(cfg));

      std::queue<const Block*> work_list;

      for(const auto& block : cfg) {
        result->block_out[block.get()] = this->topValue();
        result->block_in[block.get()] = this->topValue();
        work_list.push(block.get());
      }

      result->block_in[cfg.entry()] = this->copy(boundary_value);

      while(not work_list.empty()) {
        const auto *current_block = work_list.front();
        work_list.pop();

        auto block_in_current = this->copy(*result->block_in[current_block]);
        for(const auto *predecessor : current_block->predecessors()) {
          block_in_current = this->meet(*block_in_current, *result->block_out[predecessor]);
        }

        auto block_out_current = this->post(*current_block, *block_in_current);
        result->block_in[current_block] = std::move(block_in_current);
        // block_in_current is invalid from this line

        if(not this->is_equal(*result->block_out[current_block], *block_out_current)) {
          result->block_out[current_block] = std::move(block_out_current);
          // block_out_current is invalid from this line

          for(const auto* successor : current_block->successors()) {
            work_list.push(successor);
          }
        }
      }

      return result;
    }

    std::unique_ptr<DataFlowObject<GraphType, Value>> doAnalysis(const Cfg &cfg) override {

      auto result = std::unique_ptr<DataFlowObject<GraphType, Value>>(new DataFlowObject<GraphType, Value>(cfg));

      std::queue<const Block*> work_list;

      for(const auto& block : cfg) {
        result->block_out[block.get()] = this->topValue();
        result->block_in[block.get()] = this->topValue();
        work_list.push(block.get());
      }

      result->block_in[cfg.entry()] = this->boundary_value(cfg);

      while(not work_list.empty()) {
        const auto *current_block = work_list.front();
        work_list.pop();

        auto block_in_current = this->copy(*result->block_in[current_block]);
        for(const auto *predecessor : current_block->predecessors()) {
          block_in_current = this->meet(*block_in_current, *result->block_out[predecessor]);
        }

        auto block_out_current = this->post(*current_block, *block_in_current);
        result->block_in[current_block] = std::move(block_in_current);
        // block_in_current is invalid from this line

        if(not this->is_equal(*result->block_out[current_block], *block_out_current)) {
          result->block_out[current_block] = std::move(block_out_current);
          // block_out_current is invalid from this line

          for(const auto* successor : current_block->successors()) {
            work_list.push(successor);
          }
        }
      }

      return result;
    }


    std::unique_ptr<Value> post(const Block &block, const Value &value) override {

      auto result = this->copy(value);
      for(const auto& statement : block) {
        result = this->post_statement(*statement, *result);
      }
      return result;
    }
  };
}

#endif //DISTILLERY_FORWARD_ANALYSIS_HH
