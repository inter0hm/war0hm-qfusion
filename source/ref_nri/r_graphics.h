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

#define R_VK_UTILITY_INSERT(current, next) { \
  const void* __pNext = (current)->pNext; \
  (current)->pNext = (next); \
  (next)->pNext = __pNext; \
}


enum r_device_api {
  DEVICE_API_UNKNOWN,
  DEVICE_API_VK, 
  DEVICE_API_D3D11, 
  DEVICE_API_D3D12, 
  DEVICE_API_MTL 
};

enum r_graphics_result_e {
  R_GRAPHICS_FAIL = -1,
  R_GRAPHICS_SUCCESS = 0,
  R_GRAPHICS_INCOMPLETE 
};

struct r_GPU_device_s {
  union {
    #if(DEVICE_IMPL_VULKAN)
    #endif
    #if(DEVICE_IMPL_MTL)
    #endif
  };
};

struct r_GPU_physical_devices_s {
};

struct r_device_desc_s {

};

struct r_renderer_s {
  uint8_t api; // r_device_api 
  union {
    #if(DEVICE_IMPL_VULKAN)
      struct {
        VkInstance instance;       
        VkAllocationCallbacks vkAllocationCallback;
      } vk;
    #endif

  };
};


struct r_backend_init_s {
  enum r_device_api api;
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



int initRenderer(const struct r_backend_init_s* api, struct r_renderer_s* renderer);
void shutdownGPUBackend(struct r_renderer_s* renderer);

int enumerateAdapters(struct r_renderer_s* renderer,struct r_GPU_physical_devices_s* adapters, uint32_t* numAdapters);
int initGPUDevice(struct r_device_desc_s* init, struct r_GPU_device_s device);
int freeGPUDevice(struct r_GPU_device_s*  dev);

#endif
