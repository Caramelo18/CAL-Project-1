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
	this->edgeId = counter++;
}

long long Map::SubRoad::getOriginId()
{
	return originId;
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
	readPOI(in);
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
		getline(in, read, separator); // longitude_degrees
		getline(in, read, separator); // longitude_radians
		ss << setprecision(10) << read;
		ss >> longitude;
		ss.clear();
		getline(in, read); // latitude_radians
		ss << setprecision(10) << read;
		ss >> latitude;
		ss.clear();

		//cout << latitude << "  " << longitude << endl;
		Node tempNode(id, latitude, longitude);
		graph.addVertex(tempNode);

		shared_ptr<Node> tempPointer(new Node(id, latitude, longitude));
		pair<long long, shared_ptr<Node> > tempPair(id, tempPointer);
		nodes.insert(tempPair);
		pair<double, shared_ptr<Node> > orderedLatPair(tempNode.getLatitude(), tempPointer);
		orderedLatNodes.insert(orderedLatPair);
		pair<double, shared_ptr<Node> > orderedLonPair(tempNode.getLongitude(), tempPointer);
		orderedLonNodes.insert(orderedLonPair);
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

void Map::readPOI(ifstream &in)
{
	in.open("poi.txt");
	string line, type, name;
	long long id;
	double lat, lon;

	while(!in.eof())
	{
		getline(in, line);

		int delimID = line.find(";");
		id = strtoull(line.substr(0, delimID).c_str(), NULL, 10);

		int delimType = line.find(";;", delimID);
		type = line.substr(delimID + 1, delimType - delimID - 1);

		int delimName = line.find(";;", delimType + 2);
		name = line.substr(delimType + 2, delimName - delimType - 2);

		int delimLat = line.find(";", delimName + 2);
		lat = strtod(line.substr(delimName + 2, delimLat - delimName - 2).c_str(), NULL);
		lat = lat * acos(-1) / 180;
		lon = strtod(line.substr(delimLat + 1).c_str(), NULL);
		lon = lon * acos(-1) / 180;

		long long nodeID;
		if (nodes.count(id) != 0)
			nodeID = id;
		else
			nodeID = findClosestNodeID(lat, lon);

		switch (type[0]) {
		case 'a':
			atmList.push_back(nodeID);
			break;
		case 'f':
			fuelList.push_back(nodeID);
			break;
		case 'h':
			hospitalList.push_back(nodeID);
			break;
		case 'p':
			pharmacyList.push_back(nodeID);
			break;
		case 'r':
			restaurantList.push_back(nodeID);
			break;
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
			shared_ptr<Node> destNode = nodes.at(tempSubRoad.getDestId());
			Road tempRoad = roads.at(tempSubRoad.getRoadId());
			double distance = getDistance(originNode, *destNode);
			//	cout << "Origin: " << originNode.getId() << " - Destination: " << destNode.getId() << " - Distance: " << distance << endl;
			graph.addEdge(node->getInfo(), *destNode, tempRoad, distance);
		}
	}
}

double Map::getDistance(Node n1, Node n2)
{
	//	Distance calculated using Haversine formula.
	//  Taken from: http://stackoverflow.com/questions/365826/calculate-distance-between-2-gps-coordinates
	int R = 6371;
	double res = 1;

	double deltaLat = n2.getLatitude() - n1.getLatitude();
	double deltaLon = n2.getLongitude() - n1.getLongitude();

	double a = sin(deltaLat / 2) * sin(deltaLat / 2) + sin(deltaLon / 2) * sin(deltaLon / 2) * cos(n1.getLatitude()) * cos(n2.getLatitude());
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));

	res = R * c;

	return res;
}

void Map::askSource()
{
	string input;
	stringstream ss;
	long long id, destID;
	double startLat, startLon, endLat, endLon;
	/*cout << "Please insert the starting node ID: ";
	//cin >> id;
	cout << "Please insert the finishing node ID: \n";
	//cin >> destID;*/
	/*cout << "Please insert the starting point coordinates in radians (latitude, longitude): ";
	getline(cin, input);
	ss << input;
	ss >> startLat >> startLon;
	ss.clear();
	id = findID(startLat, startLon);
	auto it = nodes.find(id);
	if(it == nodes.end())
		cout << "No such node" << endl;
	cout << "Please insert the finishing point coordinates in radians (latitude, longitude): ";
	getline(cin, input);
	ss << input;
	ss >> endLat >> endLon;
	destID = findID(endLat, endLon);
	auto itDest = nodes.find(destID);
	if(itDest == nodes.end())
		cout << "No such node" << endl;*/

	id = 4090279480;
	destID = 1602335210;
	//id = 441803607; //rua 2
	//destID = 1309243906; // rua 22
	//id = 441803456; // 35 368
	//destID = 768566003; // 27 874
	auto it = nodes.find(id);
	auto itDest = nodes.find(destID);

	calculateShortestPath(*it->second, *itDest->second);

}

