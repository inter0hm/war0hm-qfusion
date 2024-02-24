#include "vectormath.h"
#include "../../gameshared/q_shared.h"

static const float VECTORMATH_SLERP_TOL = 0.999f;
static const double VECTORMATH_SLERP_TOL_D = 0.999;

#define q_vec_x(X) _Generic((X), \
    struct vec3f: __vec3f_x \
  )(X)

#define q_vec_y(X) _Generic((X), \
    struct vec3f: __vec3f_y \
  )(X)

#define q_vec_z(X) _Generic((X), \
    struct vec3f: __vec3f_z \
  )(X)

#define q_vec_dot(X,Y) _Generic((X), \
    struct vec3f: __vec3f_dot \
  )(X)

inline const struct vec3_s vec3_create( float x, float y, float z )
{
	struct vec3_s res = { .x = x, .y = y, .z = z };
	return res;
}

inline const struct vec3_s vec3_create_single_scalar( float x )
{
	struct vec3_s res = { .x = x, .y = x, .z = x };
	return res;
}

inline const struct vec3_s vec3_create_vec3( const struct vec3_s *v )
{
	struct vec3_s res = *v;
	return res;
}

inline const struct vec3_s vec3_slerp( float t, const struct vec3_s *unitVec0, const struct vec3_s *unitVec1 ) {
	float recipSinAngle, scale0, scale1, cosAngle, angle;
	cosAngle = dot(unitVec0, unitVec1);
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
	return ((unitVec0 * scale0) + (unitVec1 * scale1));

}
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
