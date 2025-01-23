#include "ri_resource.h"
#include "ri_conversion.h"


int InitRITexture( struct RIDevice_s *dev, const struct RITextureDesc_s *desc, struct RITexture_s *tex )
{
	GPU_VULKAN_BLOCK( dev->renderer, ( {
						  tex->height = desc->height;
						  tex->width = desc->width;
						  tex->type = desc->type;
						  tex->sampleNum = desc->sampleNum;
						  tex->mipNum = desc->mipNum;
						  tex->depth = desc->depth;

						  VkImageCreateInfo info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
						  VkImageCreateFlags flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT; // typeless
						  const FormatProps& formatProps = GetFormatProps(textureDesc.format);
						  if (formatProps.blockWidth > 1)
						      flags |= VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT; // format can be used to create a view with an uncompressed format (1 texel covers 1 block)
						  if (desc->layerNum >= 6 && textureDesc.width == textureDesc.height)
						      flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT; // allow cube maps
						  if (desc->type ==  RI_TEXTURE_3D)
						      flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT; // allow 3D demotion to a set of layers // TODO: hook up "VK_EXT_image_2d_view_of_3d"?
						  //if (m_Desc.sampleLocationsTier && formatProps.isDepth)
						  //    flags |= VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT;

						  // info.flags = flags;
						  info.imageType = ri_vk_RITextureTypeToVKImageType( desc->type );
						  //    info.format = ::GetVkFormat(textureDesc.format, true);
						  info.extent.width = desc->width;
						  info.extent.height = Q_MAX( desc->height, 1 );
						  info.extent.depth = Q_MAX( desc->depth, 1 );
						  info.mipLevels = Q_MAX(desc->mipNum, 1);
						  info.arrayLayers = Q_MAX(desc->layerNum, 1);
						  info.samples = Q_MAX(desc->sampleNum, 1);
						  info.tiling = VK_IMAGE_TILING_OPTIMAL;
						  info.usage = ri_vk_RITextureTypeToVKImageType(desc->usageBits);
						  //    info.sharingMode = m_NumActiveFamilyIndices > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE; // TODO: still no DCC on AMD with concurrent?
						  //    info.queueFamilyIndexCount = m_NumActiveFamilyIndices;
						  //    info.pQueueFamilyIndices = m_ActiveQueueFamilyIndices.data();
						  //    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					  } ) );
	return RI_SUCCESS;
}
