#pragma once

#include "Course.h"

class Course;

class Lecture {
public:
    Lecture(Course course, int room, string teacher, int day, int period);
    Lecture();

    Course getCourse() { return _course; };
    int getRoom() { return _room; };
    string getTeacher() { return _teacher; };
    void setTime(vector<int> time);
    vector<int> getTime();
    string info();
    bool operator==(const Lecture rhs) const;

    Course _course;
    int _room;
    string _teacher;

    int _day;
    int _period;
};
