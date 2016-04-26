#include "Map.h"

int main()
{
	Map* map = new Map();

	map->readInfo();

	map->start();

	//map->askSource();
	delete(map);
	return 0;
}
