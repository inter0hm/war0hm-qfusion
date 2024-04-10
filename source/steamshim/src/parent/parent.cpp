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

#include <cstdint>
#include <cstring>
#include <stdio.h>
#define DEBUGPIPE 1
#include "parent.h"
#include "../steamshim.h"
#include "../steamshim_private.h"
#include "../os.h"
#include "../steamshim_types.h"

int GArgc = 0;
char **GArgv = NULL;


static size_t SyncToken;
static size_t RecievedSyncToken; // the position of the recieved token
struct steam_rpc_async_s {
    uint32_t token;
    void* self;
    STEAMSHIM_rpc_handle handle;
};

#define NUM_RPC_ASYNC_HANDLE 2048
struct steam_rpc_async_s handles[NUM_RPC_ASYNC_HANDLE];

static SteamshimEvent* ProcessEvent(){
    static SteamshimEvent event;
    // make sure this is static, since it needs to persist between pumps
    static PipeBuffer buf;

    if (!buf.Recieve())
        return NULL;

    if (!buf.hasmsg)
        return NULL;

    volatile unsigned int msglen =buf.ReadInt();


    char type = buf.ReadByte();
    event.type = (SteamshimEventType)type;

    switch (type){
        case EVT_CL_STEAMIDRECIEVED:
            {
                event.cl_steamidrecieved = buf.ReadLong();
            }
            break;
        case EVT_CL_PERSONANAMERECIEVED:
            {
                char *string = buf.ReadString();
                event.cl_personanamerecieved = string;
            }
            break;
        case EVT_CL_AUTHSESSIONTICKETRECIEVED:
            {
                long long pcbTicket = buf.ReadLong();
                event.cl_authsessionticketrecieved.pcbTicket = pcbTicket;
                void *ticket = buf.ReadData(AUTH_TICKET_MAXSIZE);
                memcpy(event.cl_authsessionticketrecieved.pTicket, ticket, AUTH_TICKET_MAXSIZE);
            }
            break;
        case EVT_SV_AUTHSESSIONVALIDATED:
            {
                int result = buf.ReadInt();
                event.sv_authsessionvalidated = result;
            }
            break;
        case EVT_CL_AVATARRECIEVED:
            {
                event.cl_avatarrecieved.steamid = buf.ReadLong();
                event.cl_avatarrecieved.avatar = (uint8_t*) buf.ReadData(STEAM_AVATAR_SIZE);
            }
            break;
        case EVT_CL_GAMEJOINREQUESTED:
            {
                event.cl_gamejoinrequested.steamIDFriend = buf.ReadLong();
                char *string = (char*)buf.ReadString();
                event.cl_gamejoinrequested.connectString = string;
            }
            break;
        case EVT_CL_COMMANDLINERECIEVED:
            {
                char *string = (char*)buf.ReadString();
                event.cl_commandlinerecieved = string;
            }
            break;
    }

    return &event;
}

static bool setEnvironmentVars(PipeType pipeChildRead, PipeType pipeChildWrite)
{
    char buf[64];
    snprintf(buf, sizeof (buf), "%llu", (unsigned long long) pipeChildRead);
    if (!setEnvVar("STEAMSHIM_READHANDLE", buf))
        return false;

    snprintf(buf, sizeof (buf), "%llu", (unsigned long long) pipeChildWrite);
    if (!setEnvVar("STEAMSHIM_WRITEHANDLE", buf))
        return false;



    return true;
} 

