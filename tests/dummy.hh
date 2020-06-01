#ifndef DISTILLERY_TEST_DUMMYGRAPH_H
#define DISTILLERY_TEST_DUMMYGRAPH_H
#include <string>
#include <vector>

class DummyStatement
{
public:
  unsigned id;
  std::string statement;
  /**
   * Constructor for DummyStatement (builds just 1 statement).
   */
  DummyStatement(unsigned id, std::string statement);
};

class DummyBlock
{
public:
  unsigned id;
  //! Contains just one statement!
  std::vector<DummyStatement> statements;
  std::vector<const DummyBlock *> successors;

  /**
   * Constructor for DummyBlock (builds just 1 statement).
   */
  DummyBlock(unsigned id);
  void push_successor(const DummyBlock *successor);
};

class DummyGraph
{
public:
  //! Name of this CFG
  std::string name;
  //! Contains basic blocks
  std::vector<DummyBlock> blocks;
  DummyBlock *root;

  /**
   * Constructor for DummyGraph (builds some dummy blocks).
   */
  DummyGraph();

  /**
   * Returns a bool.
   * @return Always True.
   */
  bool doSomething() { return true; }
};

#endif// DISTILLERY_TEST_DUMMYGRAPH_H
