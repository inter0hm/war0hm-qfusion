
#include <math.h>
#include <stdint.h>


static const float VECTORMATH_SLERP_TOL = 0.999f;
static const double VECTORMATH_SLERP_TOL_D = 0.999;

Q_MATH_ALIGNED_TYPE_PRE struct vec4i_s {
	int x, y, z, w;
} Q_MATH_ALIGNED_TYPE_POST;

Q_MATH_ALIGNED_TYPE_PRE struct vec3_s {
	float x, y, z;
} Q_MATH_ALIGNED_TYPE_POST;

Q_MATH_ALIGNED_TYPE_PRE struct vec4_s {
	float x, y, z, w;
} Q_MATH_ALIGNED_TYPE_POST;

Q_MATH_ALIGNED_TYPE_PRE struct mat3_s {
	struct vec3_s mCol0;
	struct vec3_s mCol1;
	struct vec3_s mCol2;
	struct vec3_s mCol3;
} Q_MATH_ALIGNED_TYPE_POST;

Q_MATH_ALIGNED_TYPE_PRE struct mat4_s {
	struct vec4_s mCol0;
	struct vec4_s mCol1;
	struct vec4_s mCol2;
	struct vec4_s mCol3;
} Q_MATH_ALIGNED_TYPE_POST;

// vec3
inline struct vec3_s q_vec3_create( float x, float y, float z );

inline float q_vec3_dot( const struct vec3_s v1, const struct vec3_s v2);
inline struct vec3_s vec3_slerp( float t, const struct vec3_s unitVec0, const struct vec3_s unitVec1 );
inline struct vec3_s q_vec3_add( const struct vec3_s v1, const struct vec3_s v2 );
inline struct vec3_s q_vec3_sub( const struct vec3_s v1, const struct vec3_s v2 );
inline struct vec3_s q_vec3_mul( const struct vec3_s v1, const struct vec3_s v2 );

inline float  q_vec3_x( const struct vec3_s v1 );
inline float  q_vec3_y( const struct vec3_s v1 );
inline float  q_vec3_z( const struct vec3_s v1 );

inline struct vec3_s q_vec3_add_scalar( const struct vec3_s v1, float value );
inline struct vec3_s q_vec3_sub_scalar( const struct vec3_s v1, float value );
inline struct vec3_s q_vec3_mul_scalar( const struct vec3_s v1, float value );
inline struct vec3_s q_vec3_div_scalar( const struct vec3_s v1, float value );

inline const struct vec3_s q_vec3_max_per_element( const struct vec3_s v1, const struct vec3_s v2);
inline const struct vec3_s q_vec3_min_per_element( const struct vec3_s v1, const struct vec3_s v2);
inline const struct vec3_s q_vec3_copy_sign_element( const struct vec3_s v1, const struct vec3_s v2);

inline const struct vec3_s q_vec3_max_element( const struct vec3_s v1);
inline const struct vec3_s q_vec3_min_element( const struct vec3_s v1);

inline float q_vec3_element( const struct vec3_s v1, uint_fast8_t index);
inline struct vec3_s q_vec3_normalize( const struct vec3_s v1);
inline float q_vec3_length( const struct vec3_s v1);
inline float q_vec3_length_sqr( const struct vec3_s v1);

inline const struct vec3_s q_vec3_cross( const struct vec3_s v1, const struct vec3_s v2);
inline const float q_vec3_outer( const struct vec3_s v1, const struct vec3_s v2);

// vec4
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
inline const float vec4_outer( const struct vec4_s *v1, const struct vec4_s *v2 );

// mat3
inline const float mat4_element( const struct mat4_s *v1, uint_fast8_t col, uint_fast8_t row );
inline const struct vec4_s mat4_create( const struct vec4_s *col0, const struct vec4_s *col1, const struct vec4_s *col2, const struct vec4_s *col3 );

// mat4
inline const float mat3_element( const struct mat3_s *v1, uint_fast8_t col, uint_fast8_t row );
inline const struct vec4_s mat3_create( const struct vec3_s *col0, const struct vec3_s *col1, const struct vec3_s *col2, const struct vec3_s *col3 );

#ifdef __cplusplus
	inline struct vec3_s q_vec_add(const struct vec3_s a, const struct vec3_s b) { return q_vec3_add(a,b); }
	inline struct vec3_s q_vec_add(const struct vec3_s a, float b) { return q_vec3_add_scalar (a,b); }
