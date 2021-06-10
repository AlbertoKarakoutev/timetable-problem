#include "Utilities.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include "Course.h"
#include "gConstants.h"
#include "gVariables.h"
#include <pplwin.h>

using std::cout;
using std::count;
using std::string;
using std::to_string;

std::random_device rd;
std::mt19937 generator(rd());
std::uniform_int_distribution<> rnd(0, 9999);

//Returns the number of duplicates (teachers, rooms, students or courses) in a given day and period
int Utilities::duplicateCount(vector<vector<vector<Lecture>>> model, int targetDay, int targetPeriod)
{
    int hits = 0;

    vector<Lecture> period = model.at(targetDay).at(targetPeriod);

    vector<string> periodTeachers;
    vector<int> periodRooms;
    vector<Course> periodCourses;
    vector<int> periodStudents;

    vector<string> checkedCoursesForStudents;

    for (Lecture lecture : period) {
        periodTeachers.push_back(lecture.getTeacher());
        periodRooms.push_back(lecture.getRoom());
        periodCourses.push_back(lecture.getCourse());
        int studentsSize = static_cast<int>(lecture.getCourse().getStudents().size());
        if (count(checkedCoursesForStudents.begin(), checkedCoursesForStudents.end(), lecture.getCourse().getName()) < 1){
            vector<int> lStudents = lecture.getCourse().getStudents();
            periodStudents.insert(periodStudents.end(), lStudents.begin(), lStudents.end());
            checkedCoursesForStudents.push_back(lecture.getCourse().getName());
        }
    }

    vector<string> checkedTeachers;
    for (string teacher : periodTeachers) {
        if (count(checkedTeachers.begin(), checkedTeachers.end(), teacher) > 0)
            continue;
        if (count(periodTeachers.begin(), periodTeachers.end(), teacher) > 1) {
            checkedTeachers.push_back(teacher);
            hits++;
        }
    }

    vector<int> checkedRooms;
    for (int room : periodRooms) {
        if (count(checkedRooms.begin(), checkedRooms.end(), room) > 0)
            continue;
        if (count(periodRooms.begin(), periodRooms.end(), room) > 1) {
            checkedRooms.push_back(room);
            hits++;
        }
    }

    vector<Course> checkedCourses;
    for (int i = 0; i < periodCourses.size(); i++) { 
        Course course = periodCourses.at(i);
        if (count(checkedCourses.begin(), checkedCourses.end(), course) > 0)
            continue;
        if (count(periodCourses.begin(), periodCourses.end(), course) > 1) {
            checkedCourses.push_back(course);
            hits++;
        }
    }

    vector<int> checkedStudents;
    for (int student : periodStudents) {
        if (count(checkedStudents.begin(), checkedStudents.end(), student) > 0)
            continue;
        if (count(periodStudents.begin(), periodStudents.end(), student) > 1) {
            checkedStudents.push_back(student);
            hits++;
        }
    }

    return hits;
}

//Checks whether a course in it's appropriate weekly range
int Utilities::courseInRange(vector<vector<vector<Lecture>>> model, Course course)
{
    int hits = 0;

    for (vector<vector<Lecture>> day : model) {
        for (vector<Lecture> period : day) {
            for (Lecture lecture : period) {
                if (lecture.getCourse() == course) {
                    hits++;
                }
            }
        }
    }
    if (hits < course.getMinPeriods()) {
        return -1;
    }
    if (hits > course.getMaxPeriods()) {
        return 1;
    }
    return 0;
}

//Evaluates the model based on the violations it has
vector<int> Utilities::evaluate(vector<vector<vector<Lecture>>> schedule)
{
    vector<vector<vector<Lecture>>> model = schedule;

    int repeat = 0;
    int coursesNotPreceding = 0;

    int coursesNotInRange = 0;

    for (int day_index = 0; day_index < model.size(); day_index++) {
        for (int period_index = 0; period_index < model[day_index].size(); period_index++) {
            repeat -= duplicateCount(model, day_index, period_index);
        }
    }

    for (int i = 0; i < cLength; i++) {
        Course course = *getCourse(i);
        if (courseInRange(model, course) != 0) {
            coursesNotInRange--;
        }
        for (string precedingCourse : course.getPrecedingCourseNames()) {
            if (precedingCourse.compare("") != 0) {
                if (!courseIsPreceeded(model, course.getName(), precedingCourse)) {
                    coursesNotPreceding--;
                }
            }
        }
    }

    vector<int> score = { repeat, coursesNotInRange, coursesNotPreceding };
    return score;
}

