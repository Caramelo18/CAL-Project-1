#include "Map.h"

using namespace std;

long long Map::SubRoad::counter = 0;

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
	return this->latitude == comparable.latitude &&
			this->longitude == comparable.longitude;
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
	string dir = "";

	latS = source.getLatitude();
	lonS = source.getLongitude();
	latD = dest.getLatitude();
	lonD = dest.getLongitude();

	double deltaLon = lonD - lonS;
	double deltaLat = latD - latS;

	if(deltaLat < 0 && abs(deltaLat) > TOLERANCE)
		dir += "South";
	else if(deltaLat > 0 && abs(deltaLat) > TOLERANCE)
		dir += "North";

	if(deltaLon > 0 && abs(deltaLon) > TOLERANCE) {
		if (dir != "")
			dir += " ";
		dir += "East";
	}

	else if(deltaLon < 0 && abs(deltaLon) > TOLERANCE) {
		if (dir != "")
			dir += " ";
		dir += "West";
	}

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
			return "turn around";
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
			return "turn around";
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
			return "turn around";
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
			return "turn around";
	}
	return "";
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

void Map::start()
{
	GraphViewer window(640, 640, false);
	window.createWindow(900, 800);

	vector<Vertex<Node, Road> *> vertexList = graph.getVertexSet();

	// Filing the GraphViewer with the Nodes
	for (unsigned int i = 0; i < vertexList.size(); ++i)
	{
		double xdouble = vertexList[i]->getInfo().getLongitude();
		double ydouble = vertexList[i]->getInfo().getLatitude();
		int x = remainder((xdouble * 10000000), 10000) - 2000;
		int y = remainder((-ydouble * 10000000), 10000) - 3000;
		window.addNode(vertexList[i]->getInfo().getId(), x , y);
	}

	// Adding the edges to the GraphViewer
	for (auto it = subRoads.begin(); it != subRoads.end(); ++it)
	{
		window.addEdge(it->second.edgeId, it->second.getOriginId(), it->second.getDestId(),
				roads.at(it->second.getRoadId()).getTwoWay() ? 0 : 1);
		window.setEdgeLabel(it->second.edgeId, roads.at(it->second.getRoadId()).getRoadName());
	}

	long long orId, destId;
	vector<long long> edges;
	vector<long long> path;
	vector<string> instructions;

	while(window.isRunning())
	{
		Sleep(100);
		if (window.isReady())
		{
			for (unsigned int i = 0; i < path.size(); ++i)
				window.setVertexColor(path[i], YELLOW);

			for (unsigned int i = 0; i < edges.size(); ++i)
			{
				window.setEdgeColor(edges[i], BLACK);
				window.setEdgeThickness(edges[i], 1);
			}
			edges.clear();
			path.clear();
			instructions.clear();

			if(!getData(orId, destId))
			{
				window.giveDirections("Error reading node IDs");
				continue;
			}
			auto start = nodes.find(orId);
			auto end   = nodes.find(destId);
			if(start == nodes.end() || end == nodes.end())
			{
				window.giveDirections("Cannot find starting and or destination IDs");
				continue;
			}
			instructions = this->calculatePath(*nodes.at(orId), *nodes.at(destId), path, edges);

			if(instructions.size()==0)
			{
				window.giveDirections("There is no reachable path to the destination from this point");
				continue;
			}

			long long currentId = destId;
			path.push_back(currentId);
			while(graph.getVertex(*nodes.at(currentId))->path != NULL &&
					graph.getVertex(*nodes.at(currentId))->path->getInfo().getId() != orId)
			{
				currentId = graph.getVertex(*nodes.at(currentId))->path->getInfo().getId();
				path.push_back(currentId);

			}

			path.push_back(orId);

			for(unsigned int i=path.size()-1; i > 0; i--)
			{
				bool found = false;
				auto range = subRoads.equal_range(path[i]);
				auto range2 = subRoads.equal_range(path[i-1]);
				for(auto it = range.first; it != range.second; ++it)
				{

					if(it->second.getDestId()==path[i-1])
					{
						edges.push_back(it->second.edgeId);
						found = true;
						break;
					}
				}
				if(found)
					continue;
				for(auto it = range2.first; it != range2.second; ++it)
				{
					if(it->second.getDestId()==path[i]){
						edges.push_back(it->second.edgeId);
						break;
					}
				}
			}

			for(unsigned int i=0; i< edges.size(); ++i){
				window.setEdgeColor(edges[i],BLUE);
				window.setEdgeThickness(edges[i],10);
			}

			for(unsigned int i=0; i < path.size(); ++i){
				window.setVertexColor(path[i], GREEN);
			}

			window.rearrange();

			for(unsigned int i=0; i<instructions.size(); ++i){
				window.giveDirections(instructions[i]);
			}
		}
	}
}

