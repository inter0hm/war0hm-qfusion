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
	uint32_t usageBits; // RITextureUsageBits_e  
};

int InitRITexture( struct RIDevice_s *dev, const struct RITextureDesc_s *desc, struct RITexture_s *tex, struct RIMemory_s* mem);
void FreeRITexture( struct RIDevice_s *dev, struct RITexture_s *tex );

struct RIBufferDesc_s {
  uint64_t size;
  uint32_t usageBits; // RIBufferUsage_e
  uint64_t structureStride; // optional field if the data has a stride
};

int initRIBuffer(struct RIDevice_s* dev, const struct RIBufferDesc_s* desc, struct RIBuffer_s *buf, struct RIMemory_s* mem);

void FreeRIMemory(struct RIDevice_s* dev, struct RIMemory_s* mem);
#endif