string Map::getOrientation(Node source, Node dest)
{
	double latS, lonS, latD, lonD;
	string dir;

	latS = source.getLatitude();
	lonS = source.getLongitude();
	latD = dest.getLatitude();
	lonD = dest.getLongitude();

	double deltaLon = lonD - lonS;
	double deltaLat = latD - latS;

	if(deltaLat < 0 && abs(deltaLat) > TOLERANCE)
		dir += "South ";
	else if(deltaLat > 0 && abs(deltaLat) > TOLERANCE)
		dir += "North ";

	if(deltaLon > 0 && abs(deltaLon) > TOLERANCE)
		dir += "East";
	else if(deltaLon < 0 && abs(deltaLon) > TOLERANCE)
		dir += "West";
	//cout << "Delta Lat: " << deltaLat << " Delta Lon: " << deltaLon << endl;
	//cout << "Return: " << dir << endl;
	return dir;
}

string Map::getNewDirection(string prevOr, string newOr)
{
	size_t prevN = prevOr.find("North");
	size_t prevS = prevOr.find("South");
	size_t prevE = prevOr.find("East");
	size_t prevW = prevOr.find("West");

	if(prevN != string::npos)
	{
		size_t newE = newOr.find("East");
		size_t newW = newOr.find("West");
		size_t newS = newOr.find("South");
		if(newE != string::npos)
			return "right";
		else if (newW != string::npos)
			return "left";
		else if(newS != string::npos)
			return "turn arround";
	}
	else if(prevS != string::npos)
	{
		size_t newE = newOr.find("East");
		size_t newW = newOr.find("West");
		size_t newN = newOr.find("North");
		if(newE != string::npos)
			return "left";
		else if (newW != string::npos)
			return "right";
		else if(newN != string::npos)
			return "turn arround";
	}
	else if(prevE != string::npos)
	{
		size_t newN = newOr.find("North");
		size_t newS = newOr.find("South");
		size_t newW = newOr.find("West");
		if(newN != string::npos)
			return "left";
		else if (newS != string::npos)
			return "right";
		else if(newW != string::npos)
			return "turn arround";
	}
	else if(prevW != string::npos)
	{
		size_t newS = newOr.find("South");
		size_t newN = newOr.find("North");
		size_t newE = newOr.find("East");
		if(newS != string::npos)
			return "left";
		else if (newN != string::npos)
			return "right";
		else if(newE != string::npos)
			return "turn arround";
	}
}


long long Map::findID(double latitude, double longitude)
{
	auto range = orderedLatNodes.equal_range(latitude);
	for (auto it = range.first; it != range.second; ++it) {
		shared_ptr<Node> node = it->second;
		if (node->getLongitude() == longitude)
			return node->getId();
	}

	return -1;
}

long long Map::findClosestNodeID(double latitude, double longitude) {
	Node tempNode(0, latitude, longitude);

	// Search by Latitude
	auto it = orderedLatNodes.lower_bound(latitude);
	shared_ptr<Node> closestNode = it->second;
	double currentDistance = getDistance(tempNode, *closestNode);

	shared_ptr<Node> nextNode = (--it)->second;
	double nextDistance = getDistance(tempNode, *nextNode);
	if (nextDistance < currentDistance)
		closestNode = nextNode;

	// Search by Longitude
	it = orderedLonNodes.lower_bound(longitude);
	nextNode = it->second;
	nextDistance = getDistance(tempNode, *nextNode);
	if (nextDistance < currentDistance)
		closestNode = nextNode;
	nextNode = (--it)->second;
	nextDistance = getDistance(tempNode, *nextNode);
	if (nextDistance < currentDistance)
		closestNode = nextNode;

	return closestNode->getId();

}

double Map::getXCoords(long long id)
{
	auto it = nodes.find(id);
	double latitude = it->second->getLatitude();
	double longitude = it->second->getLongitude();

	return longitude;
	//return 6371 * cos(latitude) * cos(longitude);
}