//Displays the supplied model in a verbose manner
void Utilities::displayModel(vector<vector<vector<Lecture>>> model)
{
    for (int day_index = 0; day_index < model.size(); day_index++) {
        cout << "\n\033[96m--------------------------------------------------\n";
        cout << "\033[96mDay: " << day_index << "\n";

        for (int period_index = 0; period_index < model[day_index].size();
             period_index++) {
            cout << "\t\033[96mPeriod: " << period_index << "\n";
            for (Lecture lecture : model[day_index][period_index]) {
                cout << lecture.info();
            }

            cout << "\n\t\t\033[93m" + to_string(duplicateCount(model, day_index, period_index)) + " repeat(s)\n";
            cout << " ";
        }
    }
    cout << "\n";
    for (int i = 0; i < cLength; i++) {
        Course course = *getCourse(i);

        if (courseInRange(model, course) != 0) {
            cout << "\t" + course.getName() + " is not in range [" << course.getMinPeriods() << ", " << course.getMaxPeriods() << "]\n";
        }

        for (string precedingCourse : course.getPrecedingCourseNames()) {
            if (precedingCourse.compare("") != 0) {
                if (!courseIsPreceeded(model, course.getName(), precedingCourse)) {
                    cout << "\033[91m" + course.getName() + " is not preceeded by " + precedingCourse
                         << "\n";
                }
            }
        }
    }

    vector<int> modelScore = evaluate(model);

    displayScore(modelScore);

    //cout << "\nModel ID: " + identify(model) + "\n";

    cout << "\n\033[96m--------------------------------------------------\n";
}

//Displays the model score in a verbose manner
void Utilities::displayScore(vector<int> score)
{
    int sum = 0;
    for (int i = 0; i < 3; i++) {
        sum += score[i];
    }
    cout << "\n\033[92mScore: " << sum;
    cout << "\n\tRepeat - " + to_string(score[0]);
    cout << "\n\tCourses not in range - " + to_string(score[1]);
    cout << "\n\tCourses not preceding - " + to_string(score[2]) << "\n";
}

//Detects the first occurence of a course in the week
vector<int> Utilities::findEarliestOccurence(vector<vector<vector<Lecture>>> model, string courseName){

    vector<int> courseEarliestOccurence = { static_cast<int>(model.size()) + 1, static_cast<int>(model.at(0).size()) + 1, roomCount + 1 };

    for (int day = 0; day < model.size(); day++) {
        for (int period = 0; period < model[day].size(); period++) {
            for (int lecture = 0; lecture < model[day][period].size(); lecture++) {
                if (model[day][period][lecture].getCourse().getName().compare(
                        courseName)
                    == 0) {
                    courseEarliestOccurence[0] = day;
                    courseEarliestOccurence[1] = period;
                    courseEarliestOccurence[2] = lecture;
                    goto ready;
                }
            }
        }
    }
    ready:
    return courseEarliestOccurence;
}

//Checks whether a course is preceeded by the supplied course in the week
bool Utilities::courseIsPreceeded(vector<vector<vector<Lecture>>> schedule, string courseName, string precedingCourseName){
    vector<vector<vector<Lecture>>> model = schedule;

    vector<int> courseEarliestOccurence = findEarliestOccurence(model, courseName);
    vector<int> preceedingCourseEarliestOccurence = findEarliestOccurence(model, precedingCourseName);

    if (courseEarliestOccurence[0] != model.size() + 1 && courseEarliestOccurence[1] != model.at(0).size() + 1) {
        if (courseEarliestOccurence[0] > preceedingCourseEarliestOccurence[0]) {
            return true;
        } else {
            if (courseEarliestOccurence[1] > preceedingCourseEarliestOccurence[1]) {
                return true;
            }
        }
    }
    return false;
}

