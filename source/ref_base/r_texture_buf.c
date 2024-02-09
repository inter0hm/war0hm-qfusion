#include "r_texture_buf.h"
#include "../gameshared/q_math.h"

uint16_t T_LogicalH( struct texture_buf_s *buf )
{
	return buf->height;
}
uint16_t T_LogicalW( struct texture_buf_s *buf )
{
	return buf->width;
}
uint16_t T_PixelW( struct texture_buf_s *buf )
{
	return buf->width;
}
uint16_t T_PixelH( struct texture_buf_s *buf )
{
	return buf->height;
}

static void T_ConfigureTextureBuf( struct texture_buf_s *buf, const struct base_format_def_s *def, uint32_t width, uint32_t height, uint16_t align )
{
	switch( def->base ) {
		case R_BASE_FORMAT_FIXED_8: {
			buf->rowAlign = align;
			buf->rowPitch = ALIGN( width * def->fixed_8.blockByteSize, align );
			buf->size = ( buf->rowPitch * height );
			buf->width = width;
			buf->height = height;
			break;
		}
		default:
			assert( 0 );
			break;
	}
}

size_t T_Size( struct texture_buf_s *buf )
{
	return buf->size;
}
void T_ReallocTextureBuf( struct texture_buf_s *buf, const struct texture_buf_desc_s *desc )
{
	if( ( buf->flags & TEX_BUF_IS_ALIASED ) > 0 ) {
		buf->buffer = NULL;
	}
	while( buf->size > buf->capacity ) {
		buf->capacity = ( buf->capacity == 0 ) ? buf->size : ( buf->capacity >> 1 ) + buf->capacity;
	}
	buf->buffer = realloc( buf->buffer, buf->capacity );
}
void T_AliasTextureBuf( struct texture_buf_s *buf, const struct texture_buf_desc_s *desc, uint8_t *buffer )
{
	if( ( buf->flags & TEX_BUF_IS_ALIASED ) == 0 ) {
		free( buf->buffer ); // we free the underlying memory
	}
	buf->buffer = buffer;
}


void T_MipMapQuarterInPlace( struct texture_buf_s *tex) {
	const uint32_t halfWidth = ( T_LogicalW(tex) >> 1 );
	const uint32_t halfHeight = ( T_LogicalH(tex) >> 1 );
  const struct base_format_def_s* def = tex->def;
  switch( tex->def->base ) {
	  case R_BASE_FORMAT_FIXED_8: {
      assert(def->fixed_8.normalized);
		  for( size_t row = 0; row < halfHeight; row++ ) {
			  for( size_t column = 0; column < halfWidth; column++ ) {
				  const uint8_t *b1 = &tex->buffer[tex->rowPitch * ( row * 2 + 0 ) + ( ( column * 2 + 0 ) * def->fixed_8.blockByteSize )];
				  const uint8_t *b2 = &tex->buffer[tex->rowPitch * ( row * 2 + 1 ) + ( ( column * 2 + 0 ) * def->fixed_8.blockByteSize )];
				  const uint8_t *b3 = &tex->buffer[tex->rowPitch * ( row * 2 + 0 ) + ( ( column * 2 + 1 ) * def->fixed_8.blockByteSize )];
				  const uint8_t *b4 = &tex->buffer[tex->rowPitch * ( row * 2 + 1 ) + ( ( column * 2 + 1 ) * def->fixed_8.blockByteSize )];
				  for( size_t c = 0; c < def->fixed_8.numChannels; c++ ) {
					  tex->buffer[( tex->rowPitch * row ) + column * def->fixed_8.blockByteSize] = ( b1[c] + b2[c] + b3[c] + b4[c] ) >> 2;
				  }
			  }
		  }
		  break;
	  }
	  default:
		  assert( 0 );
		  break;
  }
}
