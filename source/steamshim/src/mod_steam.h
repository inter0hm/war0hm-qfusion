#ifndef _STEAM_MODULE_H
#define _STEAM_MODULE_H

#include "./steamshim_types.h"

typedef void ( *STEAMSHIM_rpc_handle )( void *self, struct steam_rpc_pkt *rec );

#define DECLARE_TYPEDEF_METHOD( ret, name, ... ) \
	typedef ret ( *name##Fn )( __VA_ARGS__ );    \
	ret name( __VA_ARGS__ );

DECLARE_TYPEDEF_METHOD( int, STEAMSHIM_dispatch );
DECLARE_TYPEDEF_METHOD( int, STEAMSHIM_sendRPC, void *req, uint32_t size, void *self, STEAMSHIM_rpc_handle rpc, uint32_t *syncIndex );
DECLARE_TYPEDEF_METHOD( int, STEAMSHIM_waitDispatchSync, uint32_t syncIndex ); // wait on the dispatch loop

#undef DECLARE_TYPEDEF_METHOD

struct steam_import_s {
	STEAMSHIM_dispatchFn STEAMSHIM_dispatch;
	STEAMSHIM_sendRPCFn STEAMSHIM_sendRPC;
	STEAMSHIM_waitDispatchSyncFn STEAMSHIM_waitDispatchSync;
};
#define DECLARE_STEAM_STRUCT() { \
	STEAMSHIM_dispatch, \
	STEAMSHIM_sendRPC, \
	STEAMSHIM_waitDispatchSync \
};

#if MEM_DEFINE_INTERFACE_IMPL
static struct steam_import_s  steam_import;
static inline void Q_ImportSteamModule(const struct steam_imsteam_import_s* imp) {
	steam_import = *imp;
}



#endif
#endif
