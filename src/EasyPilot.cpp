#include "Map.h"

int main()
{
	Map* map = new Map();

	map->readInfo();

	map->askSource();
	delete(map);
	return 0;
}
