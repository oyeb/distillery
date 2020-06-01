//
// Created by ajeesh on 3/9/20.
//

#ifndef DISTILLERY_BLOCK_INTERFACE_HH
#define DISTILLERY_BLOCK_INTERFACE_HH

#include <iterator>
#include <memory>
#include <sstream>
#include <vector>

#include <graph_abstractions/statement_interface.hh>

namespace distillery {

template<typename GraphType>
class BlockInterface
{
public:
  using Cfg = typename GraphType::Cfg;
  using Block = typename GraphType::Block;
  using Statement = typename GraphType::Statement;

protected:
  unsigned block_no{ 0 };
  std::vector<std::unique_ptr<StatementInterface<GraphType>>> statements;
  std::vector<BlockInterface<GraphType> *> v_predecessors;
  std::vector<BlockInterface<GraphType> *> v_successors;

public:
  BlockInterface() = default;

  explicit BlockInterface(const Block *bb1);

  unsigned block_id() const
  {
    return block_no;
  }

  void set_block_id(unsigned n)
  {
    block_no = n;
  }

  void add_statement(std::unique_ptr<StatementInterface<GraphType>> stmt)
  {
    statements.push_back(stmt);
  }

  std::vector<BlockInterface<GraphType> *> &predecessors()
  {
    return v_predecessors;
  }

  const std::vector<BlockInterface<GraphType> *> &predecessors() const
  {
    return v_predecessors;
  }

  std::vector<BlockInterface<GraphType> *> &successors()
  {
    return v_successors;
  }

  const std::vector<BlockInterface<GraphType> *> &successors() const
  {
    return v_successors;
  }

  void add_predecessor(BlockInterface<GraphType> *pred)
  {
    v_predecessors.push_back(pred);
  }

  void add_successor(BlockInterface<GraphType> *succ)
  {
    v_successors.push_back(succ);
  }

  void dump(FILE *out = stdout)
  {
    fprintf(out, "Block No : %d\n", block_no);
    for (auto &stmt : statements) {
      stmt->dump(out);
    }
    fprintf(out, "\nSuccessors : ");
    for (auto *succ : successors()) {
      fprintf(out, "%d ", succ->block_id());
    }
    fprintf(out, "\nPredecessors : ");
    for (auto *pred : predecessors()) {
      fprintf(out, "%d ", pred->block_id());
    }
    fprintf(out, "\n");
  }

  std::string to_string()
  {
    std::stringstream ss;
    ss << "{";
    for (const auto &pred : predecessors())
      ss << pred->block_no << ",";
    ss << "} -> [" << block_no << "] -> {";
    for (const auto &succ : successors())
      ss << succ->block_no << ",";
    ss << "}";
    return ss.str();
  }

  void merge(BlockInterface<GraphType> &block)
  {
    for (auto &stmt : block) {
      statements.push_back(std::move(stmt));
    }

    v_successors.clear();
    for (auto *succ : block.successors()) {
      v_successors.push_back(succ);
    }
  }

  /*
     * Ideally define a custom iterator.
     * The current iterator iterates through unique_ptr of objects.
     * It should instead iterate the actual objects
     */

  using iterator = typename std::vector<std::unique_ptr<StatementInterface<GraphType>>>::iterator;
  using const_iterator = typename std::vector<std::unique_ptr<StatementInterface<GraphType>>>::const_iterator;
  using reverse_iterator = typename std::vector<std::unique_ptr<StatementInterface<GraphType>>>::reverse_iterator;
  using const_reverse_iterator = typename std::vector<std::unique_ptr<StatementInterface<GraphType>>>::const_reverse_iterator;

  iterator begin()
  {
    return statements.begin();
  }

  iterator end()
  {
    return statements.end();
  }

  const_iterator begin() const
  {
    return statements.begin();
  }

  const_iterator end() const
  {
    return statements.end();
  }

  reverse_iterator rbegin()
  {
    return statements.rbegin();
  }

  reverse_iterator rend()
  {
    return statements.rend();
  }

  const_reverse_iterator rbegin() const
  {
    return statements.rbegin();
  }

  const_reverse_iterator rend() const
  {
    return statements.rend();
  }
};
}// namespace distillery

#endif//DISTILLERY_BLOCK_INTERFACE_HH
