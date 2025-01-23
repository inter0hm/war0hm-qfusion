#ifndef RI_CONVERSION_H
#define RI_CONVERSION_H

#include "ri_types.h"

static inline enum RIVendor_e VendorFromID(uint32_t vendorID) {
    switch (vendorID) {
		case 0x10DE:
			return RI_NVIDIA;
		case 0x1002:
			return RI_AMD;
		case 0x8086:
			return RI_INTEL;
	}
	return RI_UNKNOWN;
}

#if DEVICE_IMPL_VULKAN
static inline VkImageType ri_vk_RITextureTypeToVKImageType( enum RITextureType_e e )
{
	switch( e ) {
		case RI_TEXTURE_1D:
			return VK_IMAGE_TYPE_1D;
		case RI_TEXTURE_2D:
			return VK_IMAGE_TYPE_2D;
		case RI_TEXTURE_3D:
			return VK_IMAGE_TYPE_3D;
	}
	// this shouldn't happen
	assert( false );
	return VK_IMAGE_TYPE_MAX_ENUM;
}

static inline VkImageType ri_vk_RIUsageBitsToVkImageUsageFlags( enum RIUsageBits_e e )
{
	VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	if( e & RI_USAGE_SHADER_RESOURCE )
		flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

	if( e & RI_USAGE_SHADER_RESOURCE_STORAGE )
		flags |= VK_IMAGE_USAGE_STORAGE_BIT;

	if( e & RI_USAGE_COLOR_ATTACHMENT )
		flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if( e & RI_USAGE_DEPTH_STENCIL_ATTACHMENT )
		flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	if( e & RI_USAGE_SHADING_RATE )
		flags |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

	return flags;
}
#endif

#endif
