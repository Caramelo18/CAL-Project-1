#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <unordered_map>
#include <map>
#include <cmath>
#include <stack>
#include <vector>
#include "Graph.h"
#include "graphviewer.h"

#define TOLERANCE 0.0000056
#define BANK		'b'
#define GASSTATION	'g'
#define HOSPITAL	'h'
#define PHARMACY	'p'
#define RESTAURANT	'r'
#define ORIGIN		'o'
#define DESTINATION	'd'

using namespace std;

class Map {

	class Node {
	private:
		long long nodeId;
		double longitude, latitude; // in radians
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

		static long long counter;
	public:
		SubRoad(long long originId, long long destId, long long roadId);
		long long getOriginId();
		long long getDestId();
		long long getRoadId();
		long long edgeId;
	};

private:
	Graph<Node, Road> graph;

	unordered_map<long long, shared_ptr<Node> > nodes;
	unordered_map<long long, Road> roads;
	unordered_multimap<long long, SubRoad> subRoads;

	multimap<double, shared_ptr<Node> > orderedLatNodes;
	multimap<double, shared_ptr<Node> > orderedLonNodes;

	vector<long long> atmList;
	vector<long long> fuelList;
	vector<long long> hospitalList;
	vector<long long> pharmacyList;
	vector<long long> restaurantList;

	bool gasStation, pharmacy, airports, hospital, restaurant, bank;

	void readNodes(ifstream &in);
	void readRoads(ifstream &in);
	void readSubRoads(ifstream &in);
	void readPOI(ifstream &in);
	double getDistance(Node n1, Node n2);
	vector<string> calculateShortestPath(Node source, Node dest);
	string getOrientation(Node source, Node dest);
	string getNewDirection(string prevOr, string newOr);
	long long findID(double latitude, double longitude);
	long long findClosestNodeID(double latitude, double longitude);
	void getData(long long &originId, long long &destinationId);


public:
	void readInfo();
	void fillGraph();
	void askSource();
	double getXCoords(long long id);
	double getYCoords(long long id);
	void start();
	Map(){};
};

#endif // MAP_H_
