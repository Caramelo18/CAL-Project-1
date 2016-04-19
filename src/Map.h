#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "Graph.h"

using namespace std;

class Map {

	class Node {
	private:
		long long nodeId;
		double longitude, latitude; // in degrees
	public:
		Node(long long nodeId, double longitude, double latitude);
		long long getId() const;
		double getLongitude() const;
		double getLatitude() const;
		bool operator==(const Node& comparable);
	};

	class Road {
	private:
		long long roadId;
		string roadName;
		bool isTwoWay;
	public:
		Road(long long roadId, string roadName, bool isTwoWay);
		long long getId() const;
		string getRoadName() const;
		bool getTwoWay() const;
	};

private:
	Graph<Node *> graph;
	vector<Road> roads;	//temporary data structure

	void readNodes(ifstream &in);
	void readRoads(ifstream &in);
	void readSubRoads(ifstream &in);

public:
	void readInfo();
	Map(){};
};

#endif // MAP_H_
