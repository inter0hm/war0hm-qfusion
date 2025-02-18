#include "r_resource_upload.h"
#include "../gameshared/q_math.h"
#include "r_nri.h"
#include "stb_ds.h"


#include <assert.h>
#include <vk_mem_alloc.h>

#include "r_local.h"

#define NUMBER_COMMAND_SETS 3

typedef struct {
	NriMemory *memory;
	NriBuffer *buffer;
} temporary_resource_buf_t;

typedef struct command_set_s {
	size_t offset;
	NriCommandAllocator *allocator;
	NriCommandBuffer *cmd;
	uint32_t reservedStageMemory;
	temporary_resource_buf_t *temporary;

	NriTexture** seenTextures;
	NriBuffer** seenBuffers;
} resource_command_set_t;

typedef struct resource_stage_buffer_s {
	union {
#if ( DEVICE_IMPL_VULKAN )
		struct {
			VkBufferView blockView;	
		  VkBuffer buffer;
		  struct VmaAllocation_T * allocator;
		} vk;
#endif
	};
	uint8_t* pMappedAddress;

	size_t tailOffset;
	size_t remaningSpace;
} resource_stage_buffer_t;

typedef struct {
		union {
#if ( DEVICE_IMPL_VULKAN )
			struct {
				VkBuffer buffer;
			} vk;
#endif
		};
	uint64_t byteOffset;
	void *cpuMapping;
} resource_stage_response_t;

static bool __R_AllocFromStageBuffer( struct RIResourceUploader_s* uploader, size_t reqSize, resource_stage_response_t *res )
{
	size_t allocSize = ALIGN( reqSize, 4 ); // we round up to multiples of uint32_t
	if( allocSize > uploader->stage.remaningSpace ) {
		// we are out of avaliable space from staging
		return false;
	}

	// we are past the end of the buffer
	if( uploader->stage.tailOffset + allocSize > SizeOfStageBufferByte ) {
		const size_t remainingSpace = ( SizeOfStageBufferByte - uploader->stage.tailOffset ); // remaining space at the end of the buffer this unusable
		if( allocSize > uploader->stage.remaningSpace - remainingSpace ) {
			return false;
		}
		
		uploader->stage.remaningSpace -= remainingSpace;
		uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].reservedStageMemory += remainingSpace; // give the remaning space to the requesting set
		uploader->stage.tailOffset = 0;
	}

	res->byteOffset = uploader->stage.tailOffset;
	res->vk.buffer = uploader->stage.vk.buffer;
	res->cpuMapping = ( (uint8_t *)uploader->stage.pMappedAddress ) + uploader->stage.tailOffset;

	uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].reservedStageMemory += allocSize;

	uploader->stage.tailOffset += allocSize;
	uploader->stage.remaningSpace -= allocSize;
	return true;
}

