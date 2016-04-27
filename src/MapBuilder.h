#ifndef MAPBUILDER_H_
#define MAPBUILDER_H_

#include "Map.h"

constexpr double pi = acos(-1);

class MapBuilder {
private:
	Map* map;

	/**
	 * Reads the nodes from the file "nodes.txt"
	 */
	void readNodes();

	/**
	 * Reads the roads from the file "roads.txt"
	 */
	void readRoads();

	/**
	 * Reads the connections between nodes from the file "subroads.txt"
	 */
	void readSubRoads();

	/**
	 * Reads the points of interest from the file "poi.txt"
	 */
	void readPOI();

	/**
	 * Fills the graph with the information acquired
	 */
	void fillGraph();
public:
	MapBuilder(Map* map);
};

#endif /* MAPBUILDER_H_ */
