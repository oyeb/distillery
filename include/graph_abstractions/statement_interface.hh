//
// Created by ajeesh on 3/9/20.
//

#ifndef DISTILLERY_STATEMENT_INTERFACE_HH
#define DISTILLERY_STATEMENT_INTERFACE_HH

#include <pre-compiled-header.hh>

#include <cstdio>

namespace distillery {

template<typename GraphType>
class StatementInterface
{

public:
	using Cfg = typename GraphType::Cfg;
	using Block = typename GraphType::Block;
	using Statement = typename GraphType::Statement;

protected:
	const Statement *stmt;

public:
	explicit StatementInterface(const Statement *stmt1) : stmt(stmt1) {}

	const Statement &operator*() const
	{
		return *stmt;
	}

	const Statement *get() const
	{
		return stmt;
	}

	void dump(FILE *out = stdout) const;
};

template<typename GraphType>
std::ostream &operator<<(std::ostream &out, const distillery::StatementInterface<GraphType> &stmt);
}// namespace distillery

#endif//DISTILLERY_STATEMENT_INTERFACE_HH
