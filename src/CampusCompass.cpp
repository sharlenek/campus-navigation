#include <fstream>
#include <sstream>
#include "CampusCompass.h"

#include <string>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize your object
}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    std::ifstream edges_file(edges_filepath);
    if(!edges_file.is_open()) {
        return false; // failed to open edges file
    }

    string line;
    getline(edges_file, line); // skip header

    while(getline(edges_file, line)) {
        string locId1;
        string locId2;
        string name1;
        string name2;
        string time;
        stringstream ss(line);

        getline(ss, locId1, ',');   
        getline(ss, locId2, ',');
        getline(ss, name1, ',');
        getline(ss, name2, ',');
        getline(ss, time, ',');

        int id1 = stoi(locId1);
        int id2 = stoi(locId2);
        int travel_time = stoi(time);

        // populate graph with locations and edge
        campusGraph.addLocation(id1, name1);
        campusGraph.addLocation(id2, name2);
        campusGraph.addEdge(id1, id2, travel_time);
    }

    // return boolean based on whether parsing was successful or not
    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    // do whatever regex you need to parse validity
    // hint: return a boolean for validation when testing. For example:
    bool is_valid = true; // replace with your actual validity checking

    return is_valid;
}
