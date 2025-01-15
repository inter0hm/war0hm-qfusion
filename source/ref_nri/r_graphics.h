#ifndef R_GRAPHICS_H
#define R_GRAPHICS_H

#include "../gameshared/q_arch.h"

#define DEVICE_SUPPORT_VULKAN

#ifdef DEVICE_SUPPORT_VULKAN
#include "volk.h"
#define DEVICE_IMPL_VULKAN 1
#else
#define DEVICE_IMPL_VULKAN 0
#endif

#ifdef DEVICE_SUPPORT_MTL
#define DEVICE_IMPL_MTL 1
#else
#define DEVICE_IMPL_MTL 0
#endif

#ifdef DEVICE_SUPPORT_D3D11
#define DEVICE_IMPL_D3D11 1
#else
#define DEVICE_IMPL_D3D11 0
#endif

#ifdef DEVICE_SUPPORT_D3D12
#define DEVICE_IMPL_D3D12 1
#else
#define DEVICE_IMPL_D3D12 0
#endif

#if ( ( DEVICE_IMPL_D3D12 + DEVICE_IMPL_D3D11 + DEVICE_IMPL_MTL + DEVICE_IMPL_VULKAN ) > 1 )
#define DEVICE_IMPL_MUTLI 1
#else
#define DEVICE_IMPL_MUTLI 0
#endif


#if(DEVICE_IMPL_MUTLI)

#if(DEVICE_IMPL_VULKAN)
  #define GPU_VULKAN_BLOCK(backend, block) if(backend->api == DEVICE_API_VK) { block }
#else
  #define GPU_VULKAN_BLOCK(backend, block) 
#endif

#if(DEVICE_IMPL_D3D11 )
  #define GPU_D3D11_BLOCK(backend,  block) if(backend->api == DEVICE_API_D3D11) { block }
#else
  #define GPU_D3D11_BLOCK(backend,  block) 
#endif

#if(DEVICE_IMPL_D3D12 )
  #define GPU_D3D12_BLOCK(backend,  block) if(backend->api == DEVICE_API_D3D12) { block }
#else
  #define GPU_D3D12_BLOCK(backend,  block) 
#endif

#if(DEVICE_IMPL_MTL )
  #define GPU_MTL_BLOCK(backend, block) if(backend->api == DEVICE_API_MTL) { block }
#else
  #define GPU_MTL_BLOCK(backend, block) 
#endif

#else

#if(DEVICE_IMPL_VULKAN)
  #define GPU_VULKAN_BLOCK(backend, block)  { block }
#else
  #define GPU_VULKAN_BLOCK(backend, block) 
#endif

#if(DEVICE_IMPL_D3D11 )
  #define GPU_D3D11_BLOCK(backend, block)  { block }
#else
  #define GPU_D3D11_BLOCK(backend, device, block) 
#endif

#if(DEVICE_IMPL_D3D12 )
  #define GPU_D3D12_BLOCK(backend, block) { block }
#else
  #define GPU_D3D12_BLOCK(backend, device, block) 
#endif

#if(DEVICE_IMPL_MTL )
  #define GPU_MTL_BLOCK(backend, block) { block }
#else
  #define GPU_MTL_BLOCK(backend, device, block) 
#endif

#endif

#define R_VK_ADD_FEATURE(current, next) { \
  void* __pNext = (current)->pNext; \
  (current)->pNext = (next); \
  (next)->pNext = __pNext; \
}

#define RI_DEVICE_MAX_QUEUE 4
#define RI_QUEUE_RENDERER_BIT  0x1
#define RI_QUEUE_TRANSFER_BIT  0x2
#define RI_QUEUE_BINDLESS_BIT  0x4

enum RIDeviceAPI_e {
  RI_DEVICE_API_UNKNOWN,
  RI_DEVICE_API_VK, 
  RI_DEVICE_API_D3D11, 
  RI_DEVICE_API_D3D12, 
  RI_DEVICE_API_MTL 
};

enum RIResult_e {
  RI_FAIL = -1,
  RI_SUCCESS = 0,
  RI_INCOMPLETE 
};

enum RIVendor_e {
    RI_UNKNOWN,
    RI_NVIDIA,
    RI_AMD,
    RI_INTEL
};

const char* RIResultToString(enum RIResult_e res);

struct RICmdQueue_s {
  struct RIRenderer_s* renderer;
  uint8_t features;
  union {
    #if(DEVICE_IMPL_VULKAN)
      struct {
        VkQueue queue;
      } vk;
    #endif
  };

};





struct RIRenderer_s {
  uint8_t api; // RIDeviceAPI_e  
  union {
    #if(DEVICE_IMPL_VULKAN)
      struct {
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessageUtils;
      } vk;
    #endif
  };
};


struct RIBackendInit_s {
  uint8_t api; // RIDeviceAPI_e 
  const char* applicationName;
  union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
      bool enableValidationLayer;
      size_t numFilterLayers;
      const char* filterLayers[]; // filter layers for the renderer 
    } vk;
    #endif
    #if(DEVICE_IMPL_MTL)
    #endif
  };
};


struct RIPhysicalAdapter_s {
	char name[256];
	uint64_t luid;
	uint64_t videoMemorySize;
	uint64_t systemMemorySize;
	uint32_t deviceId;
	uint8_t vendor; // RIVendor_e
	union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
      VkPhysicalDevice physicalDevice; 
    } vk; 
    #endif
    #if(DEVICE_IMPL_MTL)
    #endif
  };
};

struct RIDevice_s {
  struct RIRenderer_s* renderer;
  struct RIPhysicalAdapter_s adapter;

  uint8_t numQueues;
  struct RICmdQueue_s queues[RI_DEVICE_MAX_QUEUE];
  union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
      VkDevice device; 
    } vk; 
    #endif
    #if(DEVICE_IMPL_MTL)
    #endif
  };
};


struct RIDeviceInit_s {
  struct RIPhysicalAdapter_s* physicalAdapter;
};

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


int InitRIRenderer(const struct RIBackendInit_s* api, struct RIRenderer_s* renderer);
void ShutdownRIRenderer(struct RIRenderer_s* renderer);

int EnumerateRIAdapters(struct RIRenderer_s* renderer,struct RIPhysicalAdapter_s* adapters, uint32_t* numAdapters);
int InitRIDevice(struct RIDeviceInit_s* init, struct RIDevice_s device);
int FreeRIDevice(struct RIDevice_s*  dev);

#endif
