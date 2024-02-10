#include "r_ktx_loader.h"
#include "../gameshared/q_shared.h"
#include "../gameshared/q_math.h"

#include "r_texture_buf.h"
#include "r_texture_format.h"
#include "gl_format.h"

#include "stb_ds.h"

#pragma pack( push, 1 )
struct __raw_ktx_header_s {
	char identifier[12];
	int endianness;
	int type;
	int typeSize;
	int format;
	int internalFormat;
	int baseInternalFormat;
	int pixelWidth;
	int pixelHeight;
	int pixelDepth;
	int numberOfArrayElements;
	int numberOfFaces;
	int numberOfMipmapLevels;
	int bytesOfKeyValueData;
};
#pragma pack( pop )


static struct ktx_image_s *R_KTXGetImage( const struct ktx_context_s *cntx, uint32_t mipLevel, uint32_t faceIndex, uint32_t arrayOffset )
{
	assert( cntx );
	assert( cntx->textures);
	const size_t numberOfArrayElements = max( 1, cntx->numberOfArrayElements );
	const size_t numberOfFaces = max( 1, cntx->numberOfFaces );
	const size_t index = ( mipLevel * numberOfArrayElements * numberOfFaces ) + ( faceIndex * numberOfArrayElements ) + arrayOffset;
	assert( index < arrlen( cntx->textures) );
	return &cntx->textures[index];
}

struct texture_buf_s *R_KTXResolveBuffer( struct ktx_context_s *cntx, uint32_t mipLevel, uint32_t faceIndex, uint32_t arrayOffset ) {
	struct ktx_image_s *img = R_KTXGetImage( cntx, mipLevel, faceIndex, arrayOffset);
	if(!img->updated) {
		switch( cntx->desc->base ) {
			case R_BASE_FORMAT_FIXED_16:
			case R_BASE_FORMAT_FIXED_32:
			case R_BASE_FORMAT_PACKED_16:
			  T_SwapEndianness( &img->texture );
				break;
			default:
				break;
		}
		img->updated = true;
	}
	return &img->texture;
}
enum ktx_context_result_e  R_InitKTXContext( uint8_t *memory, size_t size, struct ktx_context_s *cntx ) {
	assert( sizeof( struct __raw_ktx_header_s ) == 64 );
	struct __raw_ktx_header_s *rawHeader = (struct __raw_ktx_header_s *)memory;
	if( memcmp( rawHeader->identifier, "\xABKTX 11\xBB\r\n\x1A\n", 12 ) ) {
		return KTX_ERR_INVALID_IDENTIFIER;
	}
  assert(cntx->pixelDepth == 0);

	const bool swapEndian = ( rawHeader->endianness == 0x01020304 ) ? true : false;
	cntx->buffer = memory;
	cntx->swapEndianess = swapEndian;
	cntx->type = swapEndian ? LongSwap( rawHeader->type ) : rawHeader->type;
	cntx->typeSize = swapEndian ? LongSwap( rawHeader->typeSize ) : rawHeader->typeSize;
	cntx->format = swapEndian ? LongSwap( rawHeader->format ) : rawHeader->format;
	cntx->internalFormat = swapEndian ? LongSwap( rawHeader->internalFormat ) : rawHeader->internalFormat;
	cntx->baseInternalFormat = swapEndian ? LongSwap( rawHeader->baseInternalFormat ) : rawHeader->baseInternalFormat;
	cntx->pixelWidth = swapEndian ? LongSwap( rawHeader->pixelWidth ) : rawHeader->pixelWidth;
	cntx->pixelHeight = swapEndian ? LongSwap( rawHeader->pixelHeight ) : rawHeader->pixelHeight;
	cntx->pixelDepth = swapEndian ? LongSwap( rawHeader->pixelDepth ) : rawHeader->pixelDepth;
	cntx->numberOfArrayElements = swapEndian ? LongSwap( rawHeader->numberOfArrayElements ) : rawHeader->numberOfArrayElements;
	cntx->numberOfFaces = swapEndian ? LongSwap( rawHeader->numberOfFaces ) : rawHeader->numberOfFaces;
	cntx->numberOfMipmapLevels = swapEndian ? LongSwap( rawHeader->numberOfMipmapLevels ) : rawHeader->numberOfMipmapLevels;
	cntx->bytesOfKeyValueData = swapEndian ? LongSwap( rawHeader->bytesOfKeyValueData ) : rawHeader->bytesOfKeyValueData;

	const size_t numberOfArrayElements = max( 1, cntx->numberOfArrayElements );
	const size_t numberOfFaces = max( 1, cntx->numberOfFaces );
	const size_t numberOfMips = max( 1, cntx->numberOfMipmapLevels );

