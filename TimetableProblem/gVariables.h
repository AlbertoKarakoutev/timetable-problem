#pragma once

#include <vector>

#include "gConstants.h"
#include "Course.h"
#include "Lecture.h"

using std::vector;

#ifndef GLOBAL_VARS
#define GLOBAL_VARS


extern int solution_score[4];

static vector<int> allRooms(roomCount);
static vector<int> allStudents(studentCount);

static Course allCourses[cLength];

extern void setup();
void addRoomsAndStudents();
Course* getCourse(int index);

#endif