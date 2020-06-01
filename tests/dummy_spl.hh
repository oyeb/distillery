#ifndef DISTILLERY_TEST_DUMMYGRAPH_SPECIALIZATION_H
#define DISTILLERY_TEST_DUMMYGRAPH_SPECIALIZATION_H
#include "dummy.h"
#include <graph_abstractions/cfg_interface.hh>

struct DummyGraphSpecialization
{
  using Cfg = DummyGraph;
  using Block = DummyBlock;
  using Statement = DummyStatement;
};

#endif// DISTILLERY_TEST_DUMMYGRAPH_SPECIALIZATION_H
