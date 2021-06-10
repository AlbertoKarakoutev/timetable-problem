/*
#include <cuda_runtime.h>
#include <vector>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include <device_launch_parameters.h>

#include "Utilities.h"
#include "Lecture.h"
#include "gVariables.h"

struct ModelData {
    vector<vector<vector<Lecture>>> solution;
    vector<int> solutionScore = { -1000, -1000, -1000 };
    string modelID;
};

static __global__ void evolveAndEvaluate(const vector<vector<vector<Lecture>>>* solution, ModelData* modelData){
    int tid = (blockIdx.x * blockDim.x) + threadIdx.x;

    modelData[tid].solutionScore = Utilities::evaluate(modelData[tid].solution);
    modelData[tid].solution = Utilities::evolve(solution[tid]);
    modelData[tid].modelID = Utilities::identify(modelData[tid].solution);

}

vector<ModelData> generate(vector<vector<vector<Lecture>>> solutionInput)
{

    ModelData result;
    vector<vector<vector<Lecture>>> solution = solutionInput;

    int solutionSize = sizeof(solution);
    
    int solutionSize_max = daysInWeek * periodInDay * 2 * roomCount * (5 * sizeof(int) + 3 * sizeof(string) + studentsPerClass * sizeof(int));
    int scoreSize = 3 * sizeof(int);
    int idSize = sizeof(string);
    int resultSize = solutionSize_max + scoreSize + idSize;

    vector<vector<vector<Lecture>>>* d_solution;
    ModelData* d_result;

    cudaMalloc(&d_solution, solutionSize);
    cudaMalloc(&d_result, resultSize);

    cudaMemcpy(&d_solution, &solution, solutionSize, cudaMemcpyHostToDevice);
    cudaMemcpy(&d_result, &result, resultSize, cudaMemcpyHostToDevice);

    int NUM_THREADS = 1 << 10;
    int NUM_BLOCKS = (solutionSize_max + NUM_THREADS - 1) / NUM_THREADS;

    evolveAndEvaluate<<<NUM_BLOCKS, NUM_THREADS>>>(d_solution, d_result);

    cudaMemcpy(&result, d_result, resultSize, cudaMemcpyDeviceToHost);

    vector<ModelData> results;

    results.push_back(result);

    return results;
}

__global__ vector<int> evaluate(vector<vector<vector<Lecture>>> schedule)
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

__global__ int duplicateCount(vector<vector<vector<Lecture>>> model, int targetDay, int targetPeriod)
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
        if (count(checkedCoursesForStudents.begin(), checkedCoursesForStudents.end(), lecture.getCourse().getName()) < 1) {
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

__global__ int courseInRange(vector<vector<vector<Lecture>>> model, Course course)
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

__global__ bool courseIsPreceeded(vector<vector<vector<Lecture>>> schedule, string courseName, string precedingCourseName)
{
    vector<vector<vector<Lecture>>> model = schedule;

    int courseEarliestOccurence[2] = { static_cast<int>(model.size()) + 1, static_cast<int>(model.at(0).size()) + 1 };
    int preceedingCourseEarliestOccurence[2] = { static_cast<int>(model.size()), static_cast<int>(model.at(0).size()) };

    for (int day = 0; day < model.size(); day++) {
        for (int period = 0; period < model[day].size(); period++) {
            for (int lecture = 0; lecture < model[day][period].size(); lecture++) {
                if (model[day][period][lecture].getCourse().getName().compare(
                        courseName)
                    == 0) {
                    courseEarliestOccurence[0] = day;
                    courseEarliestOccurence[1] = period;
                    break;
                }
            }
        }
    }

    for (int day = 0; day < model.size(); day++) {
        for (int period = 0; period < model[day].size(); period++) {
            for (int lecture = 0; lecture < model[day][period].size(); lecture++) {
                if (model[day][period][lecture].getCourse().getName().compare(
                        precedingCourseName)
                    == 0) {
                    preceedingCourseEarliestOccurence[0] = day;
                    preceedingCourseEarliestOccurence[1] = period;
                    break;
                }
            }
        }
    }

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

*/