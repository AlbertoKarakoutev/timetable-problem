#pragma once
#include <string>
#include <vector>
#include <random>

#include "gConstants.h"
#include "Lecture.h"

using std::vector;

class Utilities{
public:
    static int duplicateCount(vector<vector<vector<Lecture>>> model, int targetDay, int targetPeriod);
    static int courseInRange(vector<vector<vector<Lecture>>> model, Course course);
    static vector<int> evaluate(vector<vector<vector<Lecture>>> schedule);
    static vector<Lecture> loadRepeatingLectures(vector<vector<vector<Lecture>>> model);
	static bool courseIsPreceeded(vector<vector<vector<Lecture>>> schedule, string courseName, string precedingCourseName);
    static vector<vector<vector<Lecture>>> evolve(vector<vector<vector<Lecture>>> schedule);
    static vector<vector<vector<Lecture>>> switchLectures(vector<vector<vector<Lecture>>> model, Lecture l1, Lecture l2);
    static vector<vector<vector<Lecture>>> optimizeRepeats(vector<vector<vector<Lecture>>> model);
    static vector<vector<vector<Lecture>>> optimizeCourseRanges(vector<vector<vector<Lecture>>> model);
    static vector<vector<vector<Lecture>>> optimizeCoursePreceedings(vector<vector<vector<Lecture>>> model);
	static Course createCourse(std::string courseName);
    static vector<vector<Lecture>> createDay(int day_index);
    static void displayModel(vector<vector<vector<Lecture>>> schedule);
    static void displayScore(vector<int> score);
    static vector<int> findEarliestOccurence(vector<vector<vector<Lecture>>> model, string courseName);
	static string identify(vector<vector<vector<Lecture>>> schedule);


};

