#ifndef DISTILLERY_DATA_FLOW_OBJECT_HH
#define DISTILLERY_DATA_FLOW_OBJECT_HH

#include <graph_abstractions/cfg_interface.hh>
#include <memory>
#include <unordered_map>

namespace distillery {
template<
  typename GraphType,
  typename Value>
struct DataFlowObject
{

  using Cfg = typename GraphType::Cfg;
  using Block = typename GraphType::Block;
  using Statement = typename GraphType::Statement;

  const Cfg &cfg;

  std::unordered_map<const Block *, std::unique_ptr<Value>> block_in, block_out;
  std::unordered_map<const Statement *, std::unique_ptr<Value>> statement_in, statement_out;

  explicit DataFlowObject(const Cfg &cfg1) : cfg(cfg1)
  {
  }

  void dump(FILE *out = stdout)
  {
    for (auto &block : cfg) {
      fprintf(out, "Block : %d\nBlock In : \n", block->block_id());
      block_in[block.get()]->dump(out);
      fprintf(out, "\n");
      block->dump(out);
      fprintf(out, "Block Out : \n");
      block_out[block.get()]->dump(out);
      fprintf(out, "\n");
    }
  }
};
}// namespace distillery

#endif//DISTILLERY_DATA_FLOW_OBJECT_HH
