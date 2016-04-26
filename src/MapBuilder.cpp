#include "MapBuilder.h"

MapBuilder::MapBuilder(Map* map) {
	this->map = map;
	readNodes();
	readRoads();
	readSubRoads();
	readPOI();
	fillGraph();
}

void MapBuilder::readNodes()
{
	ifstream in;
	in.open("nodes.txt");

	string read;
	long long id;
	double latitude, longitude;
	char separator = ';';

	stringstream ss;

	while(!in.eof())
	{
		getline(in, read, separator); // read nodeID
		id = strtoll(read.c_str(), NULL,10);
		getline(in, read, separator); // ignore latitudeDegrees
		getline(in, read, separator); // ignore longitudeDegrees
		getline(in, read, separator); // read longitudeRadians
		longitude = strtod(read.c_str(), NULL);
		getline(in, read); // read latitudeRadians
		latitude = strtod(read.c_str(), NULL);

		// add Node to graph
		Map::Node tempNode(id, latitude, longitude);
		map->graph.addVertex(tempNode);

		// store shared pointer in Map class
		shared_ptr<Map::Node> tempPointer(new Map::Node(id, latitude, longitude));
		pair<long long, shared_ptr<Map::Node> > tempPair(id, tempPointer);
		map->nodes.insert(tempPair);
		pair<double, shared_ptr<Map::Node> > orderedLatPair(tempNode.getLatitude(), tempPointer);
		map->orderedLatNodes.insert(orderedLatPair);
		pair<double, shared_ptr<Map::Node> > orderedLonPair(tempNode.getLongitude(), tempPointer);
		map->orderedLonNodes.insert(orderedLonPair);
	}

	in.close();
}

void MapBuilder::readRoads()
{
	ifstream in;
	in.open("roads.txt");

	string read, roadName;
	long long roadID;
	bool twoWay;
	char separator = ';';

	stringstream ss;

	while(!in.eof())
	{
		getline(in, read, separator); // read roadID
		roadID = strtoll(read.c_str(), NULL, 10);
		getline(in, roadName, separator); // read roadName
		getline(in, read); // checks if it's twoWay
		if(read == "True")
			twoWay = true;
		else
			twoWay = false;

		// store Roads in Map class
		Map::Road tempRoad(roadID, roadName, twoWay);
		pair<long long, Map::Road> tempPair(roadID, tempRoad);
		map->roads.insert(tempPair);
	}

	in.close();
}


void MapBuilder::readSubRoads()
{
	ifstream in;
	in.open("subroads.txt");

	string read, tmp;
	long long roadID, node1ID, node2ID;
	char separator = ';';

	stringstream ss;

	while(!in.eof())
	{
		getline(in, read, separator); // read roadID
		roadID = strtoll(read.c_str(), NULL, 10);
		getline(in, read, separator); // read node1ID
		node1ID = strtoll(read.c_str(), NULL, 10);
		getline(in, read, separator); // read node2ID
		node2ID = strtoll(read.c_str(), NULL, 10);

		Map::SubRoad tempSubRoad(node1ID, node2ID, roadID);
		pair<long long, Map::SubRoad> tempPair(node1ID, tempSubRoad);
		map->subRoads.insert(tempPair);
	}

	in.close();
}

void MapBuilder::readPOI()
{
	ifstream in;
	in.open("poi.txt");

	string line, type, name;
	long long id;
	double lat, lon;

	while(!in.eof())
	{
		getline(in, line);

		int delimID = line.find(";"); // read nodeID
		id = strtoll(line.substr(0, delimID).c_str(), NULL, 10);

		int delimType = line.find(";;", delimID); // read type
		type = line.substr(delimID + 1, delimType - delimID - 1);

		int delimName = line.find(";;", delimType + 2); // read name
		name = line.substr(delimType + 2, delimName - delimType - 2);

		// read latitude and longitude in degrees and convert to radians
		int delimLat = line.find(";", delimName + 2);
		lat = strtod(line.substr(delimName + 2, delimLat - delimName - 2).c_str(), NULL);
		lat = lat * pi / 180;
		lon = strtod(line.substr(delimLat + 1).c_str(), NULL);
		lon = lon * pi / 180;

		long long nodeID;

		if (map->nodes.count(id) != 0)
			nodeID = id;
		else
			nodeID = map->findClosestNodeID(lat, lon);

		switch (type[0]) {
		case 'a':
			map->atmList.push_back(nodeID);
			break;
		case 'f':
			map->fuelList.push_back(nodeID);
			break;
		case 'h':
			map->hospitalList.push_back(nodeID);
			break;
		case 'p':
			map->pharmacyList.push_back(nodeID);
			break;
		case 'r':
			map->restaurantList.push_back(nodeID);
			break;
		}
	}

	in.close();
}


void MapBuilder::fillGraph()
{
	for (auto node : map->graph.getVertexSet())
	{
		long long id = node->getInfo().getId();
		auto range = map->subRoads.equal_range(id);
		for (auto it = range.first; it != range.second; ++it)
		{
			Map::SubRoad tempSubRoad = it->second;
			Map::Node originNode = node->getInfo();
			shared_ptr<Map::Node> destNode = map->nodes.at(tempSubRoad.getDestId());
			Map::Road tempRoad = map->roads.at(tempSubRoad.getRoadId());
			double distance = map->getDistance(originNode, *destNode);

			map->graph.addEdge(node->getInfo(), *destNode, tempRoad, distance);
			if(tempRoad.getTwoWay())
				map->graph.addEdge(*destNode, node->getInfo(), tempRoad, distance);
		}
	}
}

