/*
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2002-2013 Victor Luchits

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef R_RESOURCE_UPLOAD_H
#define R_RESOURCE_UPLOAD_H

#include "../gameshared/q_shared.h"
#include "ri_format.h"
#include "ri_types.h"

#define RI_RESOURCE_UPLOADER_SET_SIZE (5)


static const uint32_t SizeOfStageBufferByte = 8 * MB_TO_BYTE;
//static const NriAccessLayoutStage ResourceUploadTexturePostAccess = { 
//	.layout = NriLayout_COPY_DESTINATION, 
//	.access = NriAccessBits_COPY_DESTINATION, 
//	.stages = NriStageBits_COPY 
//};
//static const NriAccessStage ResourceUploadBufferPostAccess = { 
//	.access = NriLayout_COPY_DESTINATION, 
//	.stages = NriStageBits_COPY 
//};

struct RIBufferUploadDesc_s {
	//NriBuffer *target;
	//NriAccessStage before;
	//NriAccessStage after; 
		union {
#if ( DEVICE_IMPL_VULKAN )
			struct {
				VkBuffer target;

				// the backing stage buffer
				VkBuffer stage;
				size_t stageOffset;
			} vk;
#endif
		};

	size_t numBytes;
	size_t byteOffset;

	// begin mapping
	void *data;

//		union {
//#if ( DEVICE_IMPL_VULKAN )
//			struct {
//				VkBuffer src;
//				struct VmaAllocation_T *allocator;
//			} vk;
//#endif
//		} internal;

	//struct {
	//	NriBuffer *backing;
	//	size_t byteOffset;
	//} internal;
};
//buffer_upload_desc_t;

struct RITextureUploadDesc_s {
	//NriTexture *target;
	//NriAccessLayoutStage before;
	//NriAccessLayoutStage after;

	union {
#if ( DEVICE_IMPL_VULKAN )
		struct {
			VkImage target;

			VkBuffer stage;
			size_t stageOffset;
		} vk;
#endif

	};

	enum RI_Format_e format;

	// https://github.com/microsoft/DirectXTex/wiki/Image
	uint32_t sliceNum;
	uint32_t rowPitch;

	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint32_t width;
	uint32_t height;
	uint32_t depth;

	uint32_t arrayOffset;
	uint32_t mipOffset;

	// begin mapping
	void *data;
	uint32_t alignRowPitch;
	uint32_t alignSlicePitch;
};
//texture_upload_desc_t;

struct RIResourceTempAlloc_s{
		union {
#if ( DEVICE_IMPL_VULKAN )
			struct {
				VkBufferView blockView;
				VkBuffer buffer;
				struct VmaAllocation_T *allocator;
			} vk;
#endif
		};
};

struct RIResourceUploader_s {
	// stage buffer
	struct {
		union {
#if ( DEVICE_IMPL_VULKAN )
			struct {
				VkBufferView blockView;
				VkBuffer buffer;
				struct VmaAllocation_T *allocator;
			} vk;
#endif
		};
		uint8_t *pMappedAddress;

		size_t tailOffset;
		size_t remaningSpace;
	} stage;


	union {
#if ( DEVICE_IMPL_VULKAN )
		struct {
			VkSemaphore sem;
		} vk;
#endif
	};

	uint32_t cmdSetCount;
	struct RIQueue_s* queue;
	struct {
		size_t reservedStageMemory;
		struct RIResourceTempAlloc_s* temporary;
		union {
#if ( DEVICE_IMPL_VULKAN )
			struct {
				VkCommandPool pool;
				VkCommandBuffer cmd;
			} vk;
#endif
		};
	} cmdSets[RI_RESOURCE_UPLOADER_SET_SIZE];
};

void RI_InitResourceUploader(struct RIDevice_s* device, struct RIResourceUploader_s* uploader );
void RI_ExitResourceUpload(struct RIResourceUploader_s* uploader);

//void R_InitResourceUpload();

// buffer upload
// NriAccessStage R_ResourceTransitionBuffer( NriBuffer *buffer, NriAccessStage currentAccessAndLayout );
void R_ResourceBeginCopyBuffer( struct RIDevice_s *device, struct RIResourceUploader_s *uploader, struct RIBufferUploadDesc_s  *action );
void R_ResourceEndCopyBuffer( struct RIDevice_s *device, struct RIResourceUploader_s *uploader, struct RIBufferUploadDesc_s *action );

// texture upload
// void R_ResourceTransitionTexture( NriTexture *texture, NriAccessLayoutStage currentAccessAndLayout );
void R_ResourceBeginCopyTexture( struct RIDevice_s *device, struct RIResourceUploader_s *uploader, struct RITextureUploadDesc_s *desc );
void R_ResourceEndCopyTexture( struct RIDevice_s *device, struct RIResourceUploader_s *uploader, struct RITextureUploadDesc_s *desc );
void RI_ResourceSubmit( struct RIDevice_s *device, struct RIResourceUploader_s *uploader );

void R_FlushTransitionBarrier( struct RIDevice_s *device, struct RIResourceUploader_s *uploader );

#endif
