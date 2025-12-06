#include <fstream>
#include <sstream>
#include <iostream>
#include "CampusCompass.h"

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

    // Parse classes file
    std::ifstream classes_file(classes_filepath);
    if(!classes_file.is_open()) {
        return false;
    }

    getline(classes_file, line); // skip header

    while(getline(classes_file, line)) {
        string classCode;
        string locIdStr;
        string startTimeStr;
        string endTimeStr;
        stringstream ss(line);

        getline(ss, classCode, ',');
        getline(ss, locIdStr, ',');
        getline(ss, startTimeStr, ',');
        getline(ss, endTimeStr, ',');

        int locationId = stoi(locIdStr);
        
        // Parse time HH:MM to minutes
        int startHour = stoi(startTimeStr.substr(0, 2));
        int startMin = stoi(startTimeStr.substr(3, 2));
        int startMinutes = startHour * 60 + startMin;
        
        int endHour = stoi(endTimeStr.substr(0, 2));
        int endMin = stoi(endTimeStr.substr(3, 2));
        int endMinutes = endHour * 60 + endMin;

        studentManager.addClassInfo(classCode, locationId, startMinutes, endMinutes);
    }

    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    stringstream ss(command);
    string cmd;
    ss >> cmd;
    
    if (cmd == "insert") {
        // insert "STUDENT_NAME" STUDENT_ID RESIDENCE_LOCATION_ID N CLASSCODE_1 ...
        string name;
        string id;
        int residence;
        int n;
        
        // Extract quoted name
        string temp;
        getline(ss, temp, '"'); // skip to first quote
        getline(ss, name, '"'); // get name between quotes
        
        ss >> id >> residence >> n;
        
        // Validate residence exists
        if (!campusGraph.hasLocation(residence)) {
            cout << "unsuccessful" << endl;
            return false;
        }
        
        vector<string> classes;
        for (int i = 0; i < n; i++) {
            string classCode;
            ss >> classCode;
            classes.push_back(classCode);
        }
        
        if (classes.size() != (size_t)n) {
            cout << "unsuccessful" << endl;
            return false;
        }
        
        bool success = studentManager.insertStudent(name, id, residence, classes);
        cout << (success ? "successful" : "unsuccessful") << endl;
        return success;
    }
    else if (cmd == "remove") {
        // remove STUDENT_ID
        string id;
        ss >> id;
        
        bool success = studentManager.removeStudent(id);
        cout << (success ? "successful" : "unsuccessful") << endl;
        return success;
    }
    else if (cmd == "dropClass") {
        // dropClass STUDENT_ID CLASSCODE
        string id, classCode;
        ss >> id >> classCode;
        
        if (!studentManager.classExists(classCode)) {
            cout << "unsuccessful" << endl;
            return false;
        }
        
        bool success = studentManager.dropClass(id, classCode);
        cout << (success ? "successful" : "unsuccessful") << endl;
        return success;
    }
    else if (cmd == "replaceClass") {
        // replaceClass STUDENT_ID CLASSCODE_1 CLASSCODE_2
        string id, classFrom, classTo;
        ss >> id >> classFrom >> classTo;
        
        bool success = studentManager.replaceClass(id, classFrom, classTo);
        cout << (success ? "successful" : "unsuccessful") << endl;
        return success;
    }
    else if (cmd == "removeClass") {
        // removeClass CLASSCODE
        string classCode;
        ss >> classCode;
        
        int count = studentManager.removeClassFromAll(classCode);
        cout << count << endl;
        return true;
    }
    else if (cmd == "toggleEdgesClosure") {
        // toggleEdgesClosure N LOCATION_ID_X LOCATION_ID_Y ...
        int n;
        ss >> n;
        
        for (int i = 0; i < n; i++) {
            int locX, locY;
            ss >> locX >> locY;
            campusGraph.toggleEdge(locX, locY);
        }
        
        cout << "successful" << endl;
        return true;
    }
    else if (cmd == "checkEdgeStatus") {
        // checkEdgeStatus LOCATION_ID_X LOCATION_ID_Y
        int locX, locY;
        ss >> locX >> locY;
        
        cout << campusGraph.edgeStatus(locX, locY) << endl;
        return true;
    }
    else if (cmd == "isConnected") {
        // isConnected LOCATION_ID_1 LOCATION_ID_2
        int loc1, loc2;
        ss >> loc1 >> loc2;
        
        bool connected = campusGraph.isConnected(loc1, loc2);
        cout << (connected ? "successful" : "unsuccessful") << endl;
        return connected;
    }
    else if (cmd == "printShortestEdges") {
        // printShortestEdges ID
        string id;
        ss >> id;
        
        const Student* student = studentManager.getStudent(id);
        if (!student) {
            cout << "unsuccessful" << endl;
            return false;
        }
        
        cout << "Name: " << student->name << endl;
        
        vector<string> sortedClasses = studentManager.getSortedClasses(id);
        for (const auto& classCode : sortedClasses) {
            const ClassInfo* classInfo = studentManager.getClassInfo(classCode);
            if (classInfo) {
                int distance = campusGraph.shortestPath(student->residenceLocationId, classInfo->locationId);
                cout << classCode << " | Total Time: " << distance << endl;
            }
        }
        
        return true;
    }
    else if (cmd == "printStudentZone") {
        // printStudentZone ID
        string id;
        ss >> id;
        
        const Student* student = studentManager.getStudent(id);
        if (!student) {
            cout << "unsuccessful" << endl;
            return false;
        }
        
        // Collect all vertices from shortest paths to all classes
        unordered_set<int> vertices;
        vector<string> sortedClasses = studentManager.getSortedClasses(id);
        
        for (const auto& classCode : sortedClasses) {
            const ClassInfo* classInfo = studentManager.getClassInfo(classCode);
            if (classInfo) {
                vector<int> route;
                int distance = campusGraph.shortestPathWithRoute(student->residenceLocationId, classInfo->locationId, route);
                
                if (distance >= 0) {
                    for (int vertex : route) {
                        vertices.insert(vertex);
                    }
                }
            }
        }
        
        int cost = campusGraph.mstCost(vertices);
        cout << "Student Zone Cost For " << student->name << ": " << cost << endl;
        return true;
    }
    else if (cmd == "verifySchedule") {
        // verifySchedule ID
        string id;
        ss >> id;
        
        const Student* student = studentManager.getStudent(id);
        if (!student) {
            cout << "unsuccessful" << endl;
            return false;
        }
        
        // Get all classes with their start times
        vector<pair<int, string>> classSchedule; // (startMinutes, classCode)
        for (const auto& classCode : student->classes) {
            const ClassInfo* classInfo = studentManager.getClassInfo(classCode);
            if (classInfo) {
                classSchedule.push_back({classInfo->startMinutes, classCode});
            }
        }
        
        if (classSchedule.size() < 2) {
            cout << "unsuccessful" << endl;
            return false;
        }
        
        // Sort by start time
        sort(classSchedule.begin(), classSchedule.end());
        
        cout << "Schedule Check for " << student->name << ":" << endl;
        
        for (size_t i = 0; i + 1 < classSchedule.size(); i++) {
            const string& class1 = classSchedule[i].second;
            const string& class2 = classSchedule[i + 1].second;
            
            const ClassInfo* info1 = studentManager.getClassInfo(class1);
            const ClassInfo* info2 = studentManager.getClassInfo(class2);
            
            if (info1 && info2) {
                int timeGap = info2->startMinutes - info1->endMinutes;
                int travelTime = campusGraph.shortestPath(info1->locationId, info2->locationId);
                
                bool canMakeIt = (travelTime >= 0 && timeGap >= travelTime);
                cout << class1 << " - " << class2 << " \"" << (canMakeIt ? "Can make it!" : "Cannot make it!") << "\"" << endl;
            }
        }
        
        return true;
    }
    
    cout << "unsuccessful" << endl;
    return false;
}
