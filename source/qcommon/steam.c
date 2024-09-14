/*
Copyright (C) 2014 Victor Luchits

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "../qcommon/qcommon.h"
#include "../steamshim/src/parent/parent.h"
#include "cvar.h"
#include "steam.h"
#include <string.h>

static const SteamshimEvent* blockOnEvent(SteamshimEventType type){

	while( 1 ) {
		const SteamshimEvent *evt = STEAMSHIM_pump();
		if (!evt) continue;

		if (evt->type == type){
			return evt;
		} else {
			printf("warning: ignoring event %i\n",evt->type);
		}
	}
}
cvar_t *steam_debug;
/*
* Steam_Init
*/
void Steam_Init( void )
{
	 steam_debug = Cvar_Get( "steam_debug", "0", 0);

	 SteamshimOptions opts;
	 opts.debug = steam_debug->integer;
	 opts.runserver = dedicated->integer;
	 opts.runclient = !dedicated->integer;
	int r = STEAMSHIM_init( &opts );
	if( !r ) {
		Com_Printf( "Steam initialization failed.\n" );
		return;
	}

}

/*
* Steam_Shutdown
*/
void Steam_Shutdown( void )
{
	STEAMSHIM_deinit();
}

/*
* Steam_Active
*/
int Steam_Active(){
	return STEAMSHIM_alive();
}

const char *Steam_CommandLine() {
	STEAMSHIM_requestCommandLine();
	const SteamshimEvent *e = blockOnEvent(EVT_CL_COMMANDLINERECIEVED);
	return e->cl_commandlinerecieved;
}
