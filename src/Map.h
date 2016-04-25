#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <cmath>
#include <stack>
#include <vector>
#include "Graph.h"

#define TOLERANCE 0.0000056

using namespace std;

class Map {

	class Node {
	private:
		long long nodeId;
		double longitude, latitude; // in radians
		string type, name; //for Points of Interest (POI)
	public:
		Node(long long nodeId, double longitude, double latitude);
		long long getId() const;
		double getLongitude() const;
		double getLatitude() const;
		bool operator==(const Node& comparable);
		friend ostream& operator<< (std::ostream &out, const Node &node)
		{
			out << "ID: " << node.nodeId << " Latitude: " << node.latitude << " Longitude: " << node.longitude << endl;
			return out;
		}
		void setType(string type);
		void setName(string name);
		string getType() const;
		string getName() const;
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

	class SubRoad {
	private:
		long long originId;
		long long destId;
		long long roadId;
	public:
		SubRoad(long long originId, long long destId, long long roadId);
		long long getOriginId();
		long long getDestId();
		long long getRoadId();
	};

private:
	Graph<Node, Road> graph;
	unordered_map<long long, Node> nodes;
	unordered_map<long long, Road> roads;
	unordered_multimap<long long, SubRoad> subRoads;

	void readNodes(ifstream &in);
	void readRoads(ifstream &in);
	void readSubRoads(ifstream &in);
	void readPOI(ifstream &in);
	double getDistance(Node n1, Node n2);
	void calculateShortestPath(Node source, Node dest);
	string getOrientation(Node source, Node dest);
	string getNewDirection(string prevOr, string newOr);
	long long findID(double latitude, double longitude);

public:
	void readInfo();
	void fillGraph();
	void askSource();
	Map(){};
};

#endif // MAP_H_
