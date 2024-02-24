#include "../../q_arch.h"


//#ifdef VECTORMATH_DEBUG
//#include <cstdio>
//#endif // VECTORMATH_DEBUG

#if defined( _MSC_VER )
// Visual Studio (MS compiler)
#define VECTORMATH_ALIGNED( type ) __declspec( align( 16 ) ) type
#define VECTORMATH_ALIGNED_TYPE_PRE __declspec( align( 16 ) )
#define VECTORMATH_ALIGNED_TYPE_POST /* nothing */
#elif defined( __GNUC__ )
// GCC or Clang
#define VECTORMATH_ALIGNED( type ) type __attribute__( ( aligned( 16 ) ) )
#define VECTORMATH_ALIGNED_TYPE_PRE /* nothing */
#define VECTORMATH_ALIGNED_TYPE_POST __attribute__( ( aligned( 16 ) ) )
#else
// Unknown compiler
#error "Define VECTORMATH_ALIGNED for your compiler or platform!"
#endif

VECTORMATH_ALIGNED_TYPE_PRE struct vec4i_s {
	int x,y,z,w;
} VECTORMATH_ALIGNED_TYPE_POST;


//VECTORMATH_ALIGNED_TYPE_PRE struct vec2_s {
//	float x, y;
//} VECTORMATH_ALIGNED_TYPE_POST;
//
//inline const float vec2_element( const struct vec2_s *v1, uint_fast8_t index);
//
//inline const struct vec2_s vec2_create( float x, float y );
//inline const struct vec2_s vec2_create_single( float x);
//inline const struct vec2_s vec2_create_vec2( const struct vec2_s* v);
//
//inline const struct vec2_s __vec2_x( float x );
//inline const struct vec2_s __vec2_y( float x);
//inline float vec2_scalar_select(const struct vec2_s *v, unsigned int select );
//
//inline const struct vec2_s vec2_slerp( float t, const struct vec2_s *unitVec0, const struct vec2_s *unitVec1 );
//inline const struct vec2_s vec2_add( const struct vec2_s *v1, const struct vec2_s *v2 );
//inline const struct vec2_s vec2_sub( const struct vec2_s *v1, const struct vec2_s *v2 );
//inline const struct vec2_s vec2_mul( const struct vec2_s *v1, const struct vec2_s *v2 );
//
//inline const struct vec3_s vec2_add_scalar( const struct vec2_s *v1, float value);
//inline const struct vec3_s vec2_sub_scalar( const struct vec2_s *v1, float value );
//inline const struct vec3_s vec2_mul_scalar( const struct vec2_s *v1, float value );
//
//inline const struct vec2_s vec2_max_per_element( const struct vec2_s *v1, const struct vec2_s *v2);
//inline const struct vec2_s vec2_min_per_element( const struct vec2_s *v1, const struct vec2_s *v2);
//inline const struct vec2_s vec2_copy_sign_element( const struct vec2_s *v1, const struct vec2_s *v2);
//
//inline const struct vec2_s vec2_max_element( const struct vec2_s *v1);
//inline const struct vec2_s vec2_min_element( const struct vec2_s *v1);
//
//inline const float vec2_normalize( const struct vec2_s *v1);
//inline const float vec2_length( const struct vec2_s *v1);
//inline const float vec2_length_sqr( const struct vec2_s *v1);
//
//inline float sum(const struct vec2_s* vec);
//
//inline const float vec2_outer( const struct vec2_s *v1, const struct vec2_s *v2);

VECTORMATH_ALIGNED_TYPE_PRE struct vec3_s {
	float x, y, z;
} VECTORMATH_ALIGNED_TYPE_POST;


inline const struct vec3_s vec3_create( float x, float y, float z );
inline const struct vec3_s vec3_create_single_scalar( float x);
inline const struct vec3_s vec3_create_vec3( const struct vec3_s* v);

inline const struct vec3_s vec3_slerp( float t, const struct vec3_s *unitVec0, const struct vec3_s *unitVec1 );
inline const struct vec3_s vec3_add( const struct vec3_s *v1, const struct vec3_s *v2 );
inline const struct vec3_s vec3_sub( const struct vec3_s *v1, const struct vec3_s *v2 );
inline const struct vec3_s vec3_mul( const struct vec3_s *v1, const struct vec3_s *v2 );

inline const struct vec3_s vec3_add_scalar( const struct vec3_s *v1, float value);
inline const struct vec3_s vec3_sub_scalar( const struct vec3_s *v1, float value );
inline const struct vec3_s vec3_mul_scalar( const struct vec3_s *v1, float value );

