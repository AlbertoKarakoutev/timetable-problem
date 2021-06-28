#pragma once

#include <vector>

#include "gConstants.h"

using std::string;
using std::vector;

class Course {
public:
    Course(string name, int minPeriods, int maxPeriods, vector<string> precedingCourseNames, vector<int> students);
    Course(const Course& other);
    Course();

    vector<int> getStudents() { return _students; };
    int getMinPeriods() { return _minPeriods; };
    int getMaxPeriods() { return _maxPeriods; };

    string getName() { return _name; };
    vector<string> getPrecedingCourseNames() { return _precedingCourseNames; };
    string info();
    bool operator==(const Course & rhs) const;
    string _name;

private:
    int _minPeriods;
    int _maxPeriods;
    vector<string> _precedingCourseNames;
    vector<int> _students;
};
