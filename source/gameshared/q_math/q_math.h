#ifndef Q_MATH_H_ 
#define Q_MATH_H_

#include "../q_arch.h"

#define q_abs(x) (( (x) > 0 ) ? (x) : -(x))
#define q_max( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define q_min( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )

#ifdef __cplusplus
	#define Q_MATH_METHOD(METHOD_C, METHOD_CPP) METHOD_CPP 
#else
	#define Q_MATH_METHOD(METHOD_C, METHOD_CPP) METHOD_C 
#endif

#if defined( _MSC_VER )
// Visual Studio (MS compiler)
#define Q_MATH_ALIGNED( type ) __declspec( align( 16 ) ) type
#define Q_MATH_ALIGNED_TYPE_PRE __declspec( align( 16 ) )
#define Q_MATH_ALIGNED_TYPE_POST /* nothing */
#elif defined( __GNUC__ )
// GCC or Clang
#define Q_MATH_ALIGNED( type ) type __attribute__( ( aligned( 16 ) ) )
#define Q_MATH_ALIGNED_TYPE_PRE /* nothing */
#define Q_MATH_ALIGNED_TYPE_POST __attribute__( ( aligned( 16 ) ) )
#else
// Unknown compiler
#error "Define Q_MATH_ALIGNED for your compiler or platform!"
#endif

//#elif (VECTORMATH_CPU_HAS_SSE1_OR_BETTER && !VECTORMATH_FORCE_SCALAR_MODE) // SSE
//    #include "sse/vectormath.hpp"
//    using namespace Vectormath::SSE;
//#elif (VECTORMATH_CPU_HAS_NEON && !VECTORMATH_FORCE_SCALAR_MODE) // NEON
//	#include "neon/vectormath.hpp"
//	using namespace Vectormath::Neon;
//#else // !SSE
    #include "scalar/vectormath.h"
//#endif // Vectormath mode selection

#ifdef __cplusplus
#else
#endif


#endif 

