#ifndef RI_RESOURCE_H
#define RI_RESOURCE_H

#include "ri_types.h"

struct RITextureDesc_s {
	uint16_t type;		  // RITextureType_e 
	uint16_t sampleNum; // RISampleCount_e
	uint32_t format;	  // RI_Format_e
	uint16_t width, height, depth;
	uint8_t layerNum;
	uint8_t mipNum;
	uint8_t usageBits; // RIUsageBits_e 
};

int InitRITexture( struct RIDevice_s *dev, const struct RITextureDesc_s *desc, struct RITexture_s *tex );

struct RIBufferDesc_s {

};
#endif
