#include "ui_precompiled.h"
#include "kernel/ui_common.h"
#include "datasources/ui_friends_datasource.h"
#include "kernel/ui_syscalls.h"
#include "../../qcommon/qcommon.h"
#include <cstdint>

#define TABLE_NAME "blockedplayers"

namespace WSWUI
{
    FriendsDataSource::FriendsDataSource() : Rocket::Controls::DataSource( "steamfriends" )
	{

	}
	
	void FriendsDataSource::GetRow( Rocket::Core::StringList &row, const Rocket::Core::String&, int row_index, const Rocket::Core::StringList& cols )
	{
		for( Rocket::Core::StringList::const_iterator it = cols.begin(); it != cols.end(); ++it )
		{
			uint64_t steamid;

			char name[MAX_NAME_BYTES+1] = {0};
			uint8_t *avatar;
			int personastate;
			int playingGameID;
			trap::Steam_GetFriend(row_index, name, &steamid, &avatar, &personastate, &playingGameID);

			char *avatarname = va("friendavatar64-%llu", steamid);

			trap::R_RegisterRawPic(avatarname, 64, 64, avatar, 4);

			// TODO: this is a nasty hack to be able to affect the resultant column with css. surely there's a better way than hacking colorcode formatter?
			if(*it == "name") row.push_back(va("^7%s",name));

			if(*it =="avatar") row.push_back(avatarname);
			if(*it == "cleanname") row.push_back(COM_RemoveColorTokens(name));
			if(*it == "steamid") row.push_back(va("%llu", steamid) );
			if(*it == "state") {
				if (personastate == 1 || personastate == 5 || personastate == 6) {
					if (playingGameID) {
						if (playingGameID == APP_STEAMID) {
							row.push_back("Playing warfork (attempt to parse the connect string)");
						} else {
							row.push_back("In-Game");
						}
					} else {
						row.push_back("Online");
					}
				} else {
					row.push_back("Offline");
				}
			}
			if(*it == "index") row.push_back(va("%i", row_index));
		}
	}

	int FriendsDataSource::GetNumRows(const Rocket::Core::String&)
	{
		size_t i = 0;
		for (;;i++) {
  		if (!trap::Steam_GetFriend(i, NULL, NULL, NULL, NULL, NULL))
    		break;
		}
		return i;
	}
}