//Gets all the lectures that are violating the repeatence criteria
vector<Lecture> Utilities::loadRepeatingLectures(vector<vector<vector<Lecture>>> model)
{

    vector<Lecture> repeatingLectures;

    for (int day_index = 0; day_index < model.size(); day_index++) {
        for (int period_index = 0; period_index < model[day_index].size(); period_index++) {
            vector<Lecture> period = model.at(day_index).at(period_index);

            vector<string> periodTeachers;
            vector<int> periodRooms;
            vector<Course> periodCourses;
            vector<int> periodStudents;

            for (Lecture lecture : period) {
                periodTeachers.push_back(lecture.getTeacher());
                periodRooms.push_back(lecture.getRoom());
                periodCourses.push_back(lecture.getCourse());
                for (int i = 0; i < studentsPerClass; i++) {
                    periodStudents.push_back(lecture.getCourse().getStudents().at(i));
                }
            }

            for (string teacher : periodTeachers) {
                if (count(periodTeachers.begin(), periodTeachers.end(), teacher) > 1) {
                    for (Lecture lecture : period) {
                        if (count(repeatingLectures.begin(), repeatingLectures.end(), lecture) < 1) {
                            if (lecture.getTeacher().compare(teacher) == 0) {
                                repeatingLectures.push_back(lecture);
                            }
                        }
                    }
                }
            }

            for (int room : periodRooms) {
                if (count(periodRooms.begin(), periodRooms.end(), room) > 1) {
                    for (Lecture lecture : period) {
                        if (count(repeatingLectures.begin(), repeatingLectures.end(), lecture) < 1) {
                            if (lecture.getRoom() == room) {
                                repeatingLectures.push_back(lecture);
                            }
                        }
                    }
                }
            }

            for (Course course : periodCourses) {
                if (count(periodCourses.begin(), periodCourses.end(), course) > 1) {
                    for (Lecture lecture : period) {
                        if (lecture.getCourse() == course) {
                            if (count(repeatingLectures.begin(), repeatingLectures.end(), lecture) < 1) {
                                repeatingLectures.push_back(lecture);
                            }
                        }
                    }
                }
            }

            for (int student : periodStudents) {
                if (count(periodStudents.begin(), periodStudents.end(), student) > 1) {
                    for (Lecture lecture : period) {
                        vector<int> lectureStudents = lecture.getCourse().getStudents();
                        if (count(std::begin(lectureStudents), std::end(lectureStudents), student) > 1) {
                            if (count(repeatingLectures.begin(), repeatingLectures.end(), lecture) < 2) {
                                repeatingLectures.push_back(lecture);
                            }
                        }
                    }
                }
            }
        }
    }

    return repeatingLectures;
}

//Creates a 'day' 2D vector
vector<vector<Lecture>> Utilities::createDay(int day_index){

    vector<vector<Lecture>> periodsInThisDay;
    for (int i = 0; i < periodInDay; i++) {
        vector<Lecture> lectures;
        int maxLectures = rnd(generator) % (roomCount - 1);

        for (int j = 0; j < maxLectures; j++) {
            int courseIndex = rnd(generator) % (cLength - 1);
            Course randomCourse = *getCourse(courseIndex);

            int randomRoom = rnd(generator) % (roomCount - 1);

            int teacherIndex = rnd(generator) % (tLength - 1);
            string randomTeacher = allTeachers[teacherIndex];

            Lecture lecture = Lecture(randomCourse, randomRoom, randomTeacher, day_index, i);
            lectures.push_back(lecture);
        }
        periodsInThisDay.push_back(lectures);
    }
    return periodsInThisDay;
}

//Evolves the model with three ordered strategies, desidged to optimise different aspects of it.
vector<vector<vector<Lecture>>> Utilities::evolve(vector<vector<vector<Lecture>>> schedule){

    vector<vector<vector<Lecture>>> model = schedule;
    vector<int> modelScore = evaluate(model);

    if (modelScore[1] < 0) return optimizeCourseRanges(model);

    if (modelScore[2] < 0) return optimizeCoursePreceedings(model);

    return optimizeRepeats(model);
}

