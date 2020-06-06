#ifndef DISTILLERY_CFG_INTERFACE_HH
#define DISTILLERY_CFG_INTERFACE_HH

#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include <graph_abstractions/block_interface.hh>

namespace distillery {
template<typename GraphType>
class CfgInterface
{

  using Cfg = typename GraphType::Cfg;
  using Block = typename GraphType::Block;
  using Statement = typename GraphType::Statement;

  std::string fun_name;
  std::vector<std::unique_ptr<BlockInterface<GraphType>>> blocks;
  BlockInterface<GraphType> *m_entry{ nullptr };
  BlockInterface<GraphType> *m_exit{ nullptr };

public:
  /**
     * Specialize the constructor for the CFG interface. The constructor should populate these members:
     *   - func : reference to the original cfg. This reference is never used. Putting there if required later.
     *   - blocks : vector of blocks (Wrapped in BlockInterface class). Have to be populated in the reverse post order.
     *   - m_entry : const pointer to the entry block
     *   - m_exit : const pointer to the exit block
     *   - Also the successor and predecessor info of each block has to be updated. Use add_successor and
     *      add_predecessor functions to populate the info.
     * @param fun The cfg of function.(Not using const because, Gimple does not provide a const iterator for its cfg.)
     */
  explicit CfgInterface(Cfg &fun);

  const std::string &get_function_name() const
  {
    return fun_name;
  }

  BlockInterface<GraphType> *entry()
  {
    return m_entry;
  }

  const BlockInterface<GraphType> *entry() const
  {
    return m_entry;
  }

  BlockInterface<GraphType> *exit()
  {
    return m_exit;
  }

  const BlockInterface<GraphType> *exit() const
  {
    return m_exit;
  }

  void set_entry(const BlockInterface<GraphType> *node)
  {
    this->m_entry = node;
  }

  void set_exit(const BlockInterface<GraphType> *node)
  {
    this->m_exit = node;
  }

  std::size_t size() const
  {
    return blocks.size();
  }

  void dump(FILE *out = stdout)
  {
    fprintf(out, "Function : %s\n", fun_name.c_str());
    for (auto &block : blocks) {
      block->dump(out);
    }
  }

  void simplify()
  {
    std::unordered_set<BlockInterface<GraphType> *> reachable;
    std::queue<BlockInterface<GraphType> *> work_list;
    work_list.push(m_entry);
    while (not work_list.empty()) {
      BlockInterface<GraphType> *block = work_list.front();
      work_list.pop();
      reachable.insert(block);
      for (BlockInterface<GraphType> *succ : block->successors()) {
        if (reachable.find(succ) == reachable.end()) {
          work_list.push(succ);
        }
      }
    }

    std::vector<std::unique_ptr<BlockInterface<GraphType>>> duplicate_blocks = std::move(blocks);
    blocks.clear();

    std::unordered_set<BlockInterface<GraphType> *> deleted_nodes;

    for (std::unique_ptr<BlockInterface<GraphType>> &block : duplicate_blocks) {
      if (!block) continue;

      if (deleted_nodes.find(block.get()) != deleted_nodes.end()) continue;
      if (reachable.find(block.get()) == reachable.end()) continue;

      auto successor_count = [](const BlockInterface<GraphType> *block_ptr) -> unsigned {
        return block_ptr->successors().size();
      };

      auto predecessor_count = [&reachable](const BlockInterface<GraphType> *block_ptr) -> unsigned {
        unsigned result{ 0 };
        for (auto *pred : block_ptr->predecessors()) {
          if (reachable.find(pred) != reachable.end()) {
            result++;
          }
        }
        return result;
      };

      while (block) {
        if (successor_count(block.get()) != 1) {
          break;
        }

        if (predecessor_count(block->successors()[0]) != 1) {
          break;
        }

        deleted_nodes.insert(block->successors()[0]);
        block->merge(*block->successors()[0]);
      }

      blocks.push_back(std::move(block));
    }

    for (auto &block : blocks) {
      block->predecessors().clear();
    }

    for (auto &block : blocks) {
      for (auto *succ : block->successors()) {
        succ->add_predecessor(block.get());
      }
    }
  }

  /*
     * The iterator of this class iterates over the blocks. The blocks are
     * iterated in the order during the construction
     */
  using iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::iterator;
  using const_iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::const_iterator;
  using reverse_iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::reverse_iterator;
  using const_reverse_iterator = typename std::vector<std::unique_ptr<BlockInterface<GraphType>>>::const_reverse_iterator;

  iterator begin()
  {
    return blocks.begin();
  }

  iterator end()
  {
    return blocks.end();
  }

  const_iterator begin() const
  {
    return blocks.begin();
  }

  const_iterator end() const
  {
    return blocks.end();
  }

  reverse_iterator rbegin()
  {
    return blocks.rbegin();
  }

  reverse_iterator rend()
  {
    return blocks.rend();
  }

  const_reverse_iterator rbegin() const
  {
    return blocks.rbegin();
  }

  const_reverse_iterator rend() const
  {
    return blocks.rend();
  }
};

template<typename GraphType>
struct InterfaceWrapper
{
  using Cfg = CfgInterface<GraphType>;
  using Block = BlockInterface<GraphType>;
  using Statement = StatementInterface<GraphType>;
};
}// namespace distillery

#endif//DISTILLERY_CFG_INTERFACE_HH
