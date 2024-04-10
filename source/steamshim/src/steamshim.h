/*
Copyright (C) 2023 coolelectronics

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include "steamshim_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUTH_TICKET_MAXSIZE 1024

enum shim_cmd_s {
  RPC_BEGIN, 
  RPC_REQUEST_STEAM_ID,

  RPC_END,
  EVT_BEGIN = RPC_END,


  EVT_END
};

typedef void (*STEAMSHIM_rpc_handle)(void* self, struct steam_rpc_recieve_s* rec );

#define STEAM_SHIM_COMMON() \
  enum shim_cmd_s cmd; 

#define STEAM_RPC_SHIM_COMMON() \
  STEAM_SHIM_COMMON()  \
  uint32_t sync;

#define STEAM_RPC_REQ(name) struct name ## _req_s
#define STEAM_RPC_RECV(name) struct name ## _recv_s 

#pragma pack(push, 1)
struct steam_shim_common_s {
  STEAM_RPC_SHIM_COMMON()
};

struct steam_rpc_shim_common_s {
  STEAM_RPC_SHIM_COMMON()
};

STEAM_RPC_REQ(steam_id) {
  STEAM_RPC_SHIM_COMMON()
  uint64_t id;
};

STEAM_RPC_RECV(steam_id) {
  STEAM_RPC_SHIM_COMMON()
};

STEAM_RPC_REQ(persona_name) {
  STEAM_RPC_SHIM_COMMON()
};

STEAM_RPC_RECV(persona_name) {
  STEAM_RPC_SHIM_COMMON()
};

STEAM_RPC_REQ(set_rich_presence) {
  STEAM_RPC_SHIM_COMMON()
};

STEAM_RPC_RECV(set_rich_presence) {
  STEAM_RPC_SHIM_COMMON()
};

struct steam_rpc_req_s {
  union {
    struct steam_rpc_shim_common_s common;
    steam_id_req_s steam_req;
    persona_name_req_s persona_req;
  };
};

struct steam_rpc_recieve_s {
  union {
    struct steam_rpc_shim_common_s common;
  };
};

struct steam_packet_buffer {
    union {
        struct steam_shim_common_s common; 
        struct steam_rpc_shim_common_s rpc_common;
        struct steam_rpc_req_s rpc_req;
    };
};

#pragma pack(pop) 

typedef enum eventtype
{
    
    EVT_CL_STEAMIDRECIEVED,
    EVT_CL_PERSONANAMERECIEVED,
    EVT_CL_AUTHSESSIONTICKETRECIEVED,
    EVT_CL_AVATARRECIEVED,
    EVT_CL_COMMANDLINERECIEVED,

    EVT_CL_GAMEJOINREQUESTED,
    EVT_CL_SERVERRECIEVED,

    EVT_SV_AUTHSESSIONVALIDATED,
} SteamshimEventType;

typedef uint64_t Event_cl_steamidrecieved_t;
typedef char *Event_cl_personanamerecieved_t;
typedef struct Event_cl_authsessionticketrecieved {
    char pTicket[AUTH_TICKET_MAXSIZE];
    long pcbTicket;
} Event_cl_authsessionticketrecieved_t;
typedef struct Event_cl_avatarrecieved {
    uint8_t *avatar;
    uint64_t steamid;
} Event_cl_avatarrecieved_t;
typedef char *Event_cl_commandlinerecieved_t;
typedef struct Event_cl_gamejoinrequested {
    uint64_t steamIDFriend;
    char *connectString;
} Event_cl_gamejoinrequested_t;
typedef struct Event_cl_serverrecieved {
    char *serverName;
} Event_cl_serverrecieved_t;
typedef int Event_sv_authsessionvalidated_t;


typedef struct SteamshimEvent {
    SteamshimEventType type;
    union {
        Event_cl_steamidrecieved_t cl_steamidrecieved;
        Event_cl_personanamerecieved_t cl_personanamerecieved;
        Event_cl_authsessionticketrecieved_t cl_authsessionticketrecieved;
        Event_cl_avatarrecieved_t cl_avatarrecieved;
        Event_cl_commandlinerecieved_t cl_commandlinerecieved;
        Event_cl_gamejoinrequested_t cl_gamejoinrequested;
        Event_cl_serverrecieved_t cl_serverrecieved;
        Event_sv_authsessionvalidated_t sv_authsessionvalidated;
    };
} SteamshimEvent;

typedef enum {
    AVATAR_SMALL,
    AVATAR_MEDIUM,
    AVATAR_LARGE,
} SteamAvatarSize;

#ifdef __cplusplus
}
#endif

