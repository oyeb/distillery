#ifndef DISTILLERY_BACKWARD_ANALYSIS_HH
#define DISTILLERY_BACKWARD_ANALYSIS_HH

#include <program_analysis/program_analysis.hh>
#include <queue>

namespace distillery {
template<
  typename GraphType,
  typename Value>
class BackwardAnalysis : public ProgramAnalysis<GraphType, Value>
{
protected:
  using Cfg = typename GraphType::Cfg;
  using Block = typename GraphType::Block;
  using Statement = typename GraphType::Statement;

public:
  std::unique_ptr<DataFlowObject<GraphType, Value>> doAnalysis(const Cfg &cfg) override
  {
    auto result = std::unique_ptr<DataFlowObject<GraphType, Value>>(new DataFlowObject<GraphType, Value>(cfg));

    std::queue<const Block *> work_list;

    auto rc_iter = cfg.rbegin();
    while (rc_iter != cfg.rend()) {
      const auto *block = &(**rc_iter);
      result->block_out[block] = this->topValue();
      result->block_in[block] = this->topValue();
      work_list.push(block);
      rc_iter++;
    }

    result->block_in[cfg.exit()] = this->boundary_value(cfg);

    while (not work_list.empty()) {
      const auto *current_block = work_list.front();
      work_list.pop();

      auto block_out_current = this->copy(*result->block_out[current_block]);
      for (const auto *successor : current_block->successors()) {
        block_out_current = this->meet(*block_out_current, *result->block_in[successor]);
      }

      auto block_in_current = this->post(*current_block, *block_out_current);
      result->block_out[current_block] = std::move(block_out_current);
      // block_in_current is invalid from this line

      if (not this->is_equal(*result->block_in[current_block], *block_in_current)) {
        result->block_in[current_block] = std::move(block_in_current);
        // block_out_current is invalid from this line

        for (const auto *predecessor : current_block->predecessors()) {
          work_list.push(predecessor);
        }
      }
    }

    return result;
  }


  std::unique_ptr<Value> post(const Block &block, const Value &value) override
  {

    auto result = this->copy(value);
    auto rc_iter = block.rbegin();
    while (rc_iter != block.rend()) {
      result = this->post_statement(**rc_iter, *result);
      rc_iter++;
    }
    return result;
  }
};
}// namespace distillery

#endif//DISTILLERY_BACKWARD_ANALYSIS_HH
