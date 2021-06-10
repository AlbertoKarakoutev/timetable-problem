#include "gVariables.h"

#include <iostream>

#include "Utilities.h"
#include "gConstants.h"

class Lecture;
class Course;

void setup() {
  addRoomsAndStudents();

  for (int i = 0; i < cLength; i++) {
    Course newCourse = Utilities::createCourse(allCourseNames[i]);
    allCourses[i] = newCourse;
    std::cout << allCourses[i].info();
  }
}

static void addRoomsAndStudents() {
  for (int i = 0; i < roomCount; i++) {
    allRooms[i] = i;
  }

  for (int i = 0; i < studentCount; i++) {
    allStudents[i] = i;
  }
}

Course* getCourse(int index) { return &allCourses[index]; }

