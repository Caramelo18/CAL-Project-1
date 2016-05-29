#include "Map.h"

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
	// Distance calculated using Haversine formula.
	// Taken from: http://stackoverflow.com/questions/365826/calculate-distance-between-2-gps-coordinates
	int R = 6371;
	double res = 1;

	double deltaLat = n2.getLatitude() - n1.getLatitude();
	double deltaLon = n2.getLongitude() - n1.getLongitude();

	double a = sin(deltaLat / 2) * sin(deltaLat / 2) + sin(deltaLon / 2) * sin(deltaLon / 2) * cos(n1.getLatitude()) * cos(n2.getLatitude());
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));

	res = R * c;

	return res;
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
	auto tmpit = orderedLatNodes.lower_bound(latitude);
	auto it = tmpit;
	auto finalit = orderedLatNodes.end();
	shared_ptr<Node> closestNode = it->second;
	double currentDistance = getDistance(tempNode, *closestNode);

	--it;

	for (; it != finalit; --it)
	{
		shared_ptr<Node> nextNode = it->second;

		if (latitude - nextNode->getLatitude() > currentDistance)
			break;

		double nextDistance = getDistance(tempNode, *nextNode);
		if (nextDistance < currentDistance)
		{
			closestNode = nextNode;
			currentDistance = nextDistance;
		}
	}

	it = tmpit;

	for (++it; it != finalit; ++it)
	{
		shared_ptr<Node> nextNode = it->second;

		if (nextNode->getLatitude() - latitude > currentDistance)
			break;

		double nextDistance = getDistance(tempNode, *nextNode);
		if (nextDistance < currentDistance)
		{
			closestNode = nextNode;
			currentDistance = nextDistance;
		}
	}

	// Search by Longitude
	tmpit = orderedLonNodes.lower_bound(longitude);
	it = tmpit;
	finalit = orderedLonNodes.end();

	for (; it != finalit; --it)
	{
		shared_ptr<Node> nextNode = it->second;

		if (latitude - nextNode->getLatitude() > currentDistance)
			break;

		double nextDistance = getDistance(tempNode, *nextNode);
		if (nextDistance < currentDistance)
		{
			closestNode = nextNode;
			currentDistance = nextDistance;
		}
	}

	it = tmpit;

	for (++it; it != finalit; ++it)
	{
		shared_ptr<Node> nextNode = it->second;

		if (nextNode->getLongitude() - longitude > currentDistance)
			break;

		double nextDistance = getDistance(tempNode, *nextNode);
		if (nextDistance < currentDistance)
		{
			closestNode = nextNode;
			currentDistance = nextDistance;
		}
	}

	return closestNode->getId();

}

