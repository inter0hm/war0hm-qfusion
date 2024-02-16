
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

static inline float __vec3f_x(const struct vec3f* v1) {
  return v1->x;
}

static inline float __vec3f_y(const struct vec3f* v1) {
  return v1->y;
}

static inline float __vec3f_z(const struct vec3f* v1) {
  return v1->z;
}

// vec3f
static inline struct q_vec3 vec3f_create(float x, float y, float z) {
  struct q_vec3 res;
  res.x = x;
  res.y = y;
  res.z = z;
  return res;
}

static inline struct q_vec4 vec4f_create(float x, float y, float z, float w) {
  struct q_vec4 res;
  res.x = x;
  res.y = y;
  res.z = z;
  res.w = w;
  return res;
}

inline const struct q_vec3 q_vec3_slerp(float t, const struct q_vec3* unitVec0, const struct q_vec3* unitVec1)
{
	float recipSinAngle, scale0, scale1, cosAngle, angle;
	cosAngle = q_vec_dot(unitVec0, unitVec1);
	if (cosAngle < VECTORMATH_SLERP_TOL)
	{
		angle = std::acosf(cosAngle);
		recipSinAngle = (1.0f / std::sinf(angle));
		scale0 = (std::sinf(((1.0f - t) * angle)) * recipSinAngle);
		scale1 = (std::sinf((t * angle)) * recipSinAngle);
	}
	else
	{
		scale0 = (1.0f - t);
		scale1 = t;
	}
	return ((unitVec0 * scale0) + (unitVec1 * scale1));
}

static inline struct q_vec3 vect3f_mul( const struct q_vec3 *v1, const struct q_vec3 *v2 ) {


}

static inline float __vec3f_dot(const struct q_vec3* v1, const struct q_vec3* v2) {

	float result;
	result = (vec_x(v1) * vec_x(v2));
	result = (result + (vec_y(v1) * vec_y(v2)));
	result = (result + (vec_z(v1) * vec_z(v2)));
	return result;

}