inline const struct vec3_s vec3_max_per_element( const struct vec3_s *v1, const struct vec3_s *v2);
inline const struct vec3_s vec3_min_per_element( const struct vec3_s *v1, const struct vec3_s *v2);
inline const struct vec3_s vec3_copy_sign_element( const struct vec3_s *v1, const struct vec3_s *v2);

inline const struct vec3_s vec3_max_element( const struct vec3_s *v1);
inline const struct vec3_s vec3_min_element( const struct vec3_s *v1);

inline const float vec3_element( const struct vec3_s *v1, uint_fast8_t index);
inline float __vec3_x(const struct vec3_s* v);
inline float __vec3_y(const struct vec3_s* v);
inline float __vec3_z(const struct vec3_s* v);

inline const float vec3_normalize( const struct vec3_s *v1);
inline const float vec3_length( const struct vec3_s *v1);
inline const float vec3_length_sqr( const struct vec3_s *v1);

inline float vec3_sum(const struct vec3_s* vec);

inline const float vec3_cross( const struct vec3_s *v1, const struct vec3_s *v2);
inline const float vec3_outer( const struct vec3_s *v1, const struct vec3_s *v2);

VECTORMATH_ALIGNED_TYPE_PRE struct vec4_s {
	float x, y, z, w;
} VECTORMATH_ALIGNED_TYPE_POST;

inline const float vec4_element( const struct vec4_s *v1, uint_fast8_t index);

inline const struct vec3_s vec4_as_vec3( const struct vec4_s* v );
inline const struct vec2_s vec4_as_vec2( const struct vec4_s* v );

inline const struct vec4_s vec4_create( float x, float y, float z );

inline const struct vec4_s vec4_slerp( float t, const struct vec4_s *unitVec0, const struct vec4_s *unitVec1 );
inline const struct vec4_s vec4_add( const struct vec4_s *v1, const struct vec4_s *v2 );
inline const struct vec4_s vec4_sub( const struct vec4_s *v1, const struct vec4_s *v2 );
inline const struct vec4_s vec4_mul( const struct vec4_s *v1, const struct vec4_s *v2 );

inline const struct vec4_s vec4_add_scalar( const struct vec4_s *v1, float value);
inline const struct vec4_s vec4_sub_scalar( const struct vec4_s *v1, float value );
inline const struct vec4_s vec4_mul_scalar( const struct vec4_s *v1, float value );

inline const struct vec4_s vec4_max_per_element( const struct vec4_s *v1, const struct vec4_s *v2);
inline const struct vec4_s vec4_min_per_element( const struct vec4_s *v1, const struct vec4_s *v2);
inline const struct vec4_s vec4_copy_sign_element( const struct vec4_s *v1, const struct vec4_s *v2);

inline const struct vec4_s vec4_max_element( const struct vec4_s *v1);
inline const struct vec4_s vec4_min_element( const struct vec4_s *v1);

inline const float vec4_normalize( const struct vec4_s *v1);
inline const float vec4_length( const struct vec4_s *v1);
inline const float vec4_length_sqr( const struct vec4_s *v1);

inline float vec4_sum(const struct vec4_s* vec);

inline const float vec4_cross( const struct vec4_s *v1, const struct vec4_s *v2);
inline const float vec4_outer( const struct vec4_s *v1, const struct vec4_s *v2);

VECTORMATH_ALIGNED_TYPE_PRE struct mat4_s {
	struct vec4_s mCol0;
	struct vec4_s mCol1;
	struct vec4_s mCol2;
	struct vec4_s mCol3;
} VECTORMATH_ALIGNED_TYPE_POST;

inline const float mat4_element( const struct mat4_s *v1, uint_fast8_t col, uint_fast8_t row );

inline const struct vec4_s mat4_create( const struct vec4_s *col0, const struct vec4_s *col1, const struct vec4_s *col2, const struct vec4_s *col3 );

VECTORMATH_ALIGNED_TYPE_PRE struct mat3_s {
	struct vec3_s mCol0;
	struct vec3_s mCol1;
	struct vec3_s mCol2;
	struct vec3_s mCol3;
} VECTORMATH_ALIGNED_TYPE_POST;

inline const float mat3_element( const struct mat3_s *v1, uint_fast8_t col, uint_fast8_t row );

inline const struct vec4_s mat3_create( const struct vec3_s *col0, const struct vec3_s *col1, const struct vec3_s *col2, const struct vec3_s *col3 );

//#include "vector.inl"