static bool __R_AllocTemporaryBuffer(struct RIDevice_s *device, struct RIResourceUploader_s* uploader, size_t reqSize, resource_stage_response_t *res )
{
	assert( res );
	if( __R_AllocFromStageBuffer( uploader, reqSize, res ) ) {
		return true;
	}

	struct RIResourceTempAlloc_s temp = {};
	GPU_VULKAN_BLOCK( device->renderer, ( {
						  VkBufferCreateInfo stageBufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
						  stageBufferCreateInfo.pNext = NULL;
						  stageBufferCreateInfo.flags = 0;
						  stageBufferCreateInfo.size = reqSize;
						  stageBufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
						  stageBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
						  stageBufferCreateInfo.queueFamilyIndexCount = 0;
						  stageBufferCreateInfo.pQueueFamilyIndices = NULL;
						  VK_WrapResult( vkCreateBuffer( device->vk.device, &stageBufferCreateInfo, NULL, &temp.vk.buffer ) );

						  VmaAllocationInfo allocationInfo = {};
						  VmaAllocationCreateInfo allocInfo = {};
						  allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
						  vmaAllocateMemoryForBuffer( device->vk.vmaAllocator, temp.vk.buffer, &allocInfo, &temp.vk.allocator, &allocationInfo );
						  vmaBindBufferMemory2( device->vk.vmaAllocator, temp.vk.allocator, 0, uploader->stage.vk.buffer, NULL );
						  res->cpuMapping = allocationInfo.pMappedData;
						  res->vk.buffer = temp.vk.buffer;

						  VkBufferViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO };
						  createInfo.flags = (VkBufferViewCreateFlags)0;
						  createInfo.buffer = uploader->stage.vk.buffer;
						  createInfo.format = VK_FORMAT_UNDEFINED;
						  createInfo.offset = 0;
						  createInfo.range = SizeOfStageBufferByte;
						  VK_WrapResult( vkCreateBufferView( device->vk.device, &createInfo, NULL, &temp.vk.blockView ) );
					  } ) );
	arrput( uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].temporary, temp );
	res->byteOffset = 0;
	return true;

	//// Com_Printf( "Creating temporary buffer ran out space in staging" );
	//temporary_resource_buf_t temp = {0};
	//NriBufferDesc bufferDesc = { .size = reqSize };
	//NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateBuffer( rsh.nri.device, &bufferDesc, &temp.buffer ) );

	//struct NriMemoryDesc memoryDesc = {0};
	//rsh.nri.coreI.GetBufferMemoryDesc( rsh.nri.device, &bufferDesc, NriMemoryLocation_HOST_UPLOAD, &memoryDesc );

	//NriAllocateMemoryDesc allocateMemoryDesc = {0};
	//allocateMemoryDesc.size = memoryDesc.size;
	//allocateMemoryDesc.type = memoryDesc.type;
	//NRI_ABORT_ON_FAILURE( rsh.nri.coreI.AllocateMemory( rsh.nri.device, &allocateMemoryDesc, &temp.memory ) );

	//NriBufferMemoryBindingDesc bindBufferDesc = {
	//	.memory = temp.memory,
	//	.buffer = temp.buffer,
	//};
	//NRI_ABORT_ON_FAILURE( rsh.nri.coreI.BindBufferMemory( rsh.nri.device, &bindBufferDesc, 1 ) );

}

//static void __UploadBeginCommandSet(struct RIDevice_s *device, uint32_t setIndex )
//{
//	struct command_set_s *set = &commandSets[activeSet];
//	stageBuffer.remaningSpace += set->reservedStageMemory;
//	set->reservedStageMemory = 0;
//	rsh.nri.coreI.BeginCommandBuffer( set->cmd, 0 );
//}

static void R_UploadEndCommandSet( uint32_t setIndex )
{
	struct command_set_s *set = &commandSets[activeSet];
	const NriCommandBuffer *cmdBuffers[] = { set->cmd };

	NriFenceSubmitDesc signalFence = { 0 };
	signalFence.fence = uploadFence;
	signalFence.value = 1 + syncIndex;

	rsh.nri.coreI.EndCommandBuffer( set->cmd );
	NriQueueSubmitDesc queueSubmit = { 0 };
	queueSubmit.commandBuffers = cmdBuffers;
	queueSubmit.commandBufferNum = 1;
	queueSubmit.signalFenceNum = 1;
	queueSubmit.signalFences = &signalFence;
	rsh.nri.coreI.QueueSubmit( cmdQueue, &queueSubmit );
	syncIndex++;
}

void R_ExitResourceUpload() {
	rsh.nri.coreI.DestroyBuffer(stageBuffer.buffer);
	rsh.nri.coreI.FreeMemory(stageBuffer.memory);

	syncIndex = 0;
	activeSet = 0;

	rsh.nri.coreI.DestroyFence(uploadFence);

	for( size_t i = 0; i < Q_ARRAY_COUNT( commandSets ); i++ ) {
		resource_command_set_t *set = &commandSets[i];
		rsh.nri.coreI.DestroyCommandBuffer( set->cmd );
		rsh.nri.coreI.DestroyCommandAllocator( set->allocator );
		for( size_t i = 0; i < arrlen( set->temporary ); i++ ) {
			rsh.nri.coreI.DestroyBuffer( set->temporary[i].buffer );
			rsh.nri.coreI.FreeMemory( set->temporary[i].memory );
		}
		arrfree(set->temporary);
		arrfree(set->seenTextures);
		arrfree(set->seenBuffers);
	}
}

