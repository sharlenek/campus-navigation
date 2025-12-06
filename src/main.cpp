#include <iostream>

#include "CampusCompass.h"

using namespace std;

int main() {
    CampusCompass compass;

    // Try multiple possible paths for data files
    if (!compass.ParseCSV("data/edges.csv", "data/classes.csv")) {
        if (!compass.ParseCSV("../data/edges.csv", "../data/classes.csv")) {
            compass.ParseCSV("./data/edges.csv", "./data/classes.csv");
        }
    }

    string command;
    getline(cin, command);
    int num_of_lines = stoi(command);
    for(int i = 0; i < num_of_lines; i++){
        // Parse commands
        getline(cin, command);
        compass.ParseCommand(command);
    }

}
