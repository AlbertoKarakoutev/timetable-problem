#pragma once

#include <vector>
#include <string>
#include <list>
#include <numeric>

using std::vector;
using std::string;

#ifndef GLOBAL_CONSTS
#define GLOBAL_CONSTS

static const int daysInWeek = 5;
static const int periodInDay = 8;
static const int studentsPerClass = 10;
static const int roomCount = 10;
static const int studentCount = studentsPerClass * roomCount * 4;

static const int parallelisation = 8;

static const string allTeachers[] = { "Ivanov", "Georgieva", "Petrov", "Tomova", "Velichkov", "Boneva", "Penev", "Pancheva", "Ganev", "Jivkova" };;
static const string allCourseNames[] = { "Math", "Geography", "History", "English", "Spanish", "PE", "Literature", "Biology", "Chemistry", "Physics" };;

static const int cLength = sizeof(allCourseNames) / sizeof(allCourseNames[0]);
static const int tLength = sizeof(allTeachers) / sizeof(allTeachers[0]);

static const int possibleLecturesCount = daysInWeek * periodInDay * roomCount * cLength * (sizeof(allTeachers) / sizeof(allTeachers[0]));

#endif
