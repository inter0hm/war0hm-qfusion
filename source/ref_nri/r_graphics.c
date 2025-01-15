#include "r_graphics.h"
#include "../gameshared/q_arch.h"
#include "ref_mod.h"

#if ( DEVICE_IMPL_VULKAN )
inline static bool __vk_isExtensionSupported( const char *targetExt, VkExtensionProperties *properties, size_t numExtensions )
{
	for( size_t i = 0; i < numExtensions; i++ ) {
		if( strcmp( properties[i].extensionName, targetExt ) == 0 ) {
			return true;
		}
	}

	return false;
}

VkBool32 VKAPI_PTR __VK_DebugUtilsMessenger( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
											 VkDebugUtilsMessageTypeFlagsEXT messageType,
											 const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
											 void *userData )
{
	switch( messageSeverity ) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			Com_Printf( "KV ERROR: %s", callbackData->pMessage );
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			Com_Printf( "KV WARNING: %s", callbackData->pMessage );
			break;
		default:
			Com_Printf( "KV INFO: %s", callbackData->pMessage );
			break;
	}
	return VK_FALSE;
}
#endif

const char* RIResultToString(enum RIResult_e res) {
	switch(res) {
  	case RI_FAIL: 
  		return "RI_FAIL";
  	case RI_SUCCESS:
  		return "RI_SUCCESS";
  	case RI_INCOMPLETE:
  		return "RI_INCOMPLETE";
	}
	return "RI_UNKNOWN";
}


int InitRIRenderer( const struct RIBackendInit_s *init, struct RIRenderer_s *renderer )
{
	renderer->api = init->api;
	GPU_VULKAN_BLOCK( renderer, {
		volkInitialize();

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = init->applicationName;
		appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.pEngineName = "qfusion";
		appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.apiVersion = VK_API_VERSION_1_3;

		const VkValidationFeatureEnableEXT enabledValidationFeatures[] = { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };

		VkValidationFeaturesEXT validationFeatures = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
		validationFeatures.enabledValidationFeatureCount = Q_ARRAY_COUNT( enabledValidationFeatures );
		validationFeatures.pEnabledValidationFeatures = enabledValidationFeatures;

		VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		instanceCreateInfo.pApplicationInfo = &appInfo;
		const char *enabledLayerNames[8] = {};
		const char *enabledExtensionNames[8] = {};
		instanceCreateInfo.ppEnabledLayerNames = enabledLayerNames;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames;
		instanceCreateInfo.enabledExtensionCount = 0;
		{
			assert( 1 <= Q_ARRAY_COUNT( enabledLayerNames ) );
			uint32_t enumInstanceLayers = 0;
			vkEnumerateInstanceLayerProperties( &enumInstanceLayers, NULL );
			VkLayerProperties *layerProperties = malloc( enumInstanceLayers * sizeof( VkLayerProperties ) );
			vkEnumerateInstanceLayerProperties( &enumInstanceLayers, layerProperties );
			for( size_t i = 0; i < enumInstanceLayers; i++ ) {
				bool useLayer = false;
				useLayer |= ( init->vk.enableValidationLayer && strcmp( layerProperties[i].layerName, "VK_LAYER_KHRONOS_validation" ) == 0 );
				Com_Printf( "Instance Layer: %s(%d): %s", layerProperties[i].layerName, layerProperties[i].specVersion, useLayer ? "ENABLED" : "DISABLED" );
				if( useLayer ) {
					assert( instanceCreateInfo.enabledLayerCount < Q_ARRAY_COUNT( enabledLayerNames ) );
					enabledLayerNames[instanceCreateInfo.enabledLayerCount++] = layerProperties[i].layerName;
				}
			}
			free( layerProperties );
		}
		{
			uint32_t extensionNum = 0;
			vkEnumerateInstanceExtensionProperties( NULL, &extensionNum, NULL );
			VkExtensionProperties *extProperties = malloc( extensionNum * sizeof( VkExtensionProperties ) );
			vkEnumerateInstanceExtensionProperties( NULL, &extensionNum, extProperties );
		
			const bool supportSurfaceExtension = __vk_isExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME, extProperties, extensionNum );
			for( size_t i = 0; i < extensionNum; i++ ) {
				bool useExtension = false;

				if( supportSurfaceExtension ) {
#ifdef VK_USE_PLATFORM_WIN32_KHR
					useExtension |= ( strcmp( extProperties[i].extensionName, VK_KHR_WIN32_SURFACE_EXTENSION_NAME ) == 0 );
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
					useExtension |= ( strcmp( extProperties[i].extensionName, VK_EXT_METAL_SURFACE_EXTENSION_NAME ) == 0 );
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
					useExtension |= ( strcmp( extProperties[i].extensionName, VK_KHR_XLIB_SURFACE_EXTENSION_NAME ) == 0 );
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
					useExtension |= ( strcmp( extProperties[i].extensionName, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME ) == 0 );
#endif
				}

				useExtension |= ( strcmp( extProperties[i].extensionName, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME ) == 0 );
				useExtension |= ( strcmp( extProperties[i].extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME ) == 0 );
				Com_Printf( "Instance Extensions: %s(%d): %s", extProperties[i].extensionName, extProperties[i].specVersion, useExtension ? "ENABLED" : "DISABLED" );
			  if(useExtension) {
					assert( instanceCreateInfo.enabledExtensionCount < Q_ARRAY_COUNT( enabledExtensionNames ) );
					enabledExtensionNames[instanceCreateInfo.enabledExtensionCount++] = extProperties[i].extensionName;
			  }
			}
			free( extProperties );
		}

		if( init->vk.enableValidationLayer ) {
			R_VK_ADD_FEATURE( &instanceCreateInfo, &validationFeatures );
		}

		VkResult result = vkCreateInstance( &instanceCreateInfo, NULL, &renderer->vk.instance);
		if(result != VK_SUCCESS) { 
			Com_Printf("Vulkan failed error - vk: %d", result);
			return RI_FAIL;  
		}

		if( init->vk.enableValidationLayer ) {
			VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
			createInfo.pUserData = renderer;
			createInfo.pfnUserCallback = __VK_DebugUtilsMessenger;

			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
			createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			vkCreateDebugUtilsMessengerEXT( renderer->vk.instance, &createInfo, NULL, &renderer->vk.debugMessageUtils);
		}

	} );
	return RI_SUCCESS; 
}

