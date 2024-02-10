#include "r_texture_buf.h"
#include "../gameshared/q_math.h"
#include "../gameshared/q_shared.h"
#include "r_texture_format.h"

uint16_t T_LogicalH( struct texture_buf_s *buf )
{
	switch( buf->def->base ) {
		case R_BASE_BLOCKED_COMPRESSED:
			return ( buf->width / buf->def->compressed.blockWidth ) + ( buf->width % buf->def->compressed.blockWidth == 0 ? 0 : 1 );
		default:
			break;
	}
	return buf->height;
}
uint16_t T_LogicalW( struct texture_buf_s *buf )
{
	switch( buf->def->base ) {
		case R_BASE_BLOCKED_COMPRESSED:
			return ( buf->height / buf->def->compressed.blockHeight ) + ( buf->width % buf->def->compressed.blockHeight == 0 ? 0 : 1 );
		default:
			break;
	}
	return buf->height;
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
	buf->def = def;
	switch( def->base ) {
		case R_BASE_BLOCKED_COMPRESSED: {
			buf->width = width;
			buf->height = height;
			buf->rowAlign = align;
			buf->rowPitch = ALIGN( T_LogicalW( buf ) * def->compressed.blockByteSize, align );
			buf->size = ( buf->rowPitch * T_LogicalH( buf ) );
			buf->def = def;
			break;
		}
		case R_BASE_FORMAT_FIXED_8: {
			buf->rowAlign = align;
			buf->rowPitch = ALIGN( width * def->fixed_8.numChannels, align );
			buf->size = ( buf->rowPitch * height );
			buf->width = width;
			buf->height = height;
			buf->def = def;
			break;
		}
		case R_BASE_FORMAT_FIXED_16: {
			buf->rowAlign = align;
			buf->rowPitch = ALIGN( width * def->fixed_16.numChannels * sizeof( uint16_t ), align );
			buf->size = ( buf->rowPitch * height );
			buf->width = width;
			buf->height = height;
			buf->def = def;
			break;
		}
		case R_BASE_FORMAT_FIXED_32: {
			buf->rowAlign = align;
			buf->rowPitch = ALIGN( width * def->fixed_32.numChannels * sizeof( uint32_t ), align );
			buf->size = ( buf->rowPitch * height );
			buf->width = width;
			buf->height = height;
			buf->def = def;
			break;
		}
		case R_BASE_FORMAT_PACKED_16: {
			buf->rowAlign = align;
			buf->rowPitch = ALIGN( width * sizeof( uint16_t ), align );
			buf->size = ( buf->rowPitch * height );
			buf->width = width;
			buf->height = height;
			buf->def = def;
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

void T_PromteTextureBuf( struct texture_buf_s *tex )
{
	if( ( tex->flags & TEX_BUF_IS_ALIASED ) > 0 ) {
		uint8_t *aliasBuffer = tex->buffer;
		tex->buffer = malloc( tex->size );
		memcpy( tex->buffer, aliasBuffer, tex->size );
	}
}

void T_ReallocTextureBuf( struct texture_buf_s *buf, const struct texture_buf_desc_s *desc)
{
  // we stash the previous size and the current buffer to transfer
  T_ConfigureTextureBuf( buf, desc->def, desc->width, desc->height, desc->alignment );
  if( ( buf->flags & TEX_BUF_IS_ALIASED ) > 0 ) {
	  buf->buffer = NULL; // if the buffer is aliased we don't own the memory
	  buf->capacity = buf->size;
  } else {
	  while( buf->size > buf->capacity ) {
		  buf->capacity = ( buf->capacity == 0 ) ? buf->size : ( buf->capacity >> 1 ) + buf->capacity;
	  }
  }
  buf->flags = (buf->flags & ~TEX_BUF_IS_ALIASED);
	buf->buffer = realloc( buf->buffer, buf->capacity );
}

void T_AliasTextureBuf( struct texture_buf_s *buf, const struct texture_buf_desc_s *desc, uint8_t *buffer, size_t size)
{
  T_ConfigureTextureBuf( buf, desc->def, desc->width, desc->height, desc->alignment );
	if( ( buf->flags & TEX_BUF_IS_ALIASED ) == 0 ) {
		free( buf->buffer ); // we free the underlying memory
	}
	buf->flags |= TEX_BUF_IS_ALIASED;
	if(size == 0 ){
		buf->capacity = buf->size;
	} else {
		buf->capacity = size;
		assert( buf->capacity > size );
	}
	buf->buffer = buffer;
}
void T_SwapEndianness( struct texture_buf_s *tex) {
	const uint32_t width = T_LogicalW( tex );
	const uint32_t height = T_LogicalH( tex );
	switch( tex->def->base ) {
		case R_BASE_FORMAT_FIXED_32: {
		  for( size_t row = 0; row < height; row++ ) {
			  for( size_t column = 0; column < width; column++ ) {
				  for( size_t c = 0; c < tex->def->fixed_32.numChannels; c++ ) {
					  uint32_t *const channel = ( (uint32_t *)&tex->buffer[( tex->rowPitch * row ) + ( column * RT_BlockSize( tex->def ) )] ) + c;
					  ( *channel ) = LongSwap( *channel );
				  }
			  }
			}
			break;
		}
		case R_BASE_FORMAT_FIXED_16: {
		  for( size_t row = 0; row < height; row++ ) {
			  for( size_t column = 0; column < width; column++ ) {
				  for( size_t c = 0; c < tex->def->fixed_16.numChannels; c++ ) {
					  uint16_t *const channel = ( (uint16_t *)&tex->buffer[( tex->rowPitch * row ) + ( column * RT_BlockSize( tex->def ) )] ) + c;
					  ( *channel ) = ShortSwap( *channel );
				  }
				}
			}
			break;
		}
		default:
			break;
	}
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
				  const uint8_t *b1 = &tex->buffer[tex->rowPitch * ( row * 2 + 0 ) + ( ( column * 2 + 0 ) * def->fixed_8.numChannels )];
				  const uint8_t *b2 = &tex->buffer[tex->rowPitch * ( row * 2 + 1 ) + ( ( column * 2 + 0 ) * def->fixed_8.numChannels )];
				  const uint8_t *b3 = &tex->buffer[tex->rowPitch * ( row * 2 + 0 ) + ( ( column * 2 + 1 ) * def->fixed_8.numChannels )];
				  const uint8_t *b4 = &tex->buffer[tex->rowPitch * ( row * 2 + 1 ) + ( ( column * 2 + 1 ) * def->fixed_8.numChannels )];
				  for( size_t c = 0; c < def->fixed_8.numChannels; c++ ) {
					  tex->buffer[( tex->rowPitch * row ) + column * def->fixed_8.numChannels] = ( b1[c] + b2[c] + b3[c] + b4[c] ) >> 2;
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