//Swaps 2 lecturec in the model
vector<vector<vector<Lecture>>> Utilities::switchLectures(vector<vector<vector<Lecture>>> model, Lecture l1, Lecture l2)
{

    vector<int> aTime = l1.getTime();
    vector<int> bTime = l2.getTime();

    for (int i = 0; i < model[aTime[0]][aTime[1]].size(); i++) {
        Lecture lec = model[aTime[0]][aTime[1]][i];
        if (lec == l1) {
            model[aTime[0]][aTime[1]].erase(model[aTime[0]][aTime[1]].begin() + i);
            break;
        }
    }
    for (int i = 0; i < model[bTime[0]][bTime[1]].size(); i++) {
        Lecture lec = model[bTime[0]][bTime[1]][i];
        if (lec == l2) {
            model[bTime[0]][bTime[1]].erase(model[bTime[0]][bTime[1]].begin() + i);
            break;
        }
    }

    l1.setTime(bTime);
    l2.setTime(aTime);

    model[aTime[0]][aTime[1]].push_back(l2);
    model[bTime[0]][bTime[1]].push_back(l1);

    return model;
}

//Optimizes the model's repeats by swapping lectures from the repeating list
vector<vector<vector<Lecture>>> Utilities::optimizeRepeats(vector<vector<vector<Lecture>>> model) {
    
    vector<Lecture> repeatingLectures = loadRepeatingLectures(model);

    if (repeatingLectures.size() < 1)
        return model;

    int lecture1Index = rnd(generator) % (static_cast<int>(repeatingLectures.size() - 1));
    Lecture a = repeatingLectures.at(lecture1Index);

    vector<vector<int>> emptyPeriods;
    for (int i = 0; i < model.size(); i++) {
        for (int j = 0; j < model[i].size(); j++) {
            if (model[i][j].size() < 1) {
                emptyPeriods.push_back({ i, j });
            }
        }
    }

    if (static_cast<int>(emptyPeriods.size()) < 1) {

        int day_index = rnd(generator) % (model.size() - 1);
        int period_index = rnd(generator) % (model[day_index].size() - 1);

        int lecture2Index = rnd(generator) % (model[day_index][period_index].size());
        Lecture b = model[day_index][period_index].at(lecture2Index);

        model = switchLectures(model, a, b);

    } else {

        vector<int> aTimeOld = a.getTime();

        for (int i = 0; i < model[aTimeOld[0]][aTimeOld[1]].size(); i++) {
            Lecture lec = model[aTimeOld[0]][aTimeOld[1]][i];
            if (lec == a) {
                model[aTimeOld[0]][aTimeOld[1]].erase(model[aTimeOld[0]][aTimeOld[1]].begin() + i);
                break;
            }
        }
        int period_index = 0;
        if (static_cast<int>(emptyPeriods.size() > 1)) {
            period_index = rnd(generator) % (static_cast<int>(emptyPeriods.size() - 1));
        }
        vector<int> newTime = emptyPeriods.at(period_index);

        a.setTime({ newTime[0], newTime[1] });

        model[newTime[0]][newTime[1]].push_back(a);
    }

    return model;
}

//Optimizes the course ranges are by adding or subtracting lectures if occurrences for a course are out of range.
vector<vector<vector<Lecture>>> Utilities::optimizeCourseRanges(vector<vector<vector<Lecture>>> model){
    vector<int> ranges;
    for (int i = 0; i < cLength; i++) {
        Course course = *getCourse(i);
        ranges.push_back(courseInRange(model, course));
    }

    for (int i = 0; i < cLength; i++) {
        Course course = *getCourse(i);

        if (ranges[i] < 0) {
            vector<vector<int>> emptyPeriods;
            for (int i = 0; i < model.size(); i++) {
                for (int j = 0; j < model[i].size(); j++) {
                    if (model[i][j].size() < 1) {
                        emptyPeriods.push_back({ i, j });
                    }
                }
            }


            int randomDay;
            int randomPeriod;
            if (emptyPeriods.size() > 1) {
                vector<int> empty = emptyPeriods.at(rnd(generator) % (emptyPeriods.size() - 1));
                randomDay = empty[0];
                randomPeriod = empty[1];
            } else if (emptyPeriods.size() ==  1) {
                randomDay = emptyPeriods.at(0)[0];
                randomPeriod = emptyPeriods.at(0)[1];
            } else {
                randomDay = rnd(generator) % (model.size() - 1);
                randomPeriod = rnd(generator) % (model[randomDay].size() - 1);
            }

            int newRoom = rnd(generator) % (roomCount - 1);
            string newTeacher = allTeachers[rnd(generator) % (tLength - 1)];
            model[randomDay][randomPeriod].push_back(Lecture(course, newRoom, newTeacher, randomDay, randomPeriod));

            
        } 
        if (ranges[i] > 0) {

            vector<Lecture> period;
            for (int j = 0; j < model.size(); j++) {
                bool completed = false;
                for (int k = 0; k < model[j].size(); k++) {
                    vector<Lecture> dayPeriod = model[j][k];
                    for (int w = 0; w < model[j][k].size(); w++) {
                        if (model[j][k][w].getCourse() == course) {

                            model[j][k].erase(model[j][k].begin() + w);

                            break;
                        }
                    }
                    if (completed)break;
                }
                if (completed)break;
            }
        }
    }

    return model;
}