void RI_InitResourceUploader( struct RIDevice_s *device, struct RIResourceUploader_s *uploader )
{
	GPU_VULKAN_BLOCK( device->renderer, ( {
						  VkBufferCreateInfo stageBufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
						  stageBufferCreateInfo.pNext = NULL;
						  stageBufferCreateInfo.flags = 0;
						  stageBufferCreateInfo.size = SizeOfStageBufferByte;
						  stageBufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
						  stageBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
						  stageBufferCreateInfo.queueFamilyIndexCount = 0;
						  stageBufferCreateInfo.pQueueFamilyIndices = NULL;
						  VK_WrapResult( vkCreateBuffer( device->vk.device, &stageBufferCreateInfo, NULL, &uploader->stage.vk.buffer ) );

						  VmaAllocationInfo allocationInfo = {};
						  VmaAllocationCreateInfo allocInfo = {};
						  allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
						  vmaAllocateMemoryForBuffer( device->vk.vmaAllocator, uploader->stage.vk.buffer, &allocInfo, &uploader->stage.vk.allocator, &allocationInfo );
						  vmaBindBufferMemory2( device->vk.vmaAllocator, uploader->stage.vk.allocator, 0, uploader->stage.vk.buffer, NULL );
						  uploader->stage.pMappedAddress = allocationInfo.pMappedData;

						  VkBufferViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO };
						  createInfo.flags = (VkBufferViewCreateFlags)0;
						  createInfo.buffer = uploader->stage.vk.buffer;
						  createInfo.format = VK_FORMAT_UNDEFINED;
						  createInfo.offset = 0;
						  createInfo.range = SizeOfStageBufferByte;
						  VK_WrapResult( vkCreateBufferView( device->vk.device, &createInfo, NULL, &uploader->stage.vk.blockView ) );
						  for( size_t i = 0; i < NUMBER_FRAMES_FLIGHT; i++ ) {
							  {
								  VkCommandPoolCreateInfo cmdPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
								  cmdPoolCreateInfo.queueFamilyIndex = graphicsQueue->vk.queueFamilyIdx;
								  cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
								  VK_WrapResult( vkCreateCommandPool( device->vk.device, &cmdPoolCreateInfo, NULL, &uploader->cmdSets[i].vk.pool ) );
							  }
							  {
								  VkCommandBufferAllocateInfo cmdAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
								  cmdAllocInfo.commandPool = rsh.frameSets[i].vk.pool;
								  cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
								  cmdAllocInfo.commandBufferCount = 1;
								  VK_WrapResult( vkAllocateCommandBuffers( device->vk.device, &cmdAllocInfo, &uploader->cmdSets[i].vk.cmd) );
							  }
						  }

						  {
							  VkCommandBufferBeginInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
							  info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
							  vkBeginCommandBuffer( uploader->cmdSets[uploader->cmdSetCount++].vk.cmd, &info );
						  }

					  } ) );
	//__UploadBeginCommandSet( device, 0 );
}

//void R_InitResourceUpload()
//{
// // NRI_ABORT_ON_FAILURE( rsh.nri.coreI.GetCommandQueue( rsh.nri.device, NriCommandQueueType_GRAPHICS, &cmdQueue ) )
// // NriBufferDesc stageBufferDesc = { .size = SizeOfStageBufferByte };
// // NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateBuffer( rsh.nri.device, &stageBufferDesc, &stageBuffer.buffer ) )
//
// // NriResourceGroupDesc resourceGroupDesc = { .buffers = &stageBuffer.buffer, .bufferNum = 1, .memoryLocation = NriMemoryLocation_HOST_UPLOAD };
// // assert( rsh.nri.helperI.CalculateAllocationNumber( rsh.nri.device, &resourceGroupDesc ) == 1 );
// // NRI_ABORT_ON_FAILURE( rsh.nri.helperI.AllocateAndBindMemory( rsh.nri.device, &resourceGroupDesc, &stageBuffer.memory ) );
// // NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateFence( rsh.nri.device, 0, &uploadFence ) );
//
// // for( size_t i = 0; i < Q_ARRAY_COUNT( commandSets ); i++ ) {
// // 	NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateCommandAllocator( cmdQueue, &commandSets[i].allocator ) );
// // 	NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateCommandBuffer( commandSets[i].allocator, &commandSets[i].cmd ) );
// // }
//
// // // we just keep the buffer always mapped
// // stageBuffer.cpuMappedBuffer = rsh.nri.coreI.MapBuffer( stageBuffer.buffer, 0, NRI_WHOLE_SIZE );
// // stageBuffer.tailOffset = 0;
// // stageBuffer.remaningSpace = SizeOfStageBufferByte;
// // activeSet = 0;
//	R_UploadBeginCommandSet( 0 );
//}

void R_ResourceBeginCopyBuffer( struct RIDevice_s *device, struct RIResourceUploader_s *uploader, buffer_upload_desc_t *action )
{
	assert( action->target );
	resource_stage_response_t res = { 0 };
	__R_AllocTemporaryBuffer( uploader, action->numBytes, &res );
	action->vk.stageOffset = res.byteOffset;
	action->vk.stage = res.vk.buffer;
	action->data = res.cpuMapping;
}

