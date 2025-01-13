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

VkBool32 VKAPI_PTR __VK_DebugUtilsMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
}
#endif

int initRenderer( const struct r_backend_init_s *init, struct r_renderer_s *renderer )
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
		
		VkResult result = vkCreateInstance( &instanceCreateInfo, &renderer->vk.vkAllocationCallback, &renderer->vk.instance );
		if(result != VK_SUCCESS) { 
			Com_Printf("Vulkan failed error - vk: %d", result);
			return R_GRAPHICS_FAIL;  
		}

		if( init->vk.enableValidationLayer ) {
			R_VK_UTILITY_INSERT( &instanceCreateInfo, &validationFeatures );
			VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
			createInfo.pUserData = renderer;
			createInfo.pfnUserCallback = __VK_DebugUtilsMessenger;

			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
			createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

			vkCreateDebugUtilsMessengerEXT( renderer->vk.instance, &createInfo, &renderer->vk.vkAllocationCallback, NULL );
		}

		// Com_Printf ("Creating VkInstance with %ti enabled instance layers:", arrlen(layerTemp));
		// for (int i = 0; i < arrlen(layerTemp); i++)
		//     LOGF(eINFO, "\tLayer %i: %s", i, layerTemp[i]);

		// renderer->vk.instance =
	} );
}

void shutdownGPUBackend(struct r_renderer_s* renderer)
{
	GPU_VULKAN_BLOCK(renderer, { volkFinalize(); } )
}

int initGPUDevice( struct r_device_desc_s *init, struct r_GPU_device_s device ) {}

int enumerateAdapters(struct r_renderer_s* renderer, struct r_GPU_physical_devices_s* adapters, uint32_t* numAdapters) {
  GPU_VULKAN_BLOCK(renderer ,{

    // Create instance
    VkApplicationInfo applicationInfo = {};
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    
    if(adapters == NULL) {

    } else {


    }

    if (instance)
        vkDestroyInstance(instance, NULL);

  });

  return -1;
}

int freeGPUDevice( struct r_GPU_device_s *dev ) {}