	//const size_t keyValueOffset = sizeof(struct __raw_ktx_header_s);
	const size_t dataOffset = sizeof( struct __raw_ktx_header_s ) + cntx->bytesOfKeyValueData;
	arrsetlen( cntx->textures, numberOfArrayElements * numberOfFaces * numberOfMips );
	if( cntx->type == 0 ) {
		switch( cntx->internalFormat ) {
			case GL_ETC1_RGB8_OES:
				cntx->desc = R_BaseFormatDef( R_FORMAT_ETC1_R8G8B8_OES );
				break;
			case GL_COMPRESSED_RGB8_ETC2:
				cntx->desc  = R_BaseFormatDef( R_FORMAT_ETC2_R8G8B8_UNORM );
				break;
			default:
				assert(false);
				return KTX_ERR_UNHANDLED_TYPE;
		}
	} else {
		switch( cntx->type ) {
			case GL_UNSIGNED_SHORT_4_4_4_4:
				cntx->desc = R_BaseFormatDef( R_FORMAT_R4_G4_B4_A4_UNORM );
				break;
			case GL_UNSIGNED_SHORT_5_6_5:
				cntx->desc = R_BaseFormatDef( R_FORMAT_R5_G6_B5_UNORM );
				break;
			case GL_UNSIGNED_SHORT_5_5_5_1:
				cntx->desc = R_BaseFormatDef( R_FORMAT_R5_G5_B5_A1_UNORM );
				break;
			case GL_UNSIGNED_BYTE: {
				switch( cntx->baseInternalFormat ) {
					case GL_RGBA:
				    cntx->desc = R_BaseFormatDef( R_FORMAT_RGBA8_UNORM );
						break;
					case GL_BGRA:
				    cntx->desc = R_BaseFormatDef( R_FORMAT_BGRA8_UNORM );
						break;
					case GL_RGB:
				    cntx->desc = R_BaseFormatDef( R_FORMAT_RGB8_UNORM );
						break;
					case GL_BGR:
				    cntx->desc = R_BaseFormatDef( R_FORMAT_BGR8_UNORM );
						break;
					case GL_LUMINANCE_ALPHA:
				    cntx->desc = R_BaseFormatDef( R_FORMAT_L8_A8_UNORM );
						break;
					case GL_LUMINANCE:
				    cntx->desc = R_BaseFormatDef( R_FORMAT_L8_UNORM );
						break;
					case GL_ALPHA:
				    cntx->desc = R_BaseFormatDef( R_FORMAT_A8_UNORM );
						break;
					default:
				    assert(false);
						return KTX_ERR_UNHANDLED_TYPE;
				}
				break;
			}
			default:
				assert(false);
				return KTX_ERR_UNHANDLED_TYPE;
		}
	}

	// mip and faces are aligned 4
	// KTX File Format Spec: https://registry.khronos.org/KTX/specs/1.0/ktxspec.v1.html
	uint32_t width = cntx->pixelWidth;
	uint32_t height = cntx->pixelHeight;
	size_t offset = dataOffset;
	for( size_t mipLevel = 0; mipLevel < numberOfMips; mipLevel++ ) {
		uint32_t bufferLen = cntx->swapEndianess ? LongSwap( *( (uint32_t *)( cntx->buffer + offset ) ) ) : ( *( (uint32_t *)( cntx->buffer + offset ) ) );
		offset += sizeof( uint32_t );

		size_t imgOffset = 0;
		for( size_t arrayIdx = 0; arrayIdx < numberOfArrayElements; arrayIdx++ ) {
			for( size_t faceIdx = 0; faceIdx < numberOfFaces; faceIdx++ ) {
				struct ktx_image_s *img = R_KTXGetImage( cntx, mipLevel, faceIdx, arrayIdx );
				memset( img, 0, sizeof( struct ktx_image_s ) );
				struct texture_buf_desc_s desc = { .width = width, .height = height, .alignment = 4, .def = cntx->desc };
				T_AliasTextureBuf( &img->texture, &desc, ( cntx->buffer + offset + imgOffset ), 0 );
				imgOffset += img->texture.size;
			}
		}
		offset = ALIGN( offset + bufferLen, 4 );
		width = max( 1, width >> 1 );
		height = max( 1, height >> 1 );
	}

  if(offset > size ) {
    return KTX_ERR_TRUNCATED; 
  }
  return KTX_ERR_NONE;
}
uint16_t R_KTXGetNumberMips( const struct ktx_context_s *cntx )
{
	return max( 1, cntx->numberOfMipmapLevels );
}
uint16_t R_KTXGetNumberFaces( const struct ktx_context_s *cntx )
{
	return max( 1, cntx->numberOfFaces );
}
uint16_t R_KTXGetNumberArrayElements( const struct ktx_context_s *cntx )
{
	return max( 1, cntx->numberOfArrayElements );
}

void R_KTXFreeContext(struct ktx_context_s* context) {
	arrfree(context->buffer);
}

//void R_AliasBuffer( const struct ktx_context_s *cntx, struct texture_buf_s *tex, uint32_t faceIndex, uint32_t arrOffset, uint16_t mipLevel ){
//	assert( tex );
//	struct ktx_image_s *ktxImage = R_KTXGetImage( cntx, mipLevel, faceIndex, arrOffset );
//	assert( ktxImage );
// // struct texture_buf_desc_s desc = {
// //   .width = ktxImage->width,
// //   .height = ktxImage->height,
// //   .alignment = 4,
// //   .def = cntx->desc
// // };
// // T_AliasTextureBuf(tex, &desc, cntx->buffer + ktxImage->offset, ktxImage->size);
//
//	//
//	//	R_ConfigureImageBuffer( image, &img->layout);
//	//	memcpy( image->data, (cntx->buffer + img->offset), img->layout.size);
//	//	if( !R_FormatIsCompressed( img->layout.format) && cntx->swapEndianess ) {
//	//		R_Buf_SwapEndianess( image );
//	//	}
//}