void R_ResourceEndCopyBuffer( struct RIDevice_s *device, struct RIResourceUploader_s *uploader, buffer_upload_desc_t *action )
{
	//const NriBufferDesc* bufferDesc = rsh.nri.coreI.GetBufferDesc( action->target);

	//bool seen = false;
	//for(size_t i = 0; i < arrlen(commandSets[activeSet].seenBuffers); i++) {
	//	if(commandSets[activeSet].seenBuffers[i] == action->target) {
	//		seen = true;
	//		break;
	//	}
	//}
	//if(!seen) {
	//	NriBufferBarrierDesc transitionBarriers = {0};
	//	transitionBarriers.buffer = action->target;
	//	transitionBarriers.before = action->before;
	//	transitionBarriers.after = (NriAccessStage){
	//		.access = NriAccessBits_COPY_DESTINATION,
	//		.stages = NriStageBits_COPY
	//	};

	//	if(bufferDesc->usage & NriBufferUsageBits_INDEX_BUFFER) {
	//		transitionBarriers.after.access |= NriAccessBits_INDEX_BUFFER;
	//		transitionBarriers.after.stages |= NriStageBits_INDEX_INPUT;
	//	}
	//	if(bufferDesc->usage & NriBufferUsageBits_VERTEX_BUFFER) {
	//		transitionBarriers.after.access |= NriAccessBits_VERTEX_BUFFER;
	//		transitionBarriers.after.stages |= NriStageBits_VERTEX_SHADER;
	//	}
	//	
	//	NriBarrierGroupDesc barrierGroupDesc = { 0 };
	//	barrierGroupDesc.bufferNum = 1;
	//	barrierGroupDesc.buffers = &transitionBarriers;
	//	rsh.nri.coreI.CmdBarrier( commandSets[activeSet].cmd, &barrierGroupDesc );

	//	NriBufferBarrierDesc postTransition = {
	//		.before = transitionBarriers.after,
	//		.after = action->after,
	//		.buffer = action->target
	//	};
	//	arrput(transitionBuffers, postTransition);
	//	arrput(commandSets[activeSet].seenBuffers, action->target);
	//}

	//rsh.nri.coreI.CmdCopyBuffer( commandSets[activeSet].cmd, action->target, action->byteOffset, action->internal.backing, action->internal.byteOffset, action->numBytes );
	VkBufferCopy copyBuffer = {};
	copyBuffer.size = action->numBytes;
	copyBuffer.dstOffset = action->vk.stageOffset;
	copyBuffer.srcOffset = action->byteOffset;
	vkCmdCopyBuffer(uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].vk.cmd, action->vk.stage, action->vk.target, 1, &copyBuffer);
}

void R_ResourceBeginCopyTexture( struct RIDevice_s *device, struct RIResourceUploader_s *uploader, texture_upload_desc_t *desc )
{
	assert( desc->target );
	const NriDeviceDesc *deviceDesc = rsh.nri.coreI.GetDeviceDesc( rsh.nri.device );

	const uint64_t alignedRowPitch = ALIGN( desc->rowPitch, deviceDesc->uploadBufferTextureRowAlignment );
	const uint64_t alignedSlicePitch = ALIGN( desc->sliceNum * alignedRowPitch, deviceDesc->uploadBufferTextureSliceAlignment );

	desc->alignRowPitch = alignedRowPitch;
	desc->alignSlicePitch = alignedSlicePitch;

	resource_stage_response_t res = { 0 };
	__R_AllocTemporaryBuffer( uploader, action->numBytes, &res );

	// resource_stage_response_t res = {0};
	// R_AllocTemporaryBuffer( &commandSets[activeSet], alignedSlicePitch, &res );

	desc->vk.stageOffset = res.byteOffset;
	desc->vk.stage = res.vk.buffer;
	desc->data = res.cpuMapping;
}

