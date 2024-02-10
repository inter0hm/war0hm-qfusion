#ifndef R_TEXTURE_BUF_H
#define R_TEXTURE_BUF_H

#include "../gameshared/q_arch.h"
#include "r_texture_format.h"

enum texture_buf_flags_e {
	TEX_BUF_IS_ALIASED = 0x1,
	TEX_BUF_MAX = 0xffff
};

struct texture_buf_s {
	enum texture_buf_flags_e flags;
  const struct base_format_def_s* def;
	
	uint32_t rowPitch; // the number of bytes in a row of pixels including any alignment
  uint16_t rowAlign;
  
  uint16_t width;
  uint16_t height;

  size_t capacity;
  size_t size;
  uint8_t *buffer;
};

struct texture_buf_desc_s {
	uint32_t width;
	uint32_t height;
	uint16_t alignment;
	const struct base_format_def_s *def;
};
size_t T_Size( struct texture_buf_s *buf );
uint16_t T_LogicalH( struct texture_buf_s *buf );
uint16_t T_LogicalW( struct texture_buf_s *buf );
uint16_t T_PixelW( struct texture_buf_s *buf);
uint16_t T_PixelH( struct texture_buf_s *buf);

/**
* If a buffer is aliased then it will allocate and copy the alised memory ovecr
**/
void T_PromteTextureBuf( struct texture_buf_s *buf);
void T_ReallocTextureBuf( struct texture_buf_s *buf, const struct texture_buf_desc_s *desc);
void T_AliasTextureBuf( struct texture_buf_s *buf, const struct texture_buf_desc_s *desc, uint8_t *buffer, size_t size);

void T_MipMapQuarterInPlace( struct texture_buf_s *tex);
void T_SwapEndianness( struct texture_buf_s *tex);

#endif
