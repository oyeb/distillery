#include "dummy.hh"
#include "doctest.h"

#include <iostream>

// Tests that don't naturally fit in the headers/.cpp files directly
// can be placed in a tests/*.cpp file. Integration tests are a good example.

DummyStatement::DummyStatement(unsigned id1, std::string statement1) : id(id1),
                                                                       statement(statement1)
{}

DummyBlock::DummyBlock(unsigned id1) : id(id1)
{
  statements.emplace_back(id, std::to_string(id));
}

void DummyBlock::push_successor(const DummyBlock *successor)
{
  successors.push_back(successor);
}

DummyGraph::DummyGraph()
{
  /**
   * 0 -> 1 <-> 1
   *      1 --> 2 --> 3
   *            2 --------> 4
   *                  3 --> 4
   */

  for (size_t idx = 0; idx < 5; idx++)
    blocks.emplace_back(idx);

  for (auto &b : blocks) {
    if (b.id < 4)
      b.push_successor(&blocks[b.id + 1]);
  }
  blocks[1].push_successor(&blocks[1]);
  blocks[2].push_successor(&blocks[4]);
  root = &blocks[0];
}

TEST_CASE("dummy graph works")
{
  DummyGraph dg;
  for (const auto &b : dg.blocks) {
    bool has_someone_larger = false, has_someone = false;
    for (const auto &successor : b.successors) {
      has_someone = true;
      has_someone_larger |= successor->id > b.id;
    }
    CHECK((!has_someone || has_someone_larger));
  }
  CHECK(dg.doSomething() == true);
  CHECK(dg.blocks.size() == 5);
}
