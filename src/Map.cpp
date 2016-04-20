#include "Map.h"

using namespace std;

Map::Node::Node(long long nodeId, double latitude, double longitude)
{
	this->nodeId = nodeId;
	this->latitude = latitude;
	this->longitude = longitude;
}

long long Map::Node::getId() const
{
	return nodeId;
}

double Map::Node::getLongitude() const
{
	return longitude;
}

double Map::Node::getLatitude() const
{
	return latitude;
}

bool Map::Node::operator==(const Node& comparable)
{
	if (this->nodeId == comparable.nodeId &&
		this->latitude == comparable.latitude &&
		this->longitude == comparable.longitude)
		return true;

	return false;
}

Map::Road::Road(long long roadId, string roadName, bool isTwoWay)
{
	this->roadId = roadId;
	this->roadName = roadName;
	this->isTwoWay = isTwoWay;
}

long long Map::Road::getId() const
{
	return roadId;
}

string Map::Road::getRoadName() const
{
	return roadName;
}

bool Map::Road::getTwoWay() const
{
	return isTwoWay;
}

Map::SubRoad::SubRoad(long long originId, long long destId, long long roadId)
{
	this->originId = originId;
	this->destId = destId;
	this->roadId = roadId;
}

long long Map::SubRoad::getDestId()
{
	return destId;
}

long long Map::SubRoad::getRoadId()
{
	return roadId;
}

void Map::readInfo()
{
	ifstream in;
	readNodes(in);
	readRoads(in);
	readSubRoads(in);
	fillGraph();
}

void Map::readNodes(ifstream &in)
{
	string read;
	long long id;
	double latitude, longitude;
	char separator = ';';
	stringstream ss;

	in.open("nodes.txt");

	while(!in.eof())
	{
		getline(in, read, separator); // node_id
		ss << read;
		ss >> id;
		ss.clear();
		getline(in, read, separator); // latitude_degrees
		ss << setprecision(10) << read;
		ss >> latitude;
		ss.clear();
		getline(in, read, separator); // longitude_degrees
		ss << setprecision(10) << read;
		ss >> longitude;
		ss.clear();
		getline(in, read); // ignores the latitude and longitude on radians

		Node tempNode(id, latitude, longitude);
		graph.addVertex(tempNode);
		pair<long long, Node> tempPair(id, tempNode);
		nodes.insert(tempPair);
	}

	in.close();
}

void Map::readRoads(ifstream &in)
{
	string read;
	long long roadID;
	string roadName;
	bool twoWay;
	char separator = ';';
	stringstream ss;

	in.open("roads.txt");

	while(!in.eof())
	{
		getline(in, read, separator); // road_id
		ss << read;
		ss >> roadID;
		ss.clear();
	//	cout << "road id: " << roadID;

		getline(in, roadName, separator); // road_name
	//	cout << "  -  road name: " << roadName;

		getline(in, read); // is_two_way

		if(read == "True")
			twoWay = true;
		else
			twoWay = false;

		Road tempRoad(roadID, roadName, twoWay);
		pair<long long, Road> tempPair(roadID, tempRoad);
		roads.insert(tempPair);
	}

	in.close();
}

void Map::readSubRoads(ifstream &in)
{
	string read, tmp;
	long long roadID, node1ID, node2ID;
	char separator = ';';
	stringstream ss;

	in.open("subroads.txt");

	while(!in.eof())
	{
		getline(in, read, separator); // road_id
		ss << read;
		ss >> roadID;
		ss.clear();
		//cout << "road id: " << roadID << endl;
		getline(in, read, separator); // node1_id
		ss << read;
		ss >> node1ID;
		ss.clear();
		//cout << "node 1 id: " << node1ID << endl;
		getline(in, read, separator); // node2_id
		ss << read;
		ss >> node2ID;
		ss.clear();
		//cout << "node 2 id: " << node2ID << endl;

		SubRoad tempSubRoad(node1ID, node2ID, roadID);
		pair<long long, SubRoad> tempPair(node1ID, tempSubRoad);
		subRoads.insert(tempPair);

		Road tempRoad = roads.at(roadID);
		if (tempRoad.getTwoWay()) {
			SubRoad tempSubRoad2(node2ID, node1ID, roadID);
			pair<long long, SubRoad> tempPair2(node2ID, tempSubRoad2);
			subRoads.insert(tempPair2);
		}
	}

	in.close();
}

void Map::fillGraph()
{
	for (auto node : graph.getVertexSet())
	{
		long long id = node->getInfo().getId();
		auto range = subRoads.equal_range(id);
		for (auto it = range.first; it != range.second; ++it)
		{
			SubRoad tempSubRoad = it->second;
			Node originNode = node->getInfo();
			Node destNode = nodes.at(tempSubRoad.getDestId());
			Road tempRoad = roads.at(tempSubRoad.getRoadId());
			double distance = getDistance(originNode, destNode);
			graph.addEdge(node->getInfo(), destNode, tempRoad, distance);
		}
	}
}

double Map::getDistance(Node n1, Node n2)
{
	return 1;
}
