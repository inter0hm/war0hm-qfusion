#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../q_math.h"
#include "../q_arch.h"

void test_vec3_vec3_add() {
	struct {
		struct vec3_s v1, v2;
		struct vec3_s expected;
	} test[] = {
		{q_vec3_create(1, 0, 0), q_vec3_create(1, 1, 1), q_vec3_create(2, 1, 1)},
		{q_vec3_create(0, 0, 0), q_vec3_create(1, 1, 1), q_vec3_create(1, 1, 1)},
		{q_vec3_create(-1, -1, -1), q_vec3_create(1, 1, 1), q_vec3_create(0, 0, 0)}
	};

	for(size_t i = 0; i < Q_ARRAY_COUNT(test); i++){
		struct vec3_s res = q_vec_add(test[i].v1, test[i].v2);
			assert_float_equal( 
				q_vec3_element( res, 0 ), 
				q_vec3_element( test[i].expected, 0 ), 0.9999f );
			assert_float_equal( 
				q_vec3_element( res, 1 ), 
				q_vec3_element( test[i].expected, 1 ), 0.9999f );
			assert_float_equal( 
				q_vec3_element( res, 2 ), 
				q_vec3_element( test[i].expected, 2 ), 0.9999f );
	}
}

int main( void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_vec3_vec3_add),
//		cmocka_unit_test( test_swizzle_texture_buffer_rgb8_unorm_grba ),
//		cmocka_unit_test( test_promote_textuire_buffer ),
//		cmocka_unit_test( test_swap_texture_buffer_short_4_4_4_4_rgba_grba ),
	};

	return cmocka_run_group_tests( tests, NULL, NULL );
}
