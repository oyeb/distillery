#include "dummy_spl.hh"
#include "doctest.h"
#include <iostream>
#include <unordered_map>

using Statement = DummyStatement;
using Block = DummyBlock;

template<>
distillery::StatementInterface<DummyGraphSpecialization>::StatementInterface(const Statement *stmt1)
  : stmt(stmt1) {}

template<>
distillery::BlockInterface<DummyGraphSpecialization>::BlockInterface(const Block *block)
{
  statements.emplace_back(new StatementInterface<DummyGraphSpecialization>(&block->statements.front()));
}

template<>
distillery::CfgInterface<DummyGraphSpecialization>::CfgInterface(DummyGraph &graph)
{
  this->fun_name = "dummy";
  std::unordered_map<unsigned, BlockInterface<DummyGraphSpecialization> *> corresponding_interface;

  for (const auto &block : graph.blocks) {
    blocks.emplace_back(new BlockInterface<DummyGraphSpecialization>(&block));
    blocks.back()->set_block_id(block.id);
    corresponding_interface[block.id] = blocks.back().get();
  }

  for (const auto &block : graph.blocks) {
    for (const auto &successor : block.successors) {
      corresponding_interface[block.id]->add_successor(corresponding_interface[successor->id]);
      corresponding_interface[successor->id]->add_predecessor(corresponding_interface[block.id]);
    }
  }

  this->m_entry = corresponding_interface[graph.root->id];
  this->m_exit = corresponding_interface[graph.blocks.back().id];
}

TEST_CASE("dummy graph traversable using interface")
{
  DummyGraph dg;
  distillery::CfgInterface<DummyGraphSpecialization> dummy(dg);

  for (const auto &block : dummy) {
    switch (block->block_id()) {
    case 0:
      CHECK(block->to_string() == "{} -> [0] -> {1,}");
      break;
    case 1:
      CHECK(block->to_string() == "{0,1,} -> [1] -> {2,1,}");
      break;
    case 2:
      CHECK(block->to_string() == "{1,} -> [2] -> {3,4,}");
      break;
    case 3:
      CHECK(block->to_string() == "{2,} -> [3] -> {4,}");
      break;
    case 4:
      CHECK(block->to_string() == "{2,3,} -> [4] -> {}");
      break;
    default:
      break;
    }
  }
}
