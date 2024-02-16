
// ================================================================================================
// -*- C++ -*-
// File: vectormath/vectormath.hpp
// Author: Guilherme R. Lampert
// Created on: 30/12/16
// Brief: This header exposes the Sony Vectormath library types and functions into the global scope.
// ================================================================================================

#ifndef VECTORMATH_HPP
#define VECTORMATH_HPP

#include "../q_arch.h"
#include "vectormath_settings.h"

//#elif (VECTORMATH_CPU_HAS_SSE1_OR_BETTER && !VECTORMATH_FORCE_SCALAR_MODE) // SSE
//    #include "sse/vectormath.hpp"
//    using namespace Vectormath::SSE;
//#elif (VECTORMATH_CPU_HAS_NEON && !VECTORMATH_FORCE_SCALAR_MODE) // NEON
//	#include "neon/vectormath.hpp"
//	using namespace Vectormath::Neon;
//#else // !SSE
    #include "scalar/vectormath.h"
//#endif // Vectormath mode selection

////========================================= #TheForgeMathExtensionsBegin ================================================
////========================================= #TheForgeAnimationMathExtensionsBegin =======================================
//#include "soa/soa.hpp"
//using namespace Vectormath::Soa;
////========================================= #TheForgeAnimationMathExtensionsEnd =======================================
////========================================= #TheForgeMathExtensionsEnd ================================================
//
//#include "vec2d.hpp"  // - Extended 2D vector and point classes; not aligned and always in scalar floats mode.
//#include "common.hpp" // - Miscellaneous helper functions.

#include "vectormath_common.h"

#endif // VECTORMATH_HPP
