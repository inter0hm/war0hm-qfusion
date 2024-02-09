#ifndef R_TEXTURE_FORMAT_DECODE_H
#define R_TEXTURE_FORMAT_DECODE_H

#include "../gameshared/q_arch.h"
#include "r_texture_format.h"

/**
* decodes a 4x4 block into 4x4x3 R_FORMAT_RGB8_UNORM
*
* the block is in row column order
**/
void R_ETC1DecodeBlock_RGB8(uint8_t* block, uint8_t* colors);

#endif
