#include "ui_precompiled.h"
#include "kernel/ui_common.h"
#include "datasources/ui_maps_datasource.h"

#define MAPS_SOURCE "maps"
#define TABLE_NAME  "list"
#define MAP_TITLE   "title"
#define MAP_FILE    "name"

namespace WSWUI
{
MapsDataSource::MapsDataSource() : Rml::Controls::DataSource( MAPS_SOURCE ) {
	getMapsList( mapList );

		// notify the changes
		NotifyRowAdd( TABLE_NAME, 0, mapList.size() );
	}
}

void MapsDataSource::GetRow( Rml::Core::StringList &row, const Rml::Core::String&, int row_index, const Rml::Core::StringList& cols ) {
	if( row_index < 0 || (size_t)row_index > mapList.size() ) {
		return;
	}

	for( Rml::Core::StringList::const_iterator it = cols.begin(); it != cols.end(); ++it ) {
		if( *it == MAP_TITLE ) {
			row.push_back( mapList[row_index].second.empty() ? mapList[row_index].first.c_str() : mapList[row_index].second.c_str() );
		} else if( *it == MAP_FILE ) {
			row.push_back( mapList[row_index].first.c_str() );
		} else {
			row.push_back( "" );
		}
	}

int MapsDataSource::GetNumRows( const Rml::Core::String& ) {
	return mapList.size();
}
}