void R_ResourceEndCopyTexture( texture_upload_desc_t *desc )
{
	//const NriTextureDesc *textureDesc = rsh.nri.coreI.GetTextureDesc( desc->target );

	//bool seen = false;
	//for(size_t i = 0; i < arrlen(commandSets[activeSet].seenTextures); i++) {
	//	if(commandSets[activeSet].seenTextures[i] == desc->target) {
	//		seen = true;
	//		break;
	//	}
	//}
	//
	//if(!seen) {
	//	NriTextureBarrierDesc transitionBarriers = { 0 };
	//	transitionBarriers.before = desc->before;
	//	transitionBarriers.texture = desc->target;
	//	transitionBarriers.after = (NriAccessLayoutStage){	
	//		.layout = NriLayout_COPY_DESTINATION, 
	//		.access = NriAccessBits_COPY_DESTINATION, 
	//		.stages = NriStageBits_COPY 
	//	};

	//	NriBarrierGroupDesc barrierGroupDesc = { 0 };
	//	barrierGroupDesc.textureNum = 1;
	//	barrierGroupDesc.textures = &transitionBarriers;
	//	rsh.nri.coreI.CmdBarrier( commandSets[activeSet].cmd, &barrierGroupDesc );

	//	const NriTextureBarrierDesc postTransition = {
	//		.before = transitionBarriers.after ,
	//		.after = desc->after,
	//		.texture = desc->target
	//	};
	//	arrpush(transitionTextures, postTransition );
	//	arrpush(commandSets[activeSet].seenTextures, desc->target);
	//} else {
	//	NriTextureBarrierDesc transitionBarriers = { 0 };
	//	transitionBarriers.before = (NriAccessLayoutStage){	
	//		.layout = NriLayout_COPY_DESTINATION, 
	//		.access = NriAccessBits_COPY_DESTINATION, 
	//		.stages = NriStageBits_COPY 
	//	};
	//	transitionBarriers.texture = desc->target;
	//	transitionBarriers.after = (NriAccessLayoutStage){	
	//		.layout = NriLayout_COPY_DESTINATION, 
	//		.access = NriAccessBits_COPY_DESTINATION, 
	//		.stages = NriStageBits_COPY 
	//	};

	//	NriBarrierGroupDesc barrierGroupDesc = { 0 };
	//	barrierGroupDesc.textureNum = 1;
	//	barrierGroupDesc.textures = &transitionBarriers;
	//	rsh.nri.coreI.CmdBarrier( commandSets[activeSet].cmd, &barrierGroupDesc );
	//}
	//
	//
	const struct RIFormatProps_s formatProps = GetRIFormatProps( desc->format );
	uint32_t rowBlockNum = desc->alignRowPitch / formatProps.stride;
	uint32_t bufferRowLength = rowBlockNum * formatProps.blockWidth;

	uint32_t sliceRowNum = desc->alignSlicePitch / srcDataLayoutDesc.rowPitch;
	uint32_t bufferImageHeight = sliceRowNum * formatProps.blockWidth;

	VkBufferImageCopy region = {};
    region.bufferOffset = desc->vk.stageOffset;
    region.bufferRowLength = bufferRowLength;
    region.bufferImageHeight = bufferImageHeight;
    //region.imageSubresource = VkImageSubresourceLayers{
    //    dst.GetImageAspectFlags(),
    //    dstRegionDesc.mipOffset,
    //    dstRegionDesc.layerOffset,
    //    1,
    //};
    //region.imageOffset = VkOffset3D{
    //    dstRegionDesc.x,
    //    dstRegionDesc.y,
    //    dstRegionDesc.z,
    //};
    //region.imageExtent = VkExtent3D{
    //    (dstRegionDesc.width == WHOLE_SIZE) ? dst.GetSize(0, dstRegionDesc.mipOffset) : dstRegionDesc.width,
    //    (dstRegionDesc.height == WHOLE_SIZE) ? dst.GetSize(1, dstRegionDesc.mipOffset) : dstRegionDesc.height,
    //    (dstRegionDesc.depth == WHOLE_SIZE) ? dst.GetSize(2, dstRegionDesc.mipOffset) : dstRegionDesc.depth,
    //};
    vkCmdCopyBufferToImage(
    	uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].vk.cmd, 
    	desc->vk.stage, desc->vk.target, IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

// void R_ResourceTransitionTexture( NriTexture *texture, NriAccessLayoutStage currentAccessAndLayout )
// {
// 	NriTextureBarrierDesc transitionBarriers = { 0 };
// 	transitionBarriers.before = currentAccessAndLayout;
// 	transitionBarriers.after = ResourceUploadTexturePostAccess;
// 	transitionBarriers.texture = texture;

// 	NriBarrierGroupDesc barrierGroupDesc = { 0 };
// 	barrierGroupDesc.textureNum = 1;
// 	barrierGroupDesc.textures = &transitionBarriers;
// 	rsh.nri.coreI.CmdBarrier( commandSets[activeSet].cmd, &barrierGroupDesc );
// }

void R_ResourceSubmit(struct RIDevice_s* device, struct RIResourceUploader_s* uploader)
{
	// TODO: need to wait
	VK_WrapResult(vkEndCommandBuffer( uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].vk.cmd ));

	VkCommandBufferSubmitInfo cmdSubmitInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
	cmdSubmitInfo.commandBuffer = uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].vk.cmd;

	VkSemaphoreSubmitInfo signalSem = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
	signalSem.stageMask = VK_PIPELINE_STAGE_2_NONE;
	signalSem.value = 1 + uploader->cmdSetCount;
	signalSem.semaphore = uploader->vk.sem;
	VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
	submitInfo.pSignalSemaphoreInfos = &signalSem;
	submitInfo.signalSemaphoreInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmdSubmitInfo;
	submitInfo.commandBufferInfoCount = 1;
	vkQueueSubmit2( uploader->queue->vk.queue, 1, &submitInfo, VK_NULL_HANDLE );
	uploader->cmdSetCount++;

	VK_WrapResult(vkResetCommandPool( device->vk.device, uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].vk.pool, 0 ));
	{
		VkCommandBufferBeginInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_WrapResult( vkBeginCommandBuffer( uploader->cmdSets[uploader->cmdSetCount % RI_RESOURCE_UPLOADER_SET_SIZE].vk.cmd, &info ) );
	}
	//arrsetlen(commandSets[activeSet].seenBuffers, 0);
	//arrsetlen(commandSets[activeSet].seenTextures, 0);

	//R_UploadEndCommandSet( activeSet );
	//activeSet = ( activeSet + 1 ) % NUMBER_COMMAND_SETS;
	//if( syncIndex >= NUMBER_COMMAND_SETS ) {
	//	struct command_set_s *set = &commandSets[activeSet];
	//	rsh.nri.coreI.Wait( uploadFence, 1 + syncIndex - NUMBER_COMMAND_SETS );
	//	rsh.nri.coreI.ResetCommandAllocator( set->allocator );
	//	for( size_t i = 0; i < arrlen( set->temporary ); i++ ) {
	//		rsh.nri.coreI.DestroyBuffer( set->temporary[i].buffer );
	//		rsh.nri.coreI.FreeMemory( set->temporary[i].memory );
	//	}
	//	arrsetlen( set->temporary, 0 );
	//}
	//R_UploadBeginCommandSet( activeSet );
}

