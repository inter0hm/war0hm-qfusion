#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../vectormath.h"

int main( void )
{
	const struct CMUnitTest tests[] = {
//		cmocka_unit_test( test_alias_texture_buffer ),
//		cmocka_unit_test( test_swizzle_texture_buffer_rgb8_unorm_grba ),
//		cmocka_unit_test( test_promote_textuire_buffer ),
//		cmocka_unit_test( test_swap_texture_buffer_short_4_4_4_4_rgba_grba ),
	};

	return cmocka_run_group_tests( tests, NULL, NULL );
}
