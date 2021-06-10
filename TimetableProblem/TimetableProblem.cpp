#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

#include "Course.h"
#include "Utilities.h"
#include "gConstants.h"
#include "gVariables.h"

using std::cout;
using std::future;
using std::thread;
using std::to_string;

vector<vector<vector<Lecture>>> solution;

struct ModelData {
    vector<vector<vector<Lecture>>> solution;
    vector<int> solutionScore = { -1000, -1000, -1000 };
    string modelID;
};

static ModelData evolveAndEvaluate(vector<vector<vector<Lecture>>> solution)
{
    ModelData thisModelData;

    thisModelData.solution = Utilities::evolve(solution);
    thisModelData.solutionScore = Utilities::evaluate(thisModelData.solution);
    //thisModelData.modelID = Utilities::identify(thisModelData.solution);

    return thisModelData;
}

vector<ModelData> generate(vector<vector<vector<Lecture>>> solution)
{
    int parallelisationCapped;
    if (parallelisation > std::thread::hardware_concurrency()) {
        parallelisationCapped = std::thread::hardware_concurrency();
    }
    else {
        parallelisationCapped = parallelisation;
    }

    vector<ModelData> results;
    if (parallelisationCapped == 1) {
        ModelData modelData = evolveAndEvaluate(solution);

        results.push_back(modelData);
    }
    else {
        while (results.size() < parallelisationCapped) {
            vector<future<ModelData>> futures;

            for (int i = static_cast<int>(results.size()) - 1; i < parallelisationCapped; i++) {
                futures.push_back(std::async(evolveAndEvaluate, solution));
            }

            for (int i = 0; i < futures.size(); i++) {
                futures[i].wait();
                ModelData result = futures[i].get();
                results.push_back(result);

            }
        }
    }

    return results;
}

int main()
{
    cout << "Maximum configurations: " << possibleLecturesCount;

    cout << "\nCOURSES:";
    cout << "\n\033[96mNAME         RANGE   PRECEDING        STUDENTS\n";
    setup();

    for (int day_index = 0; day_index < daysInWeek; day_index++) {
        vector<vector<Lecture>> newDay = Utilities::createDay(day_index);
        solution.push_back(newDay);
    }

    cout << "\n\033[96mINITIAL MODEL:";
    Utilities::displayModel(solution);
    cout << "\n\t\t\tOPTIMISING...\n";

    vector<int> solutionScore = Utilities::evaluate(solution);

    int consecutiveFails = 0;

    vector<string> invalidSolutions;

    while (solutionScore[0] + solutionScore[1] + solutionScore[2] < 0) {
        vector<ModelData> proposedSolutions = generate(solution);

        ModelData evolved;
        int currentMax = -50000;
        for (ModelData proposedSolution : proposedSolutions) {
            int modelScore = proposedSolution.solutionScore[0] + proposedSolution.solutionScore[1] + proposedSolution.solutionScore[2];
            if (modelScore > currentMax) {
                evolved = proposedSolution;
            }
        }

        //for (ModelData proposedSolution : proposedSolutions) {
        //    if (std::count(invalidSolutions.begin(), invalidSolutions.end(), proposedSolution.modelID) == 0) {
        //        invalidSolutions.push_back(proposedSolution.modelID);
        //    }
        //}

        int sum = evolved.solutionScore[0] + evolved.solutionScore[1] + evolved.solutionScore[2];
        if (sum > solutionScore[0] + solutionScore[1] + solutionScore[2]) {
            cout << "\n\n\033[92mImproved:";
            Utilities::displayScore(evolved.solutionScore);
            solutionScore = evolved.solutionScore;
            solution = evolved.solution;
            consecutiveFails = 0;
            cout << "\n";
        } else {
            //cout << "\033[91m\rDid not improve x" << consecutiveFails << ", tested " << invalidSolutions.size() << " solutions";
            cout << "\033[91m\rDid not improve x" << consecutiveFails;
            consecutiveFails += parallelisation;
        }
       if (invalidSolutions.size() >= possibleLecturesCount) {
           break;
       }
    }

    cout << "\nOPTIMISED MODEL:";
    Utilities::displayModel(solution);

    return 0;
}