void Map::start()
{
	GraphViewer window(640, 640, false);
	window.createWindow(900, 800);

	vector<Vertex<Node, Road> *> vertexList = graph.getVertexSet();

	// Filing the GraphViewer with the Nodes
	for (size_t i = 0; i < vertexList.size(); ++i)
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
	string destination;
	long long orId, destId;
	vector<long long> edges;
	vector<long long> path;
	vector<string> instructions;
	vector<string> alternatives;

	while(window.isRunning())
	{
		Sleep(100);
		if (window.isReady())
		{
			for (size_t i = 0; i < path.size(); ++i)
				window.setVertexColor(path[i], YELLOW);

			for (size_t i = 0; i < edges.size(); ++i)
			{
				window.setEdgeColor(edges[i], BLACK);
				window.setEdgeThickness(edges[i], 1);
			}

			edges.clear();
			path.clear();
			instructions.clear();
			if(!getData(orId, destination))
			{
				window.giveDirections("Error reading node IDs");
				continue;
			}
			vector<Node> dest;
			if(street){
				//window.giveAlternatives("hey");
				alternatives = findNearestRoadName(destination);
				if(alternatives.empty()){
					window.giveDirections("Couldn't find road " + destination);
					continue;
				}
				if(alternatives.size() == 1 && alternatives[0] == destination)
				{
					dest = findNodeByRoad(destination);
				}
				else
				{
					for(auto i=0; i<alternatives.size(); i++){
						window.giveAlternatives(alternatives[i]);
					}
					continue;
				}
			}
			else {
				if((destId = strtoll(destination.c_str(), NULL, 10)) == 0LL)

				{
					window.giveDirections("Error reading node IDs");
					continue;
				}
				cout<<"poeira\n";

				auto start = nodes.find(orId);
				auto end   = nodes.find(destId);

				if(start == nodes.end() || end == nodes.end())
				{
					window.giveDirections("Cannot find starting and or destination IDs");
					continue;
				}
				dest.push_back(*nodes.at(destId));
			}
			instructions = this->calculatePath(*nodes.at(orId), dest, path, edges);


			if(instructions.size()==0)
			{
				window.giveDirections("There is no reachable path to the destination from this point");
				continue;
			}

			for (size_t i = 0; i< edges.size(); ++i){
				window.setEdgeColor(edges[i], BLUE);
				window.setEdgeThickness(edges[i], 10);
			}

			for (size_t i = 0; i < path.size(); ++i){
				window.setVertexColor(path[i], GREEN);
			}

			window.rearrange();

			for (size_t i = 0; i < instructions.size(); ++i){
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
			if(atmTemp.empty())
				instructions.push_back("There are no ATMs/Banks reachable from this Starting Point");
			else stopsVector.push_back(make_pair("atm", atmTemp));
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
			if(gasTemp.empty())
				instructions.push_back("There are no Gas Stations reachable from this Starting Point");
			else stopsVector.push_back(make_pair("fuel", gasTemp));
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
			if(pharmTemp.empty())
				instructions.push_back("There are no Pharmacies reachable from this Starting Point");
			else stopsVector.push_back(make_pair("pharmacy", pharmTemp));
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
			if(hospTemp.empty())
				instructions.push_back("There are no Hospitals reachable from this Starting Point");
			else stopsVector.push_back(make_pair("hospital", hospTemp));
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
			if(restTemp.empty())
				instructions.push_back("There are no Restaurants reachable from this Starting Point");
			else stopsVector.push_back(make_pair("restaurant", restTemp));
		}
		else
		{
			instructions.push_back("There are no Restaurants on the map.");
		}
	}

	return stopsVector;
}

vector<string> Map::calculatePath(const Node &source, const vector<Map::Node> &destination, vector<long long> &path, vector <long long> &edges)
{
	vector<string> instructions;
	int totalDistance = 0;

	string closestType;
	shared_ptr<Node> closestNode;
	double closestDist;

	shared_ptr<Node> origin = nodes.at(source.getId());
	graph.dijkstraShortestPath(*origin);
	Node dest = destination[0];
	int index = 0;
	for(auto i = 0; i < destination.size(); i++){
		if(graph.getVertex(destination[i])->path != NULL &&
				graph.getVertex(destination[i])->getDist() < graph.getVertex(destination[index])->getDist())
		{
			index = i;
		}
	}

	dest = destination[index];
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

		if (closestDist == INT_INFINITY) {
			stringstream ss;
			ss << "It isn't possible to reach any " << closestType << " from " << origin->getId() << ".";
			instructions.push_back(ss.str());
			continue;
		}

		pair<long, vector<string> > instPair = getInstructions(*origin, *closestNode);
		vector<string> instList = instPair.second;
		totalDistance += instPair.first;

		instructions.insert(instructions.end(), instList.begin(), instList.end());

		stringstream ss;
		ss << "You've reached the " << closestType << ".";
		instructions.push_back(ss.str());
		ss.str("");
		ss << "The distance covered was: " << instPair.first << " meters.";
		instructions.push_back(ss.str());

		fillPath(*origin, *closestNode, path);

		origin = closestNode;
		graph.dijkstraShortestPath(*origin);
	}

	pair<long, vector<string> > instPair = getInstructions(*origin, dest);
	vector<string> instList = instPair.second;
	totalDistance += instPair.first;

	instructions.insert(instructions.end(), instList.begin(), instList.end());

	if (instList.empty()) {
		stringstream ss;
		ss << "It isn't possible to reach the destination from " << origin->getId() << ".";
		instructions.push_back(ss.str());
	}
	else {
		instructions.push_back("You've reached the desired destination.");
		fillPath(*origin, dest, path);
	}

	stringstream ss;
	ss << "The total distance covered was: " << totalDistance << " meters.";
	instructions.push_back(ss.str());

	if(path.size() != 0)
	{
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
	}

	return instructions;
}

