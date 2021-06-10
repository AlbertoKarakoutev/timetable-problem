#include "Course.h"

#include <string>
#include <vector>

using std::to_string;
using std::vector;

Course::Course(string name, int minPeriods, int maxPeriods, vector<string> precedingCourseNames, vector<int> students)
{
  _name = name;
  _minPeriods = minPeriods;
  _maxPeriods = maxPeriods;
  _precedingCourseNames = precedingCourseNames;
  _students = students;
}

Course::Course(const Course& other) {
  _name = other._name;
  _minPeriods = other._minPeriods;
  _maxPeriods = other._maxPeriods;
  _precedingCourseNames = other._precedingCourseNames;
  _students = other._students;
}

Course::Course() {
    _name = "default";
    _minPeriods = 0;
    _maxPeriods = 0;
}

string Course::info() {
  string info = "";
  info += _name + string(13 - _name.length(), ' ');
  info += "[" + to_string(_minPeriods) + ", " + to_string(_maxPeriods) + "]  ";

  int precedingLength = 0;
  if (_precedingCourseNames.size() > 0) {
    precedingLength += static_cast<int>(_precedingCourseNames[0].length());
  }
  int spaces = 11 - precedingLength;
  info += "[" + _precedingCourseNames[0] + string(spaces, ' ') + "] [";
  for (int i = 0; i < _students.size(); i++) {
    info += to_string(_students[i]) + " ";
  }
  info += "]\n";
  return info;
}

bool Course::operator==(const Course & rhs) const {
  return _name.compare(rhs._name) == 0;
}
