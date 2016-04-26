#ifndef MAPBUILDER_H_
#define MAPBUILDER_H_

#include "Map.h"

constexpr double pi = acos(-1);

class MapBuilder {
private:
	Map* map;
	void readNodes();
	void readRoads();
	void readSubRoads();
	void readPOI();
	void fillGraph();
public:
	MapBuilder(Map* map);
};

#endif /* MAPBUILDER_H_ */
