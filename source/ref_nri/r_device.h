#ifndef R_DEVICE_IMP_H
#define R_DEVICE_IMP_H


#include "volk.h"

#define DEVICE_SUPPORT_VULKAN 1



enum r_device_api {
  DEVICE_VK,
  DEVICE_DX11,
  DEVICE_MTL 
};
#if(DEVICE_SUPPORT_VULKAN)
  #define GPU_VULKAN_BLOCK(device, block) if(device->api == DEVICE_VK) { block }
#else
  #define GPU_VULKAN_BLOCK(device, block) 
#endif



struct r_device_s {
  uint8_t api;
  union {
    #ifdef DEVICE_SUPPORT_VULKAN
    #endif
  };
};

void initDevice(struct r_device_s* device);

static inline void test(struct r_device_s* dev) {
  GPU_VULKAN_BLOCK(dev, {
    int i= 0;
    if(i == 0) {

    }
  })
}
#endif
