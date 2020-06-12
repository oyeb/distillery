==============
The Distillery
==============
.. rst-class:: without-title
.. rst-class:: float-center
.. caution:: **since 2020:** *Helping you find that sweet essence in your programs.*

The Distillery |release| provides you with a set of algorithms and program
analyses which work on most CFGs or IRs in a compiler agnostic manner.

To distill your program (available as custom AST/IR/compiler) into a data-flow
Value, you must specialise a few (templated) interfaces of the Distillery.
             
Contents
========

Interfaces for Control Flow
---------------------------
1. Top level Control Flow Graph
2. Basic Blocks
3. Statements
    
Interfaces for AST
------------------
1. Expressions
    
Analyses
--------
1. Liveness
2. UseDef

Examples
--------
:doc:`Dummy examples <examples>` that showcase the use of various interfaces.