double Map::getYCoords(long long id)
{
	auto it = nodes.find(id);
	double latitude = it->second->getLatitude();
	double longitude = it->second->getLongitude();

	//return 6371 * cos(latitude) * sin(longitude);
	return latitude;
}

long long Map::SubRoad::counter =0;

void Map::start(){
	GraphViewer test(640,640,false);
	//test.defineVertexColor(WHITE);
	test.createWindow(900,800);
	vector<Vertex<Node, Road> *> oui = graph.getVertexSet();
	for(unsigned int i=0; i< oui.size(); i++){
		double xdouble = getXCoords(oui[i]->getInfo().getId());
		double ydouble = getYCoords(oui[i]->getInfo().getId());
		int x = remainder((xdouble*10000000),10000) - 2000;
		int y = remainder((-ydouble*10000000),10000) - 3000;
		test.addNode(oui[i]->getInfo().getId(), x , y);
	}
	for(auto it = subRoads.begin(); it != subRoads.end(); it++ ){
		test.addEdge(it->second.edgeId, it->second.getOriginId(), it->second.getDestId(),
				roads.at(it->second.getRoadId()).getTwoWay()?0:1);
		test.setEdgeLabel(it->second.edgeId, roads.at(it->second.getRoadId()).getRoadName());
	}

	long long orId, destId;
	vector<long long> path;
	vector<long long> edges;
	vector<string> instructions;
	while(test.isRunning()){
		Sleep(59);
		if(test.isReady()){


			for(unsigned int i=0; i<path.size(); i++){
				test.setVertexColor(path[i],YELLOW);
			}
			for(unsigned int i=0; i< edges.size(); i++){
				test.setEdgeColor(edges[i],BLACK);
				test.setEdgeThickness(edges[i],1);
			}
			edges.clear();
			path.clear();

			getData(orId, destId);
			instructions.clear();
			graph.dijkstraShortestPath(*nodes.at(orId));
			instructions = this->calculateShortestPath(*nodes.at(orId), *nodes.at(destId));
			if(instructions.size()==0){
				test.giveDirections("there is no reachable path to the destination from this point");
				continue;
			}

			long long currentId = destId;
			path.push_back(currentId);
			while(graph.getVertex(*nodes.at(currentId))->path != NULL &&
					graph.getVertex(*nodes.at(currentId))->path->getInfo().getId() != orId){
				currentId = graph.getVertex(*nodes.at(currentId))->path->getInfo().getId();
				path.push_back(currentId);

			}
			path.push_back(orId);

			for(unsigned int i=path.size()-1; i > 0; i--){
				auto range = subRoads.equal_range(path[i]);
				for(auto it = range.first; it != range.second; it++){
					if(it->second.getDestId()==path[i-1]){
						edges.push_back(it->second.edgeId);
						break;
					}
				}
			}
			for(unsigned int i=0; i< edges.size(); i++){
				test.setEdgeColor(edges[i],BLUE);
				test.setEdgeThickness(edges[i],10);
			}

			for(unsigned int i=0; i < path.size(); i++){
				test.setVertexColor(path[i], GREEN);
			}
			test.rearrange();
			for(unsigned int i=0; i<instructions.size(); i++){
				test.giveDirections(instructions[i]);
			}
		}
		Sleep(40);
	}

}

vector<string> Map::calculatePath(Node source, Node dest)
{
	vector<pair<string, vector<long long> > > stopsVector;
	shared_ptr<Node> origin = nodes.at(source.getId());
	string closestType;
	shared_ptr<Node> closestNode;
	double closestDist;

	if (bank && atmList.size() > 0)
		stopsVector.push_back(make_pair("atm", atmList));
	if (gasStation && fuelList.size() > 0)
		stopsVector.push_back(make_pair("fuel", fuelList));
	if (pharmacy && pharmacyList.size() > 0)
		stopsVector.push_back(make_pair("pharmacy", pharmacyList));
	if (hospital && hospitalList.size() > 0)
		stopsVector.push_back(make_pair("hospital", hospitalList));
	if (restaurant && restaurantList.size() > 0)
		stopsVector.push_back(make_pair("restaurant", restaurantList));

	while (stopsVector.size() > 0) {
		closestNode = nodes.at(stopsVector[0].second[0]);
		closestType = stopsVector[0].first;
		closestDist = getDistance(*origin, *closestNode);

		for (size_t i = 0; i < stopsVector.size(); ++i) {
			vector<long long> amenityList = stopsVector[i].second;
			for (size_t j = 0; j < amenityList.size(); ++j) {
				shared_ptr<Node> tempNode = nodes.at(amenityList[j]);
				double tempDist = getDistance(*origin, *tempNode);
				if (tempDist < closestDist) {
					closestNode = tempNode;
					closestType = stopsVector[i].first;
					closestDist = tempDist;
				}
			}
		}

		for (size_t i = 0; i < stopsVector.size(); ++i) {
			if (stopsVector[i].first == closestType) {
				stopsVector.erase(stopsVector.begin() + i);
				break;
			}
		}

		calculateShortestPath(*origin, *closestNode);
		origin = closestNode;
	}

	calculateShortestPath(*origin, dest);


}