pair<long, vector<string> > Map::getInstructions(const Node &source, const Node &dest)
{
	vector<string> directions;
	Vertex<Node, Road>* v = graph.getVertex(dest);

	vector<Vertex<Node, Road>* > path;
	path.push_back(v);

	if(v->path == NULL)
		return make_pair(0, directions);

	while(v->path != NULL && !(v->path->getInfo() == source))
	{
		v = v->path;
		path.push_back(v);
	}

	if(v->path != NULL)
	{
		v = v->path;
		path.push_back(v);
	}

	string prevDir, currDir, tmpDir, direction;
	double dist = 0;

	currDir = getOrientation(path[path.size() - 1]->getInfo(), path[path.size() - 2]->getInfo());
	int change = path.size() - 1;

	for(size_t i = change; i > 0; i--)
	{
		prevDir = currDir;
		tmpDir = getOrientation(path[i]->getInfo(), path[i-1]->getInfo());

		if(tmpDir != "")
			currDir = tmpDir;

		size_t diff = currDir.find(prevDir);

		if(diff == string::npos)
		{
			direction = getNewDirection(prevDir, currDir);
			dist = path[i]->getDist() - path[change]->getDist();
			int distance = (int) (dist * 1000);
			stringstream ss;
			string strDist;
			ss << distance;
			ss >> strDist;
			string vDir = prevDir + " for " + strDist + " meters then turn " + direction + ".";
			directions.push_back(vDir);
			change = i;
		}
	}

	string finalDir;
	dist = path[0]->getDist() - path[change]->getDist();
	int distance = (int) (dist * 1000);
	stringstream ss;
	string strDist;
	ss << distance;
	ss >> strDist;
	finalDir = currDir + " for " + strDist + " meters.";
	directions.push_back(finalDir);
	ss.str("");

	dist = path[0]->getDist() - path[path.size() - 1]->getDist();
	distance = (int) (dist * 1000);

	return make_pair(distance, directions);
}

bool Map::getData(long long &originId, string &destination)
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
			cout << value << endl;
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
				if((originId = strtoll(value.c_str(), NULL, 10)) == 0LL)
					return false;
				break;
			case DESTINATION:
				destination = value;
				if(destination == "click/write node ID" || destination == "write street name")
					return false;
				break;
			case STREET:
				if(value == "false")
					street = false;
				else
					street = true;
				break;
			}
		}
		else return false;
	}
	return true;
}

void Map::fillPath(const Node &origin, const Node &dest, vector<long long> &path)
{
	long long currentId = dest.getId();
	Vertex<Map::Node, Map::Road> * currentVertex = graph.getVertex(*nodes.at(currentId));
	path.push_back(currentId);

	while ((currentVertex = currentVertex->path) != NULL && currentVertex->getInfo().getId() != origin.getId())
	{
		currentId = currentVertex->getInfo().getId();
		path.push_back(currentId);
	}

	path.push_back(origin.getId());
}

vector<string> Map::findNearestRoadName(string name)
{
	vector<string> nearest;
	long long id;
	int maxQ = 0;
	bool found = false;

	for(auto it = roads.begin(); it != roads.end(); ++it)
	{
		int q = KMPMatcher(it->second.getRoadName(), name);
		if(q > maxQ)
		{
			nearest.clear();
			maxQ = q;
		}
		if(q == maxQ )//&& !found)
		{
			if(!found)
				nearest.push_back(it->second.getRoadName());

			if(it->second.getRoadName() == name)
				found = true;
		}

	}
	return nearest;
}

vector<Map::Node> Map::findNodeByRoad(string name)
{
	set<long long> ids;

	for(auto it = roads.begin(); it != roads.end(); ++it)
	{
		if(it->second.getRoadName() == name)
		{
			for(auto it2 = subRoads.begin(); it2 != subRoads.end(); ++it2)
			{
				if(it2->second.getRoadId() == it->second.getId())
				{
					ids.insert(it2->second.getOriginId());
					ids.insert(it2->second.getDestId());
				}
			}
		}
	}


	vector<Node> roadNodes;
	for(auto it = ids.begin(); it != ids.end(); it++)
		roadNodes.push_back(*(nodes.at(*it)));

	return roadNodes;



}
