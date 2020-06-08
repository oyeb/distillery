
#ifndef RTL_TRANSLATION_VALIDATION_MERGE_CFG_HH
#define RTL_TRANSLATION_VALIDATION_MERGE_CFG_HH

#include <graph_abstractions/cfg_interface.hh>


namespace distillery {

	template<
			typename First,
			typename Second
	>
	struct MergedGraphType {
		using Cfg = std::pair<typename First::Cfg *, typename Second::Cfg *>;
		using Block = std::pair<typename First::Block *, typename Second::Block *>;

		struct Statement {
			bool m_is_first;
			const typename First::Statement *first;
			const typename Second::Statement *second;

			Statement() = delete;

			explicit Statement(const typename First::Statement *src)
					: m_is_first(true),
					  first(src),
					  second(nullptr) {
			}

			explicit Statement(const typename Second::Statement *dest, bool flag)
					: m_is_first(false),
					  first(nullptr),
					  second(dest) {
			}

			Statement(const Statement &other)
				: m_is_first(other.m_is_first),
				  first(other.first),
				  second(other.second){
			}

			bool is_first() const {
				return m_is_first;
			}

			bool is_second() const {
				return not m_is_first;
			}

			void dump(FILE* out = stdout) const {
				if(is_first()) {
					first->dump(out);
				} else {
					second->dump(out);
				}
			}
		};
	};
}
#endif //RTL_TRANSLATION_VALIDATION_MERGE_CFG_HH
