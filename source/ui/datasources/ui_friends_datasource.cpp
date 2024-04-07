#include "ui_precompiled.h"
#include "kernel/ui_common.h"
#include "datasources/ui_friends_datasource.h"
#include "kernel/ui_syscalls.h"
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
			trap::Steam_GetFriend(row_index, name, &steamid, &avatar);

			trap::R_RegisterRawPic(va("friendavatar64-%llu", steamid), 64, 64, avatar, 4);
			printf("name: %s\n", name);

			if(*it == "name") row.push_back(name);
			else if(*it == "cleanname") {
				row.push_back(COM_RemoveColorTokens(name));
			}
			else if(*it == "steamid") row.push_back(va("%llu", steamid) );
			else if(*it == "index") row.push_back(va("%i", row_index));
			else row.push_back("");
		}
	}

	int FriendsDataSource::GetNumRows(const Rocket::Core::String&)
	{
		size_t i = 0;
		for (;;i++) {
  		if (!trap::Steam_GetFriend(i, NULL, NULL, NULL))
    		break;
		}
		return i;
	}
}

