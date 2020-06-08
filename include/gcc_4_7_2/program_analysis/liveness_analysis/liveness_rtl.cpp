
#include <gcc_4_7_2/program_analysis/liveness_analysis/liveness.hh>
#include <gcc_4_7_2/specialization/rtl_specialization.hh>

#define NOT_SUPPORTED_GIMPLE(stmt, type) throw distillery::Exception(std::string("Not supporting the type : ") + \
							std::to_string(gimple_code(stmt)) + #type)


namespace {
	std::unordered_set<std::string>
	set_union(const std::unordered_set<std::string> &set1, const std::unordered_set<std::string> &set2) {
		std::unordered_set<std::string> result = set1;
		for(const auto &str : set2) {
			result.insert(str);
		}
		return result;
	}

	void process_def(rtx exp, std::unique_ptr<distillery::StatementSet> &result, bool is_first) {
		std::string variable;
		std::string name_cfg;
		if(is_first){
			name_cfg = "src";
		}
		else{
			name_cfg = "dest";
		}

		if(GET_CODE(exp) == REG) {
				variable = "r_" + name_cfg + "_" + std::to_string(REGNO(exp));
				result->erase(variable);
				return;
		}
		else if(GET_CODE(exp) == MEM)
		{
			if(tree gimple_node = MEM_EXPR(exp))
			{
				variable = get_name(gimple_node);
				result->erase(variable);
				return;
			}
			else
			{
				assert(false);
			}
		}
		else if(GET_CODE(exp) == PC)
		{
			variable = "pc_" + name_cfg;
			result->erase(variable);
			return;
		}
		else
		{
			assert(false);
		}
	}

