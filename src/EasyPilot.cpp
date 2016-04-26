#include "MapBuilder.h"

int main()
{
	Map* map = new Map();

	MapBuilder* builder = new MapBuilder(map);
	delete(builder);

	map->start();

	delete(map);

	return 0;
}
