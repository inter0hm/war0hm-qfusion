
#pragma once

#include <time.h>
#include <stddef.h>
#include <Rocket/Controls/DataFormatter.h>

namespace WSWUI
{

class ShaderFormatter : public Rocket::Controls::DataFormatter
{
public:
	ShaderFormatter() : Rocket::Controls::DataFormatter("shader") {}


	// emits an <img> with the shader name as the src
	void FormatData( Rocket::Core::String& formatted_data, const Rocket::Core::StringList& raw_data )
	{
		formatted_data = "";

		for( Rocket::Core::StringList::const_iterator it = raw_data.begin(); it != raw_data.end(); it++ ) {
			const char *s;

			s = it->CString();

			formatted_data += Rocket::Core::String( 128,
							"<img src=\"/shader_raw_/%s\" class=\"shaderimg\"></img>",
							s
						);
		}
	}

};

}
