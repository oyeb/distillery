#ifndef RTL_TRANSLATION_VALIDATION_PRE_COMPILED_HEADER_HH
#define RTL_TRANSLATION_VALIDATION_PRE_COMPILED_HEADER_HH

/*
 * std headers
 */
#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <memory>
#include <string>

/*
 * z3 headers.
 * Must include z3++.h before gcc-plugin.h.
 * Otherwise, some name-collision happens.
 */
#include <z3++.h>

/*
 * gcc headers.
 * Must include gcc-plugin.h as the first gcc header.
 */
#include <gcc-plugin.h>
#include <function.h>
#include <tree-pass.h>
#include <tree.h>
#include <tree-pretty-print.h>
#include <rtl.h>
// #include <print-rtl.h>

/*
 * utility headers.
 */
#include <utils/logger.hh>
//
#endif //RTL_TRANSLATION_VALIDATION_PRE_COMPILED_HEADER_HH