vector<string> Map::calculateShortestPath(Node source, Node dest)
{
	//graph.dijkstraShortestPath(source);

	vector<string> directions;
	Vertex<Node, Road>* v = graph.getVertex(dest);
	vector<Vertex<Node, Road>* > ret;
	ret.push_back(v);
	if(v->path==NULL) return directions;
	while(v->path != NULL && !(v->path->getInfo() == source))
	{
		v = v->path;
		ret.push_back(v);
	}
	if( v->path != NULL )
	{
		v = v->path;
		ret.push_back(v);
	}

	string prevDir, currDir, tmpDir, direction;
	double dist = 0;
	currDir = getOrientation(ret[ret.size() - 1]->getInfo(), ret[ret.size() - 2]->getInfo());
	int change = ret.size() - 1;
	for(unsigned int i = change; i > 0; i--)
	{
		prevDir = currDir;
		tmpDir = getOrientation(ret[i]->getInfo(), ret[i-1]->getInfo());
		if(tmpDir != "")
			currDir = tmpDir;
		size_t diff = currDir.find(prevDir);
		if(diff == string::npos)
		{

			direction = getNewDirection(prevDir, currDir);
			dist = ret[i]->getDist() - ret[change]->getDist();
			int distance = (int) (dist * 1000);
			stringstream ss;
			string strDist;
			ss << distance;
			ss >> strDist;
			string vDir = prevDir + " for " + strDist + " meters then turn " + direction;
			directions.push_back(vDir);
			change = i;
		}
		//cout << endl << ret[i]->getInfo() << "dist: " << ret[i]->getDist();
	}

	string finalDir;
	dist = ret[0]->getDist() - ret[change]->getDist();
	int distance = (int) (dist * 1000);
	stringstream ss;
	string strDist;
	ss << distance;
	ss >> strDist;
	finalDir = currDir + " for " + strDist + " meters then turn " + direction;
	directions.push_back(finalDir);
	ss.clear();
	//cout << currDir << " for " << (int) (dist * 1000) << " meters." << endl;


	dist = ret[0]->getDist() - ret[ret.size() - 1]->getDist();
	distance = (int) (dist * 1000);
	ss << distance;
	ss >> strDist;
	direction = "Total distance: " + strDist + " meters.";
	directions.push_back(direction);



	//cout << "Total distance: " << (int) (dist * 1000) << " meters" << endl;
	//cout << endl << ret[0]->getInfo()  << "dist: " << ret[0]->getDist();
	return directions;
}

void Map::getData(long long &originId, long long &destinationId){
	ifstream i;
	i.open("data.properties");
	string line, value;
	getline(i, line);
	while(!i.eof()){
		if(i.good()){
			getline(i, line, '=');
			if(line.empty()) break;
			getline(i,value);
			switch(line[0]){
			case BANK:
				if(value == "false") bank = false;
				else bank = true;
				break;
			case GASSTATION:
				if(value == "false") gasStation = false;
				else gasStation = true;
				break;
			case HOSPITAL:
				if(value == "false") hospital = false;
				else hospital = true;
				break;
			case PHARMACY:
				if(value == "false") pharmacy = false;
				else pharmacy = true;
				break;
			case RESTAURANT:
				if(value == "false") restaurant = false;
				else restaurant = true;
				break;
			case ORIGIN:
				originId = strtoll(value.c_str(), NULL, 10);
				break;
			case DESTINATION:
				destinationId = strtoll(value.c_str(), NULL, 10);
				break;
			}
		}
	}
}