vector<pair<string, vector<long long> > > Map::getStopsList(vector<string> &instructions)
{
	vector<pair<string, vector<long long> > > stopsVector;
	vector<long long> atmTemp, gasTemp, pharmTemp, hospTemp, restTemp;

	if (bank)
	{
		if (atmList.size() > 0) {
			for(auto key : atmList)
				if(graph.getVertex(*nodes.at(key))->path != NULL)
					atmTemp.push_back(key);

			stopsVector.push_back(make_pair("atm", atmTemp));
		}
		else
		{
			instructions.push_back("There are no ATMs on the map.");
		}
	}

	if (gasStation)
	{
		if (fuelList.size() > 0) {
			for(auto key : fuelList)
				if(graph.getVertex(*nodes.at(key))->path != NULL)
					gasTemp.push_back(key);
		//	if(gasTemp.empty())
		//		instructions.push_back("There are no Gas Stations reachable from this Starting Point");
			stopsVector.push_back(make_pair("fuel", gasTemp));
		}
		else
		{
			instructions.push_back("There are no Gas Stations on the map.");
		}
	}

	if (pharmacy)
	{
		if (pharmacyList.size() > 0) {
			for(auto key : pharmacyList)
				if(graph.getVertex(*nodes.at(key))->path != NULL)
					pharmTemp.push_back(key);

			stopsVector.push_back(make_pair("pharmacy", pharmTemp));
		}
		else
		{
			instructions.push_back("There are no Pharmacies on the map.");
		}
	}

	if (hospital)
	{
		if (hospitalList.size() > 0) {
			for(auto key : hospitalList)
				if(graph.getVertex(*nodes.at(key))->path != NULL)
					hospTemp.push_back(key);

			stopsVector.push_back(make_pair("hospital", hospTemp));
		}
		else
		{
			instructions.push_back("There are no Hospitals on the map.");
		}
	}

	if (restaurant)
	{
		if (restaurantList.size() > 0) {
			for(auto key : restaurantList)
				if(graph.getVertex(*nodes.at(key))->path != NULL)
					restTemp.push_back(key);

			stopsVector.push_back(make_pair("restaurant", restTemp));
		}
		else
		{
			instructions.push_back("There are no Restaurants on the map.");
		}
	}

	return stopsVector;
}