#else
	#define __Q_MATH_VEC_GENERIC_2(a,b) (void(*)(typeof(a), typeof(b)))0	
	#define q_vec_mul(a,b) _Generic(__Q_MATH_VEC_GENERIC_2(a,b), \
		void(*)(const struct vec3_s, const struct vec3_s): q_vec3_mul, \
		void(*)(const struct vec3_s, float): q_vec3_mul_scalar \
	)(a,b)
	#define q_vec_add(a,b) _Generic(__Q_MATH_VEC_GENERIC_2(a,b), \
		void(*)(const struct vec3_s, const struct vec3_s): q_vec3_add, \
		void(*)(const struct vec3_s, float): q_vec3_add_scalar \
	)(a,b)
	#define q_vec_sub(a,b) _Generic(__Q_MATH_VEC_GENERIC_2(a,b), \
		void(*)(const struct vec3_s, const struct vec3_s): q_vec3_sub, \
		void(*)(const struct vec3_s, float): q_vec3_sub_scalar \
	)(a,b)
#endif

struct vec3_s q_vec3_create( float x, float y, float z ) {
	struct vec3_s res;
	res.x = x;
	res.y = y;
	res.z = z;
	return res;
}

float q_vec3_dot( const struct vec3_s v1, const struct vec3_s v2) {
	float result = ( v1.x * v2.x );
	result += ( v1.y * v2.y );
	result += ( v1.z * v2.z );
	return result;
}

struct vec3_s q_vec3_add( const struct vec3_s v1, const struct vec3_s v2 )
{
	struct vec3_s res;
	res.x = v1.x + v2.x;
	res.y = v1.y + v2.y;
	res.z = v1.z + v2.z;
	return res;
}

struct vec3_s q_vec3_sub( const struct vec3_s v1, const struct vec3_s v2 )
{
	struct vec3_s res;
	res.x = v1.x - v2.x;
	res.y = v1.y - v2.y;
	res.z = v1.z - v2.z;
	return res;
}

struct vec3_s q_vec3_mul( const struct vec3_s v1, const struct vec3_s v2 )
{
	struct vec3_s res;
	res.x = v1.x * v2.x;
	res.y = v1.y * v2.y;
	res.z = v1.z * v2.z;
	return res;
}

float q_vec3_element( const struct vec3_s v1, uint_fast8_t index) {
  switch(index) {
    case 0:
      return v1.x;
    case 1:
      return v1.y;
    case 2:
      return v1.z;
  }
  return 0.0f;
}
struct vec3_s vec3_slerp( float t, const struct vec3_s unitVec0, const struct vec3_s unitVec1 ) {
	float recipSinAngle, scale0, scale1, cosAngle, angle;
	cosAngle = q_vec3_dot(unitVec0, unitVec1);
	if (cosAngle < VECTORMATH_SLERP_TOL)
	{
		angle = acosf(cosAngle);
		recipSinAngle = (1.0f / sinf(angle));
		scale0 = (sinf(((1.0f - t) * angle)) * recipSinAngle);
		scale1 = (sinf((t * angle)) * recipSinAngle);
	}
	else
	{
		scale0 = (1.0f - t);
		scale1 = t;
	}

	return q_vec3_add(q_vec3_mul_scalar(unitVec0, scale0), q_vec3_mul_scalar(unitVec1, scale1));
}

struct vec3_s q_vec3_add_scalar( const struct vec3_s v1, float value )
{
	struct vec3_s res;
	res.x = v1.x + value;
	res.y = v1.y + value;
	res.z = v1.z + value;
	return res;
}

struct vec3_s q_vec3_sub_scalar( const struct vec3_s v1, float value )
{
	struct vec3_s res;
	res.x = v1.x - value;
	res.y = v1.y - value;
	res.z = v1.z - value;
	return res;
}

struct vec3_s q_vec3_mul_scalar( const struct vec3_s v1, float value )
{
	struct vec3_s res;
	res.x = v1.x * value;
	res.y = v1.y * value;
	res.z = v1.z * value;
	return res;
}

struct vec3_s q_vec3_div_scalar( const struct vec3_s v1, float value )
{
	struct vec3_s res;
	res.x = v1.x / value;
	res.y = v1.y / value;
	res.z = v1.z / value;
	return res;
}


inline const struct vec3_s q_vec3_max_per_element( const struct vec3_s v1, const struct vec3_s v2) {
	struct vec3_s res;
	res.x = q_max(v1.x, v2.x);
	res.y = q_max(v1.y, v2.y);
	res.z = q_max(v1.z, v2.z);
	return res;
}
inline const struct vec3_s q_vec3_min_per_element( const struct vec3_s v1, const struct vec3_s v2) {
	struct vec3_s res;
	res.x = q_min(v1.x, v2.x);
	res.y = q_min(v1.y, v2.y);
	res.z = q_min(v1.z, v2.z);
	return res;
}
inline const struct vec3_s q_vec3_copy_sign_element( const struct vec3_s v1, const struct vec3_s v2) {
	struct vec3_s res;
	res.x = ( v2.x < 0 ) ? -q_abs(v1.x) : q_abs(v1.x);
	res.y = ( v2.y < 0 ) ? -q_abs(v1.y) : q_abs(v1.y);
	res.z = ( v2.z < 0 ) ? -q_abs(v1.z) : q_abs(v1.z);
	return res;
}

