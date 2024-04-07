#pragma once
#include <Rocket/Controls/DataSource.h>

namespace WSWUI
{

class FriendsDataSource :
	public Rocket::Controls::DataSource
{
public:
	FriendsDataSource( );

	virtual void GetRow( Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns );
	virtual int GetNumRows( const Rocket::Core::String& table );

};

}
