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

#ifndef _INCL_STEAMSHIM_TYPES_H_
#define _INCL_STEAMSHIM_TYPES_H_

#define STEAM_AVATAR_SIZE (128*128*4)
#define PIPEMESSAGE_MAX (STEAM_AVATAR_SIZE + 64)
#define AUTH_TICKET_MAXSIZE 1024
typedef struct {
  char pTicket[AUTH_TICKET_MAXSIZE];
  long long pcbTicket;
} SteamAuthTicket_t;


#define STEAM_MAX_AVATAR_SIZE ( 128 * 128 * 4 )
#define STEAM_AUTH_TICKET_MAXSIZE 1024

enum steam_avatar_size_e { STEAM_AVATAR_LARGE, STEAM_AVATAR_MED, STEAM_AVATAR_SMALL };

enum steam_cmd_s {
	RPC_BEGIN,
	RPC_PUMP,
	RPC_REQUEST_STEAM_ID,
	RPC_REQUEST_AVATAR,
	RPC_BEGIN_AUTH_SESSION,
	RPC_END_AUTH_SESSION,
	RPC_PERSONA_NAME,

	RPC_REQUEST_LAUNCH_COMMAND,

	RPC_ACTIVATE_OVERLAY,

	RPC_SET_RICH_PRESENCE,
	RPC_REFRESH_INTERNET_SERVERS,

	RPC_UPDATE_SERVERINFO,
	RPC_UPDATE_SERVERINFO_GAME_DATA,
	RPC_UPDATE_SERVERINFO_GAME_DESCRIPTION,
	RPC_UPDATE_SERVERINFO_GAME_TAGS,
	RPC_UPDATE_SERVERINFO_MAP_NAME,
	RPC_UPDATE_SERVERINFO_MOD_DIR,
	RPC_UPDATE_SERVERINFO_PRODUCT,
	RPC_UPDATE_SERVERINFO_REGION,
	RPC_UPDATE_SERVERINFO_SERVERNAME,

	RPC_END,

	EVT_BEGIN = RPC_END,

	EVT_END,
	CMD_LEN
};
#define STEAM_SHIM_COMMON() enum steam_cmd_s cmd;
#define STEAM_RPC_REQ( name ) struct name##_req_s
#define STEAM_RPC_RECV( name ) struct name##_recv_s
#define STEAM_EVT( name ) struct name##_evt_s

#pragma pack( push, 1 )
struct steam_shim_common_s {
	STEAM_SHIM_COMMON()
};

#define STEAM_RPC_SHIM_COMMON() \
	STEAM_SHIM_COMMON()         \
	uint32_t sync;

struct steam_rpc_shim_common_s {
	STEAM_RPC_SHIM_COMMON()
};

struct steam_id_rpc_s {
	STEAM_RPC_SHIM_COMMON()
	uint64_t id;
};

struct buffer_rpc_s {
	STEAM_RPC_SHIM_COMMON()
	uint8_t buf[];
};
#define RPC_BUFFER_SIZE( buf, size ) ( size - sizeof( buf ) )

STEAM_RPC_REQ( server_info )
{
	STEAM_RPC_SHIM_COMMON()
	bool advertise;
	bool dedicated;
	int maxPlayerCount;
	int botPlayerCount;
};

STEAM_RPC_REQ( steam_auth )
{
	STEAM_RPC_SHIM_COMMON()
	uint64_t pcbTicket;
	char ticket[STEAM_AUTH_TICKET_MAXSIZE];
};

STEAM_RPC_REQ( steam_avatar )
{
	STEAM_RPC_SHIM_COMMON()
	uint64_t steamID;
	enum steam_avatar_size_e size;
};

STEAM_RPC_RECV( steam_avatar )
{
	STEAM_RPC_SHIM_COMMON()
	uint32_t width;
	uint32_t height;
	uint8_t buf[];
};

STEAM_RPC_REQ( begin_auth_session )
{
	STEAM_RPC_SHIM_COMMON()
	uint64_t steamID;
	uint64_t cbAuthTicket;
	uint8_t authTicket[STEAM_AUTH_TICKET_MAXSIZE];
};

// enum steam_dialog_overlay {
//     STEAM_USER_PROFILE,
//     STEAM_CHAT,
//     STEAM_JOIN_TRADE,
//     STEAM_ACHIEVEMENTS
// };
//
// STEAM_RPC_REQ( overlay)
//{
//	STEAM_RPC_SHIM_COMMON()
// };

STEAM_RPC_RECV( steam_result )
{
	STEAM_RPC_SHIM_COMMON()
	int result;
};

struct steam_rpc_pkt {
	union {
		struct steam_rpc_shim_common_s common;
		struct steam_auth_req_s steam_auth;
		struct steam_avatar_req_s avatar;
		struct steam_rpc_shim_common_s pump;
		struct begin_auth_session_req_s begin_auth_session;
		struct steam_id_rpc_s end_auth_session;
		struct steam_rpc_shim_common_s launch_command;
		struct buffer_rpc_s rich_presence;

		struct steam_id_rpc_s open_overlay;
		struct steam_id_rpc_s steam_id;
		struct server_info_req_s server_info;
		struct buffer_rpc_s server_game_data;
		struct buffer_rpc_s server_description;
		struct buffer_rpc_s server_tags;
		struct buffer_rpc_s server_map_name;
		struct buffer_rpc_s server_mod_dir;
		struct buffer_rpc_s server_product;
		struct buffer_rpc_s server_region;
		struct buffer_rpc_s server_name;
	};
};

struct steam_evt_s {
	union {
		struct steam_shim_common_s common;
	};
};

#define STEAM_PACKED_RESERVE_SIZE ( 16384 )
struct steam_packet_buf {
	union {
		struct {
			uint32_t size;
			union {
				struct steam_shim_common_s common;
				struct steam_rpc_shim_common_s rpc_common;
				struct steam_rpc_pkt rpc_payload;
			};
		};
		uint8_t buffer[STEAM_PACKED_RESERVE_SIZE];
	};
};

#pragma pack( pop )


#endif