extern "C" {
  int STEAMSHIM_init(SteamshimOptions *options)
  {
    debug = options->debug;


    PipeType pipeParentRead = NULLPIPE;
    PipeType pipeParentWrite = NULLPIPE;
    PipeType pipeChildRead = NULLPIPE;
    PipeType pipeChildWrite = NULLPIPE;
    ProcessType childPid;

    if (options->runclient)
        setEnvVar("STEAMSHIM_RUNCLIENT", "1");
    if (options->runserver)
        setEnvVar("STEAMSHIM_RUNSERVER", "1");


    if (!createPipes(&pipeParentRead, &pipeParentWrite, &pipeChildRead, &pipeChildWrite)){
        printf("steamshim: Failed to create application pipes\n");
        return 0;
    }
    else if (!setEnvironmentVars(pipeChildRead, pipeChildWrite)){
        printf("steamshim: Failed to set environment variables\n");
        return 0;
    }
    else if (!launchChild(&childPid))
    {
        printf("steamshim: Failed to launch application\n");
        return 0;
    }

    GPipeRead = pipeParentRead;
    GPipeWrite = pipeParentWrite;

    char status;
     
    readPipe(GPipeRead, &status, sizeof status);

    if (!status){
        closePipe(GPipeRead);
        closePipe(GPipeWrite);

        GPipeWrite = GPipeRead = pipeChildRead = pipeChildWrite = NULLPIPE;
        return 0;
    }

    dbgprintf("Parent init start.\n");

    // Close the ends of the pipes that the child will use; we don't need them.
    closePipe(pipeChildRead);
    closePipe(pipeChildWrite);

    pipeChildRead = pipeChildWrite = NULLPIPE;

#ifndef _WIN32
      signal(SIGPIPE, SIG_IGN);
#endif

      dbgprintf("Child init success!\n");
      return 1;
  } 

  void STEAMSHIM_deinit(void)
  {
      dbgprintf("Child deinit.\n");
      if (GPipeWrite != NULLPIPE)
      {
          closePipe(GPipeWrite);
      } 

      if (GPipeRead != NULLPIPE)
          closePipe(GPipeRead);

      GPipeRead = GPipeWrite = NULLPIPE;

#ifndef _WIN32
      signal(SIGPIPE, SIG_DFL);
#endif
  } 

  static inline int isAlive(void)
  {
      return ((GPipeRead != NULLPIPE) && (GPipeWrite != NULLPIPE));
  } 

  static inline int isDead(void)
  {
      return !isAlive();
  }

  int STEAMSHIM_alive(void)
  {
      return isAlive();
  } 

  const SteamshimEvent *STEAMSHIM_pump(void)
  {
    Write1ByteMessage(CMD_PUMP);
    return ProcessEvent();
  } 

//  static void recv_handler(void* self, struct steam_rpc_recieve_s* recv) {
//    switch(recv->common.cmd) {
//        case RPC_REQUEST_STEAM_ID:
//            break;
//    }
//  }

 // void sample() {
 //   struct steam_rpc_req_s req;
 //   req.common.cmd = RPC_REQUEST_STEAM_ID;
 //   req.steam_req.id = 1034;
 //   STEAMSHIM_sendRPC(&req, sizeof(req.steam_req), NULL, recv_handler);
 // }

  int STEAMSHIM_sendRPC(struct steam_rpc_req_s* req, uint32_t size, void* self, STEAMSHIM_rpc_handle rpc, uint32_t* sync) {
    uint32_t syncIndex = ++SyncToken;
    struct steam_rpc_async_s* handle = handles + (syncIndex % NUM_RPC_ASYNC_HANDLE);

    if(handle->token != 0 && handle->token < RecievedSyncToken) {
        SyncToken--;
        return -1;
    }

    handle->token = syncIndex;
    handle->self = self;
    handle->handle = rpc;
    writePipe(GPipeWrite, &size, sizeof(uint32_t));
    writePipe(GPipeWrite, (uint8_t*)req, size);
    return 0;
  }

  int STEAMSHIM_waitRPC(uint32_t syncIndex) {
    return 0;
  }

  void STEAMSHIM_getSteamID()
  {
	  Write1ByteMessage(CMD_CL_REQUESTSTEAMID);
  }

  void STEAMSHIM_getPersonaName(){
      Write1ByteMessage(CMD_CL_REQUESTPERSONANAME);
  }

  void STEAMSHIM_getAuthSessionTicket(){
      Write1ByteMessage(CMD_CL_REQUESTAUTHSESSIONTICKET);
  }

  void STEAMSHIM_beginAuthSession(uint64_t steamid, SteamAuthTicket_t* ticket){
      PipeBuffer buf;
      buf.WriteByte(CMD_SV_BEGINAUTHSESSION);
      buf.WriteLong(steamid);
      buf.WriteLong(ticket->pcbTicket);
      buf.WriteData(ticket->pTicket, AUTH_TICKET_MAXSIZE);
      buf.Transmit();
  }

  void STEAMSHIM_endAuthSession(uint64_t steamid){
    PipeBuffer buf;
    buf.WriteByte(CMD_SV_ENDAUTHSESSION);
    buf.WriteLong(steamid);
    buf.Transmit();
  }

  void STEAMSHIM_setRichPresence(int num, const char** key, const char** val){
      PipeBuffer buf;
      buf.WriteByte(CMD_CL_SETRICHPRESENCE);
      buf.WriteInt(num);
      for (int i=0; i < num;i++){
          buf.WriteString(key[i]);
          buf.WriteString(val[i]);
      }
      buf.Transmit();
  }

  void STEAMSHIM_requestAvatar(uint64_t steamid, SteamAvatarSize size){
    PipeBuffer buf;
    buf.WriteByte(CMD_CL_REQUESTAVATAR);
    buf.WriteLong(steamid);
    buf.WriteInt(size);
    buf.Transmit();
  }

  void STEAMSHIM_openProfile(uint64_t steamid) {
      PipeBuffer buf;
      buf.WriteByte(CMD_CL_OPENPROFILE);
      buf.WriteLong(steamid);
      buf.Transmit();
  }

  void STEAMSHIM_requestCommandLine(){
    Write1ByteMessage(CMD_CL_REQUESTCOMMANDLINE);
  }

  void STEAMSHIM_updateServerInfo(ServerInfo *info){
    PipeBuffer buf;
    buf.WriteByte(CMD_SV_UPDATESERVERINFO);
    buf.WriteByte(info->advertise);
    buf.WriteInt(info->botplayercount);
    buf.WriteByte(info->dedicatedserver);
    buf.WriteString(info->gamedata);
    buf.WriteString(info->gamedescription);
    buf.WriteString(info->gametags);
    buf.WriteInt(info->heartbeatinterval);
    buf.WriteString(info->mapname);
    buf.WriteInt(info->maxplayercount);
    buf.WriteString(info->moddir);
    buf.WriteByte(info->passwordprotected);
    buf.WriteString(info->product);
    buf.WriteString(info->region);
    buf.WriteString(info->servername);
    buf.Transmit();
  }
}