	void process_use(rtx insn, std::unique_ptr<distillery::StatementSet>& result, bool is_first) {
//        print_rtl_single(stdout, insn);
		if(!insn) return;
		std::string name_cfg;
		if(is_first){
			name_cfg = "src";
		}
		else{
			name_cfg = "dest";
		}
		switch (GET_CODE(insn)) {
			case UNKNOWN:
				// Ignore this line. Add no constraints
				assert(false);
			case VALUE:
				// Do not know this node.
				break;
			case DEBUG_EXPR:
				// Do not know this node.
				break;
			case EXPR_LIST:
				// Do not know what to do
				break;
			case INSN_LIST: {

				auto head = XEXP(insn, 0);
				auto tail = XEXP(insn, 1);

				process_use(head, result, is_first);
				process_use(tail, result, is_first);

				return;
			}
			case SEQUENCE:
				// No need to handle. Will not come in expand.
				break;
			case ADDRESS:
				// Do not know
				break;
			case DEBUG_INSN:
				// Do not know
				break;
			case INSN: {
				// TODO: Might need to change this.
				if (auto sub_exp = XEXP(insn, 4)) {
					process_use(sub_exp, result, is_first);
					return;
				} else {
					print_rtl_single(stderr, insn);
					throw distillery::Exception(std::string(name_cfg + ": Empty expression in rtl INSN"));
				}
			}
			case JUMP_INSN: {
				if (auto sub_exp = XEXP(insn, 4)) {
					process_use(sub_exp, result, is_first);
					return;
				}
				else {
					throw distillery::Exception(std::string(name_cfg + ": No sub_exp in jmp"));
				}
			}
			case CALL_INSN:
			{
				//TODO : ...
			}
				break;
			case BARRIER:
				// Nothing to do here
				break;
			case CODE_LABEL: {
				return;
			}
			case NOTE: {
				return;
			}
			case COND_EXEC:
				// Done after if_convert pass. Not our concern.
				break;
			case PARALLEL: {
				if(XVEC(insn, 0)) {
					int len = XVECLEN(insn, 0);
					for(int i = 0; i < len; ++i) {
						process_use(XVECEXP(insn, 0, i), result, is_first);
					}
				}
				return;
			}
			case ASM_INPUT:
				// Do not know
				break;
			case ASM_OPERANDS:
				// Do not know
				break;
			case UNSPEC:
				// TODO : Need to be implemented.
				break;
			case UNSPEC_VOLATILE:
				// TODO : Need to be implemented.
				break;
			case ADDR_VEC:
				// Currently do not care.
				break;
			case ADDR_DIFF_VEC:
				// Currently do not care.
				break;
			case PREFETCH:
				// TODO : Need to be implemented.
				break;
			case SET: {
				process_def(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;
			}
			case USE:
				std::cout << "Use" << std::endl;
				process_use(XEXP(insn, 0), result, is_first);
				std::cout << "Use Done" << std::endl;
				return;
			case CLOBBER: {
				process_def(XEXP(insn, 0), result, is_first);
				return;
			}
			case CALL:
				//TODO: ...
				break;
			case RETURN:
				break;
			case SIMPLE_RETURN:
				// Not sure if it comes in expand.
				break;
			case EH_RETURN:
				// Not sure if it comes in expand.
				break;
			case TRAP_IF:
				// TODO: ...
				break;
			case CONST_INT:
				return;
			case CONST_FIXED:
				break;
			case CONST_DOUBLE:
				break;
			case CONST_VECTOR:
				break;
			case CONST_STRING:
				break;
			case CONST:
				break;
			case PC:
			{
				std::string variable = "pc_" + name_cfg;
				result->insert(variable);
			}
			case REG:
			{
				std::string variable;

				variable = "r_" + name_cfg + "_" + std::to_string(REGNO(insn));
				if((!is_first) && REG_USERVAR_P(insn) && REG_EXPR(insn) && CONST_CAST_TREE(REG_EXPR(insn))!=NULL_TREE && DECL_NAME(CONST_CAST_TREE(REG_EXPR(insn)))){
					std::string var2 = "r_src_" + std::to_string(ORIGINAL_REGNO(insn));
					result->insert(variable, var2);
				} 
				return;
			}
			case SCRATCH:
				break;
			case SUBREG:
				break;
			case STRICT_LOW_PART:
				break;
			case CONCAT:
				break;
			case CONCATN:
				break;
			case MEM:
			{
				if(auto gimple_node = MEM_EXPR(insn))
				{
					std::string variable = get_name(gimple_node);
					result->insert(variable);
					return;
				}
				assert(false);
			}
				break;
			case LABEL_REF:
			{
				return;
			}
			case SYMBOL_REF:
				break;
			case CC0:
				break;
			case IF_THEN_ELSE:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				process_use(XEXP(insn, 2), result, is_first);

				return;
			}
			case COMPARE:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case PLUS:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case MINUS:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case NEG:
				break;
			case MULT:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case SS_MULT:
				break;
			case US_MULT:
				break;
			case DIV:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case SS_DIV:
				break;
			case US_DIV:
				break;
			case MOD:
				break;
			case UDIV:
				break;
			case UMOD:
				break;
			case AND:
				break;
			case IOR:
				break;
			case XOR:
				break;
			case NOT:
				break;
			case ASHIFT:
				break;
			case ROTATE:
				break;
			case ASHIFTRT:
				break;
			case LSHIFTRT:
				break;
			case ROTATERT:
				break;
			case SMIN:
				break;
			case SMAX:
				break;
			case UMIN:
				break;
			case UMAX:
				break;
			case PRE_DEC:
				break;
			case PRE_INC:
				break;
			case POST_DEC:
				break;
			case POST_INC:
				break;
			case PRE_MODIFY:
				break;
			case POST_MODIFY:
				break;
			case NE:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case EQ:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case GE:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case GT:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case LE:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case LT:
			{
				process_use(XEXP(insn, 0), result, is_first);
				process_use(XEXP(insn, 1), result, is_first);
				return;

			}
			case GEU:
				break;
			case GTU:
				break;
			case LEU:
				break;
			case LTU:
				break;
			case UNORDERED:
				break;
			case ORDERED:
				break;
			case UNEQ:
				break;
			case UNGE:
				break;
			case UNGT:
				break;
			case UNLE:
				break;
			case UNLT:
				break;
			case LTGT:
				break;
			case SIGN_EXTEND:
				break;
			case ZERO_EXTEND:
				break;
			case TRUNCATE:
				break;
			case FLOAT_EXTEND:
				break;
			case FLOAT_TRUNCATE:
				break;
			case FLOAT:
				break;
			case FIX:
				break;
			case UNSIGNED_FLOAT:
				break;
			case UNSIGNED_FIX:
				break;
			case FRACT_CONVERT:
				break;
			case UNSIGNED_FRACT_CONVERT:
				break;
			case SAT_FRACT:
				break;
			case UNSIGNED_SAT_FRACT:
				break;
			case ABS:
				break;
			case SQRT:
				break;
			case BSWAP:
				break;
			case FFS:
				break;
			case CLRSB:
				break;
			case CLZ:
				break;
			case CTZ:
				break;
			case POPCOUNT:
				break;
			case PARITY:
				break;
			case SIGN_EXTRACT:
				break;
			case ZERO_EXTRACT:
				break;
			case HIGH:
				break;
			case LO_SUM:
				break;
			case VEC_MERGE:
				break;
			case VEC_SELECT:
				break;
			case VEC_CONCAT:
				break;
			case VEC_DUPLICATE:
				break;
			case SS_PLUS:
				break;
			case US_PLUS:
				break;
			case SS_MINUS:
				break;
			case SS_NEG:
				break;
			case US_NEG:
				break;
			case SS_ABS:
				break;
			case SS_ASHIFT:
				break;
			case US_ASHIFT:
				break;
			case US_MINUS:
				break;
			case SS_TRUNCATE:
				break;
			case US_TRUNCATE:
				break;
			case FMA:
				break;
			case VAR_LOCATION:
				break;
			case DEBUG_IMPLICIT_PTR:
				break;
			case ENTRY_VALUE:
				break;
			case DEBUG_PARAMETER_REF:
				break;
			case LAST_AND_UNUSED_RTX_CODE:
				break;
		}
		throw distillery::Exception(std::string("Node not supported : ") + GET_RTX_NAME(GET_CODE(insn)));
	}
}

namespace distillery {

	template<>
	std::unique_ptr<StatementSet>
	distillery::LivenessAnalysisUpwardExposed<InterfaceWrapper<RtlSpecialization>>::post_statement_helper(
			const distillery::StatementInterface<RtlSpecialization> &statement,
			const StatementSet &value,
			const bool is_first) {
		rtx stmt = statement.get();
		auto result = std::unique_ptr<StatementSet>(new StatementSet(value));
		process_use(stmt, result, is_first);
		return result;
	}

}