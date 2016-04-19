#include "Map.h"

using namespace std;



void Map::readInfo()
{

	ifstream in;
	readNodes(in);
	readRoads(in);
	readSubRoads(in);
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
	}

	in.close();
}

void Map::readSubRoads(ifstream &in)
{
	string read;
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
		//	cout << "road id: " << roadID << endl;
		getline(in, read, separator); // node1_id
		ss >> node1ID;
		ss.clear();
		//	cout << "node 1 id: " << node1ID << endl;
		getline(in, read, separator); // node2_id
		ss >> node2ID;
		ss.clear();
		//	cout << "node 2 id: " << node2ID << endl;
	}

	in.close();
}