void ShutdownRIRenderer(struct RIRenderer_s* renderer)
{
	GPU_VULKAN_BLOCK(renderer, { 
		vkDestroyDebugUtilsMessengerEXT(renderer->vk.instance, renderer->vk.debugMessageUtils, NULL);

		vkDestroyInstance(renderer->vk.instance, NULL);
		volkFinalize(); 
	})
}

int InitRIDevice(struct RIDeviceInit_s* init, struct RIDevice_s device) {
	assert(init->physicalAdapter);

	struct RIPhysicalAdapter_s* physicalAdapter = init->physicalAdapter;
	memcpy(&device.adapter, physicalAdapter, sizeof(struct RIPhysicalAdapter_s));
  
  VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

	GPU_VULKAN_BLOCK( renderer, {
		vkCreateDevice(physicalAdapter->vk.physicalDevice, &deviceCreateInfo, NULL, &device.vk.device );

	});

	return RI_SUCCESS;
}

int EnumerateRIAdapters( struct RIRenderer_s *renderer, struct RIPhysicalAdapter_s *adapters, uint32_t *numAdapters )
{
	GPU_VULKAN_BLOCK( renderer, {
		uint32_t deviceGroupNum = 0;
		VkResult result = vkEnumeratePhysicalDeviceGroups( renderer->vk.instance, &deviceGroupNum, NULL );
		if( result != VK_SUCCESS ) {
			Com_Printf( "Vulkan failed error - vk: %d", result );
			return RI_FAIL;
		}

		if( adapters ) {
			VkPhysicalDeviceGroupProperties *physicalDeviceGroupProperties = alloca( sizeof( VkPhysicalDeviceGroupProperties ) * deviceGroupNum );
			result = vkEnumeratePhysicalDeviceGroups( renderer->vk.instance, &deviceGroupNum, physicalDeviceGroupProperties );
			assert( ( *numAdapters ) >= deviceGroupNum );

			for( size_t i = 0; i < deviceGroupNum; i++ ) {
				struct RIPhysicalAdapter_s *physicalAdapter = &adapters[i];

				VkPhysicalDeviceIDProperties deviceIDProperties = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES };
				VkPhysicalDeviceProperties2 properties2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
				R_VK_ADD_FEATURE( &properties2, &deviceIDProperties );

				VkPhysicalDevice physicalDevice = physicalDeviceGroupProperties[i].physicalDevices[0];

				VkPhysicalDeviceMemoryProperties memoryProperties = {};
				vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memoryProperties );
				vkGetPhysicalDeviceProperties2( physicalDevice, &properties2 );

				memset( physicalAdapter, 0, sizeof( struct RIPhysicalAdapter_s ) );
				physicalAdapter->luid = *(uint64_t *)&deviceIDProperties.deviceLUID[0];
				physicalAdapter->deviceId = properties2.properties.deviceID;
				physicalAdapter->vendor = VendorFromID( properties2.properties.vendorID );
				physicalAdapter->vk.physicalDevice = physicalDevice;
				for( uint32_t k = 0; k < memoryProperties.memoryHeapCount; k++ ) {
					if( memoryProperties.memoryHeaps[k].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT )
						physicalAdapter->videoMemorySize += memoryProperties.memoryHeaps[k].size;
					else
						physicalAdapter->systemMemorySize += memoryProperties.memoryHeaps[k].size;
				}
			}
		} else {
			( *numAdapters ) = deviceGroupNum;
		}
	} );

	return RI_SUCCESS;
}

int FreeRIDevice( struct RIDevice_s *dev ) {}
