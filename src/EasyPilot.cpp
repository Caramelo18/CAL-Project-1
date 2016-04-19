#include "Map.h"

int main()
{
	Map* map = new Map();

	map->readInfo();
	delete(map);
	return 0;
}
