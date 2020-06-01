#include "dummy.hh"
#include "doctest.h"

// Tests that don't naturally fit in the headers/.cpp files directly
// can be placed in a tests/*.cpp file. Integration tests are a good example.

DummyStatement::DummyStatement(unsigned id, std::string statement) : id(id),
                                                                     statement(statement)
{}

DummyBlock::DummyBlock(unsigned id) : id(id)
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
  for (int idx = 0; idx < 5; idx++) {
    blocks.emplace_back(idx);
    if (idx > 0)
      blocks[idx - 1].push_successor(&blocks[idx]);
  }
  blocks[1].push_successor(&blocks[1]);
  blocks[2].push_successor(&blocks[4]);
  root = &blocks[0];
}


TEST_CASE("dummy graph works")
{
  DummyGraph dg;
  CHECK(dg.doSomething() == true);
  CHECK(dg.blocks.size() == 5);
}