float q_vec3_x( const struct vec3_s v1 )
{
	return v1.x;
}

float q_vec3_y( const struct vec3_s v1 )
{
	return v1.y;
}

float q_vec3_z( const struct vec3_s v1 )
{
	return v1.z;
}

inline struct vec3_s q_vec3_normalize( const struct vec3_s v1 )
{
	const float lenSqr = q_vec3_length_sqr(v1);
	const float lenInv = (1.0 / sqrt(lenSqr));
	return q_vec3_create((q_vec3_x(v1) * lenInv),
				   (q_vec3_y(v1) * lenInv),
				   (q_vec3_z(v1) * lenInv));
}

inline float q_vec3_length( const struct vec3_s v1 )
{
	return 0.0f;
}

inline float q_vec3_length_sqr( const struct vec3_s v1 )
{
	return 0.0f;
}


inline const struct vec3_s q_vec3_cross( const struct vec3_s v1, const struct vec3_s v2){ 
	return q_vec3_create(((q_vec3_y(v1) * q_vec3_z(v2)) - (q_vec3_z(v1) * q_vec3_y(v2))),
											 ((q_vec3_z(v1) * q_vec3_x(v2)) - (q_vec3_x(v1) * q_vec3_z(v2))),
				               ((q_vec3_x(v1) * q_vec3_y(v2)) - (q_vec3_y(v1) * q_vec3_x(v2))));

}
inline const float q_vec3_outer( const struct vec3_s v1, const struct vec3_s v2){return 0.0f;}


//inline const struct vec2_s vec2_create( float x, float y )
//{
//	struct vec2_s res = { .x = x, .y = y };
//	return res;
//}
//
//inline const struct vec2_s vec2_create_single( float x )
//{
//	struct vec2_s res = { .x = x, .y = x };
//	return res;
//}
//
//inline const struct vec2_s vec2_create_vec2( const struct vec2_s *v )
//{
//  assert(v);
//	struct vec2_s res = { .x = v->x, .y = v->y };
//	return res;
//}
//
//inline float vec2_scalar_select(const struct vec2_s *v, unsigned int select ) {
//  switch(select) {
//    case 0:
//      return v->x;
//    case 1:
//      return v->y;
//  }
//  return 0;
//}
//
//inline const struct vec2_s vec2_slerp( float t, const struct vec2_s *unitVec0, const struct vec2_s *unitVec1 ) {
//
//}
//inline const struct vec2_s vec2_add( const struct vec2_s *v1, const struct vec2_s *v2 ) {
//
//}
//inline const struct vec2_s vec2_sub( const struct vec2_s *v1, const struct vec2_s *v2 ) {
//
//}
//inline const struct vec2_s vec2_mul( const struct vec2_s *v1, const struct vec2_s *v2 ) {
//
//}


//static inline float __vec3f_x(const struct vec3_s* v1) {
//  return v1->x;
//}
//
//static inline float __vec3f_y(const struct vec3_s* v1) {
//  return v1->y;
//}
//
//static inline float __vec3f_z(const struct vec3_s* v1) {
//  return v1->z;
//}
//
//// vec3f
//static inline struct q_vec3 vec3f_create(float x, float y, float z) {
//  struct q_vec3 res;
//  res.x = x;
//  res.y = y;
//  res.z = z;
//  return res;
//}
//
//static inline struct q_vec4 vec4f_create(float x, float y, float z, float w) {
//  struct q_vec4 res;
//  res.x = x;
//  res.y = y;
//  res.z = z;
//  res.w = w;
//  return res;
//}
//
//inline const struct q_vec3 q_vec3_slerp(float t, const struct q_vec3* unitVec0, const struct q_vec3* unitVec1)
//{
//	float recipSinAngle, scale0, scale1, cosAngle, angle;
//	cosAngle = q_vec_dot(unitVec0, unitVec1);
//	if (cosAngle < VECTORMATH_SLERP_TOL)
//	{
//		angle = std::acosf(cosAngle);
//		recipSinAngle = (1.0f / std::sinf(angle));
//		scale0 = (std::sinf(((1.0f - t) * angle)) * recipSinAngle);
//		scale1 = (std::sinf((t * angle)) * recipSinAngle);
//	}
//	else
//	{
//		scale0 = (1.0f - t);
//		scale1 = t;
//	}
//	return ((unitVec0 * scale0) + (unitVec1 * scale1));
//}
//
//static inline struct q_vec3 vect3f_mul( const struct q_vec3 *v1, const struct q_vec3 *v2 ) {
//
//
//}
//
//static inline float __vec3f_dot(const struct q_vec3* v1, const struct q_vec3* v2) {
//
//	float result;
//	result = (vec_x(v1) * vec_x(v2));
//	result = (result + (vec_y(v1) * vec_y(v2)));
//	result = (result + (vec_z(v1) * vec_z(v2)));
//	return result;
//
//}
