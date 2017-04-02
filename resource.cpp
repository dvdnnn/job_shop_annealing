#include "resource.h"

using namespace capacity_planning;

resource::resource()
{
}

resource::resource(resource * const resource)
	: _id(resource->_id)
{
}

resource::resource(int id)
	: _id(id)
{
}


resource::~resource()
{
	clear();
}

void resource::clear()
{
	/*for (auto it = _units.begin(); it != _units.end(); ++it)
	{
		delete *it;
	}*/
	units.clear();
}