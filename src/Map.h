#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "Graph.h"


using namespace std;

class Map{

	class Node{
	public:
		long nodeId;
		//double latitude_in_degrees, longitude_in_degrees;
		double longitudeRadians, latitudeRadians;
	};
	class Road{
	public:
		long roadId;
		string road_name;
		bool is_two_way;
	};

	Graph<Node> graph;
	vector<Road> roads;	//temporary data structure

public:
	void readInfo();
	Map() {};

private:
	void readNodes(ifstream &in);
	void readRoads(ifstream &in);
	void readSubRoads(ifstream &in);

};

#endif // MAP_H_
