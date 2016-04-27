#ifndef MAP_H_
#define MAP_H_

#include "Graph.h"
#include "graphviewer.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <unordered_map>
#include <map>


constexpr double TOLERANCE  = 0.0000056;
constexpr char BANK = 'b';
constexpr char GASSTATION = 'g';
constexpr char HOSPITAL = 'h';
constexpr char PHARMACY = 'p';
constexpr char RESTAURANT = 'r';
constexpr char ORIGIN = 'o';
constexpr char DESTINATION = 'd';

class Map {
	friend class MapBuilder;

	class Node {
	private:
		long long nodeId;
		double longitude, latitude; // in radians
	public:
		/**
		 * Node class constructor
		 * @param nodeId node ID
		 * @param latitude node's latitude
		 * @param longitude node's longitude
		 */
		Node(long long nodeId, double longitude, double latitude);

		/**
		 * Simple getter.
		 * @return nodeId
		 */
		long long getId() const;

		/**
		 * Simple getter.
		 * @return longitude
		 */
		double getLongitude() const;

		/**
		 * Simple getter.
		 * @return latitude
		 */
		double getLatitude() const;

		/**
		 * Operator == that compares two nodes
		 * @param comparable the Node to compare to
		 * @return true if the nodes have the same coordinates and false otherwise
		 */
		bool operator==(const Node& comparable);

		/**
		 * Operator << overloading
		 * Prints the Node information to the output stream
		 * @param out output stream
		 * @param node the node to print
		 */
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
		/**
		 * Road class constructor
		 * @param roadId Road ID
		 * @param roadName Road's name
		 * @param isTwoWay true if the road is bidirectional
		 */
		Road(long long roadId, string roadName, bool isTwoWay);

		/**
		 * Simple getter.
		 * @return roadId
		 */
		long long getId() const;

		/**
		 * Simple getter.
		 * @return roadName
		 */
		string getRoadName() const;

		/**
		 * Simple getter.
		 * @return isTwoWay
		 */
		bool getTwoWay() const;
	};

	class SubRoad {
	private:
		long long originId;
		long long destId;
		long long roadId;

		static long long counter;
	public:
		/**
		 * SubRoad class constructor
		 * @param originId the ID of the origin Node
		 * @param roadId the ID of the destination Node
		 * @param roadId Road ID
		 */
		SubRoad(long long originId, long long destId, long long roadId);

		/**
		 * Simple getter.
		 * @return originId
		 */
		long long getOriginId();

		/**
		 * Simple getter.
		 * @return destId
		 */
		long long getDestId();

		/**
		 * Simple getter.
		 * @return roadId
		 */
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

	bool gasStation, pharmacy, hospital, restaurant, bank;

	/**
	 * Given the coordinates of a Node, finds its ID.
	 * @param latitude node's latitude
	 * @param longitude node's longitude
	 * @return the ID of the Node if it exists and -1 otherwise
	 */
	long long findID(double latitude, double longitude);

	/**
	 * Given the coordinates of a point, finds the ID of the closest Node.
	 * @param latitude point's latitude
	 * @param longitude points's longitude
	 * @return the ID of the closest Node
	 */
	long long findClosestNodeID(double latitude, double longitude);

	/**
	 * Gets the distance between two Nodes
	 * @param n1 first Node
	 * @param n2 second Node
	 * @return distance between both Nodes
	 */
	double getDistance(Node n1, Node n2);
	string getOrientation(Node source, Node dest);
	string getNewDirection(string prevOr, string newOr);

	/**
	 * Gathers data sent from the GraphViewer in the data.properties file
	 * @param originID id of the origin Node to be updated in this function
	 * @param destinationID id of the destination Node to be updated in this function
	 * @return true if it's successful and false if there's an error reading the node's id's
	 */
	bool getData(long long &originId, long long &destinationId);
	vector<pair<string, vector<long long> > > getStopsList(vector<string> &instructions);
	vector<string> calculatePath(const Node &source, const Node &dest, vector<long long> &path, vector <long long> &edges, GraphViewer &window);
	pair<long, vector<string> > getInstructions(const Node &source, const Node &dest);

	/**
	 * Fills the path vector with the ID of nodes that are covered in the path from origin to destination
	 * @param origin origin Node
	 * @param dest destination Node
	 * @param path the path vector to be filled
	 */
	void fillPath(const Node &origin, const Node &dest, vector<long long> &path);

public:
	void start();
	/**
	 * Map class constructor
	 */
	Map(){};
};

#endif // MAP_H_
