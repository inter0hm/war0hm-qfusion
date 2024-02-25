
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

inline float vec3_dot( const struct vec3_s v1, const struct vec3_s v2);
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

inline const float q_vec3_cross( const struct vec3_s v1, const struct vec3_s v2);
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
	)(a,b)
	#define q_vec_add(a,b) _Generic(__Q_MATH_VEC_GENERIC_2(a,b), \
		void(*)(const struct vec3_s, const struct vec3_s): q_vec3_add, \
		void(*)(const struct vec3_s, float): q_vec3_add_scalar \
	)(a,b)
#endif


#include "vector.inl"
