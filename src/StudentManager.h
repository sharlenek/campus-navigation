#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cctype>

struct ClassInfo {
    int locationId;
    int startMinutes;
    int endMinutes;
};

struct Student {
    std::string name;
    std::string id;
    int residenceLocationId;
    std::unordered_set<std::string> classes;
};

class StudentManager {
private:
    std::unordered_map<std::string, Student> students;
    std::unordered_map<std::string, ClassInfo> classCatalog;

    bool isValidName(const std::string& name) const {
        if (name.empty()) return false;
        for (char c : name) {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ')) {
                return false;
            }
        }
        return true;
    }

    bool isValidId(const std::string& id) const {
        if (id.size() != 8) return false;
        for (char c : id) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }

public:
    void addClassInfo(const std::string& code, int locationId, int startMinutes, int endMinutes) {
        classCatalog[code] = {locationId, startMinutes, endMinutes};
    }

    bool classExists(const std::string& code) const {
        return classCatalog.find(code) != classCatalog.end();
    }

    const ClassInfo* getClassInfo(const std::string& code) const {
        auto it = classCatalog.find(code);
        return (it != classCatalog.end()) ? &it->second : nullptr;
    }

    bool insertStudent(const std::string& name, const std::string& id, int residenceId, const std::vector<std::string>& classCodes) {
        if (!isValidName(name) || !isValidId(id)) return false;
        if (students.find(id) != students.end()) return false;
        
        for (const auto& code : classCodes) {
            if (!classExists(code)) return false;
        }
        
        Student s;
        s.name = name;
        s.id = id;
        s.residenceLocationId = residenceId;
        s.classes.insert(classCodes.begin(), classCodes.end());
        students[id] = s;
        return true;
    }

    bool removeStudent(const std::string& id) {
        return students.erase(id) > 0;
    }

    bool dropClass(const std::string& id, const std::string& classCode) {
        auto it = students.find(id);
        if (it == students.end()) return false;
        if (it->second.classes.erase(classCode) == 0) return false;
        
        if (it->second.classes.empty()) {
            students.erase(it);
        }
        return true;
    }

    bool replaceClass(const std::string& id, const std::string& classFrom, const std::string& classTo) {
        auto it = students.find(id);
        if (it == students.end()) return false;
        if (!classExists(classTo)) return false;
        if (it->second.classes.find(classFrom) == it->second.classes.end()) return false;
        if (it->second.classes.find(classTo) != it->second.classes.end()) return false;
        
        it->second.classes.erase(classFrom);
        it->second.classes.insert(classTo);
        return true;
    }

    int removeClassFromAll(const std::string& classCode) {
        int count = 0;
        std::vector<std::string> toRemove;
        
        for (auto& kv : students) {
            if (kv.second.classes.erase(classCode) > 0) {
                count++;
                if (kv.second.classes.empty()) {
                    toRemove.push_back(kv.first);
                }
            }
        }
        
        for (const auto& id : toRemove) {
            students.erase(id);
        }
        
        return count;
    }

    const Student* getStudent(const std::string& id) const {
        auto it = students.find(id);
        return (it != students.end()) ? &it->second : nullptr;
    }

    std::vector<std::string> getSortedClasses(const std::string& id) const {
        std::vector<std::string> codes;
        auto it = students.find(id);
        if (it == students.end()) return codes;
        
        codes.insert(codes.end(), it->second.classes.begin(), it->second.classes.end());
        std::sort(codes.begin(), codes.end());
        return codes;
    }
};