vector<string> Map::calculatePath(const Node &source, const Node &dest, vector<long long> &path, vector <long long> &edges)
{
	vector<string> instructions;

	string closestType;
	shared_ptr<Node> closestNode;
	double closestDist;

	shared_ptr<Node> origin = nodes.at(source.getId());
	graph.dijkstraShortestPath(*origin);

	vector<pair<string, vector<long long> > > stopsVector = getStopsList(instructions);

	while (stopsVector.size() > 0)
	{
		closestNode = nodes.at(stopsVector[0].second[0]);
		closestType = stopsVector[0].first;
		closestDist = graph.getVertex(*closestNode)->getDist();

		for (size_t i = 0; i < stopsVector.size(); ++i)
		{
			vector<long long> amenityList = stopsVector[i].second;
			for (size_t j = 0; j < amenityList.size(); ++j)
			{
				shared_ptr<Node> tempNode = nodes.at(amenityList[j]);
				double tempDist = graph.getVertex(*tempNode)->getDist();

				if (tempDist < closestDist)
				{
					closestNode = tempNode;
					closestType = stopsVector[i].first;
					closestDist = tempDist;
				}
			}
		}

		for (size_t i = 0; i < stopsVector.size(); ++i)
		{
			if (stopsVector[i].first == closestType)
			{
				stopsVector.erase(stopsVector.begin() + i);
				break;
			}
		}

		vector<string> part;
		part = calculateShortestPath(*origin, *closestNode);
		instructions.insert(instructions.end(), part.begin(), part.end());

		long long currentId = closestNode->getId();
		Vertex<Map::Node, Map::Road> * currentVertex = graph.getVertex(*nodes.at(currentId));
		path.push_back(currentId);

		while (currentVertex->path != NULL && currentVertex->path->getInfo().getId() != origin->getId())
		{
			currentId = currentVertex->path->getInfo().getId();
			currentVertex = graph.getVertex(*nodes.at(currentId));
			path.push_back(currentId);
		}

		path.push_back(origin->getId());


		origin = closestNode;
		graph.dijkstraShortestPath(*origin);
	}

	vector<string> part;
	part = calculateShortestPath(*origin, dest);
	instructions.insert(instructions.end(), part.begin(), part.end());

	long long currentId = dest.getId();
	Vertex<Map::Node, Map::Road> * currentVertex = graph.getVertex(*nodes.at(currentId));
	path.push_back(currentId);

	while (currentVertex->path != NULL && currentVertex->path->getInfo().getId() != origin->getId())
	{
		currentId = currentVertex->path->getInfo().getId();
		currentVertex = graph.getVertex(*nodes.at(currentId));
		path.push_back(currentId);
	}

	path.push_back(origin->getId());

	for(size_t i = path.size() - 1; i > 0; --i)
	{
		bool found = false;
		auto range = subRoads.equal_range(path[i]);
		auto range2 = subRoads.equal_range(path[i-1]);

		for(auto it = range.first; it != range.second; ++it)
		{
			if(it->second.getDestId()==path[i-1])
			{
				found = true;
				edges.push_back(it->second.edgeId);
				break;
			}
		}

		if(found)
			continue;

		for(auto it = range2.first; it != range2.second; it++)
		{
			if(it->second.getDestId()==path[i])
			{
				edges.push_back(it->second.edgeId);
				break;
			}
		}
	}
	return instructions;
}

vector<string> Map::calculateShortestPath(const Node &source, const Node &dest)
{
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

bool Map::getData(long long &originId, long long &destinationId)
{
	ifstream i;
	i.open("data.properties");

	string line, value;
	getline(i, line);

	while(!i.eof())
	{
		if(i.good())
		{
			getline(i, line, '=');
			if(line.empty())
				break;

			getline(i, value);

			switch (line[0]) {
			case BANK:
				if(value == "false")
					bank = false;
				else
					bank = true;
				break;
			case GASSTATION:
				if(value == "false")
					gasStation = false;
				else
					gasStation = true;
				break;
			case HOSPITAL:
				if(value == "false")
					hospital = false;
				else
					hospital = true;
				break;
			case PHARMACY:
				if(value == "false")
					pharmacy = false;
				else
					pharmacy = true;
				break;
			case RESTAURANT:
				if(value == "false")
					restaurant = false;
				else
					restaurant = true;
				break;
			case ORIGIN:
				if((originId = strtoll(value.c_str(), NULL, 10)) == NULL)
					return false;
				break;
			case DESTINATION:
				if((destinationId = strtoll(value.c_str(), NULL, 10)) == NULL)
					return false;
				break;
			}
		}
		else return false;
	}
	return true;
}
