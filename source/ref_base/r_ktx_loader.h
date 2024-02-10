#ifndef R_KTX_LOADER_H
#define R_KTX_LOADER_H

#include "r_texture_buf.h"
#include "r_texture_format.h"
#include "../gameshared/q_arch.h"

struct ktx_image_s {
  bool updated;
  struct texture_buf_s texture;
};

enum ktx_context_result_e {
  KTX_ERR_NONE = 0,
  KTX_ERR_INVALID_IDENTIFIER = -1,
  KTX_ERR_UNHANDLED_TYPE = -2,
  KTX_ERR_TRUNCATED = -3,
};

struct ktx_context_s {
	bool swapEndianess;
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

	uint8_t *buffer;
	const struct base_format_def_s *desc;
	struct ktx_image_s *textures;
};

enum ktx_context_result_e R_InitKTXContext( uint8_t *memory, size_t size, struct ktx_context_s *cntx );
uint16_t R_KTXGetNumberMips( const struct ktx_context_s *cntx );
uint16_t R_KTXGetNumberFaces( const struct ktx_context_s *cntx );
uint16_t R_KTXGetNumberArrayElements( const struct ktx_context_s *cntx );
struct texture_buf_s *R_ResolveKTXBuffer( struct ktx_context_s *cntx, uint32_t mipLevel, uint32_t faceIndex, uint32_t arrayOffset );
void R_FreeKTXContext(struct ktx_context_s* cntx);

#endif