//Optimizes the course precedings by swapping lectures that are not preceded correctly around the model.
vector<vector<vector<Lecture>>> Utilities::optimizeCoursePreceedings(vector<vector<vector<Lecture>>> schedule) {
    
    vector<vector<vector<Lecture>>> model = schedule;

    for (int i = 0; i < cLength; i++) {
        Course course = *getCourse(i);

        if (course.getPrecedingCourseNames()[0].compare("") == 0) continue;
        
        if (courseIsPreceeded(model, course.getName(), course.getPrecedingCourseNames()[0])) continue;

        vector<int> cfo = findEarliestOccurence(model, course.getName());
        vector<int> pcfo = findEarliestOccurence(model, course.getPrecedingCourseNames()[0]);

        if (cfo[0] == pcfo[0] && cfo[1] == pcfo[1]) {
            model[pcfo[0]][pcfo[1]].erase(model[pcfo[0]][pcfo[1]].begin() + pcfo[2]);
            continue;
        }

        if (cfo[0] > model.size() || pcfo[0] > model.size()) {
            return optimizeCourseRanges(model);
        } else {
            if (cfo[1] > model[cfo[0]].size() || pcfo[1] > model[pcfo[0]].size()) {
                return optimizeCourseRanges(model);
            }
        
        }

        Lecture l1 = model[cfo[0]][cfo[1]][cfo[2]];
        Lecture l2 = model[pcfo[0]][pcfo[1]][pcfo[2]];

        model = switchLectures(model, l1, l2);

        vector<int> score = evaluate(model);
        score = evaluate(model);
    }

    return model;
}

//Creates a course object
Course Utilities::createCourse(string courseName){

    int pcIndex = rnd(generator) % (cLength - 1);
    int pIndex = rnd(generator) % 100;

    vector<string> preceding;

    preceding.push_back(allCourseNames[pcIndex]);
    if (pIndex > 50) {
        preceding.clear();
        preceding.push_back("");
    }
    while (preceding[0].compare(courseName) == 0) {
        pcIndex = rnd(generator) % (cLength - 1);
        preceding = { allCourseNames[pcIndex] };
    }

    vector<int> classStudents;
    int counter = 0;
    while (counter < studentsPerClass) {
        int sIndex = rnd(generator) % (studentCount - 1);
        if (count(classStudents.begin(), classStudents.end(), sIndex) < 1) {
            classStudents.push_back(sIndex);
            counter++;
        }
    }

    int rangeMin =        1 + rnd(generator) % (daysInWeek);
    int rangeMax = rangeMin + rnd(generator) % (daysInWeek*2 - rangeMin + 1);

    Course courseObj = Course(courseName, rangeMin, rangeMax, preceding, classStudents);
    return courseObj;
}

//Creates a unique string identifier for each model
string Utilities::identify(vector<vector<vector<Lecture>>> schedule)
{
    vector<vector<vector<Lecture>>> model = schedule;
    string modelID = "";
    for (int day_index = 0; day_index < model.size(); day_index++) {
        modelID += "d";
        for (int period_index = 0; period_index < model[day_index].size();
             period_index++) {
            modelID += "p";
            for (int lecture_index = 0;
                 lecture_index < model[day_index][period_index].size();
                 lecture_index++) {
                Lecture lecture = model[day_index][period_index][lecture_index];
                modelID += "r" + to_string(lecture.getRoom());

                auto teacherIndex = find(allTeachers, allTeachers + tLength, lecture.getTeacher());
                modelID += "t" + to_string(distance(allTeachers, teacherIndex));

                auto courseIndex = find(allCourseNames, allCourseNames + cLength,
                    lecture.getCourse().getName());
                modelID += "c" + to_string(distance(allCourseNames, courseIndex));
            }
        }
    }
    return modelID;
}

