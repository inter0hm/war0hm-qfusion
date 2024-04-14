#include "../qcommon/qcommon.h"
#include "../qcommon/steam.h"
#include "client.h"
#include "../steamshim/src/parent/parent.h"
#include "../steamshim/src/packet_utils.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>


static const SteamshimEvent* blockOnEvent(SteamshimEventType type){

	while( 1 ) {
		const SteamshimEvent*evt = STEAMSHIM_pump();
		if (!evt) continue;

		if (evt->type == type){
			return evt;
		} else {
			printf("warning: ignoring event %i\n",evt->type);
		}
	}
}

/*
* Steam_RunFrame
*/
void CL_Steam_RunFrame( void )
{
	const SteamshimEvent *evt = STEAMSHIM_pump();
	if( evt ) {
		switch (evt->type){
			case EVT_CL_AVATARRECIEVED: 
				{
          CL_GameModule_CallbackRequestAvatar(evt->cl_avatarrecieved.steamid, evt->cl_avatarrecieved.avatar);
				}
				break;
			case EVT_CL_GAMEJOINREQUESTED:
				{
					uint64_t inviter = evt->cl_gamejoinrequested.steamIDFriend;
					CL_ParseSteamConnectString(evt->cl_gamejoinrequested.connectString);
				}
				break;
			default: break;
		}
	}
}

/*
* Steam_GetAuthSessionTicketBlocking
*/
const SteamAuthTicket_t* Steam_GetAuthSessionTicketBlocking(){
	static SteamAuthTicket_t ticket;

	STEAMSHIM_getAuthSessionTicket();
	const SteamshimEvent *evt = blockOnEvent(EVT_CL_AUTHSESSIONTICKETRECIEVED);

	ticket.pcbTicket = evt->cl_authsessionticketrecieved.pcbTicket;
	memcpy(ticket.pTicket, evt->cl_authsessionticketrecieved.pTicket, AUTH_TICKET_MAXSIZE);

	return &ticket;
}


void Steam_GetPersonaName( char *name, size_t namesize )
{
	if( !namesize ) {
		return;
	}
	STEAMSHIM_getPersonaName();
	const SteamshimEvent *evt = blockOnEvent(EVT_CL_PERSONANAMERECIEVED);
	strncpy(name, evt->cl_personanamerecieved, namesize);
}

void Steam_OpenProfile(uint64_t steamid) { 
	STEAMSHIM_openProfile(steamid);
}

/*
* Steam_RequestAvatar
* size is 0 for 32x32, 1 for 64x64, 2 for 128x128
*/
void Steam_RequestAvatar(uint64_t steamid, int size)
{
	STEAMSHIM_requestAvatar(steamid, size);
}

/*
* Steam_AdvertiseGame
*/
void Steam_AdvertiseGame( const uint8_t *ip, unsigned short port, uint32_t* syncToken)
{
	char rpc_connect_packet[sizeof(struct buffer_rpc_s) + 256];
	struct buffer_rpc_s* req = (struct buffer_rpc_s*)rpc_connect_packet;
	req->cmd = RPC_SET_RICH_PRESENCE; 
	char connectstr[64] = {0};

	if (port) {
		snprintf( connectstr,sizeof connectstr, "%d.%d.%d.%d:%i",ip[0],ip[1],ip[2],ip[3],port);
	} 	
	const char* pairs[] = {
		"connect",
		connectstr
	};
	const size_t bufferLen = pack_cstr_null_terminated( (char *)req->buf, 256, pairs, 2 );
	STEAMSHIM_sendRPC(req, bufferLen + sizeof(struct buffer_rpc_s), NULL, NULL, syncToken);
}