void R_FlushTransitionBarrier(struct RIDevice_s* device, struct RIResourceUploader_s* uploader) {
	// size_t numTextureBarrier = 0;
	// NriTextureBarrierDesc textureBarriers[128] = {}; 
	// for(size_t i = 0; i < arrlen(transitionTextures); i++) {

	// 	NriAccessLayoutStage textureShaderAccess = {
	// 		.layout = NriLayout_SHADER_RESOURCE,
	// 		.access = NriAccessBits_SHADER_RESOURCE,
	// 		.stages = NriStageBits_FRAGMENT_SHADER 
	// 	};

	// 	textureBarriers[numTextureBarrier++] = (NriTextureBarrierDesc){
	// 		.texture = transitionTextures[i],
	// 		.before = ResourceUploadTexturePostAccess,
	// 		.after = textureShaderAccess
	// 	};
	// 	if(numTextureBarrier >= Q_ARRAY_COUNT(textureBarriers)) {
	// 		NriBarrierGroupDesc barrierGroupDesc = {0};
	// 		barrierGroupDesc.textureNum = numTextureBarrier;
	// 		barrierGroupDesc.textures = textureBarriers;
	// 		rsh.nri.coreI.CmdBarrier(cmd, &barrierGroupDesc);
	// 		numTextureBarrier = 0;
	// 	}
	// }
	//NriBarrierGroupDesc barrierGroupDesc = {0};
	//barrierGroupDesc.textureNum = arrlen(transitionTextures);
	//barrierGroupDesc.textures = transitionTextures;
	//barrierGroupDesc.bufferNum = arrlen(transitionBuffers);
	//barrierGroupDesc.buffers = transitionBuffers;
	//rsh.nri.coreI.CmdBarrier(cmd, &barrierGroupDesc);
	//arrsetlen(transitionBuffers, 0);
	//arrsetlen(transitionTextures, 0);

	// if(numTextureBarrier >= 0) {
	// }
	//
}
