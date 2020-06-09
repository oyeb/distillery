
#ifndef RTL_TRANSLATION_VALIDATION_RTL_SPECIALIZATION_HH
#define RTL_TRANSLATION_VALIDATION_RTL_SPECIALIZATION_HH

#include <pre-compiled-header.hh>

#include "gcc-plugin.h"
#include "function.h"
#include "rtl.h"

namespace distillery {
	struct RtlSpecialization {
		using Cfg = ::function;
		using Block = basic_block;
		using Statement = rtx;
	};
}

#endif //RTL_TRANSLATION_VALIDATION_RTL_SPECIALIZATION_HH
