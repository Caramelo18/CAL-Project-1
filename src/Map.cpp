#include "Map.h"

using namespace std;



void Map::getInfo(){

	ifstream in;
	getNodes(in);
	getRoads(in);
	getSubRoads(in);
}

void Map::getNodes(ifstream &in){

	string read;
	long id;
	double latitude, longitude;
	char separator = ';';

	in.open("nodes.txt");

	while(!in.eof()){
		getline(in, read, separator);
		//id = strtol(read);
	}
}

void Map::getRoads(ifstream &in){

}

void Map::getSubRoads(ifstream &in){

}
