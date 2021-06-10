# Introduction

A C++ console containing an optimization algorithm, designed to calculate an optimal school week. It works on the principals of a genetic algorithm, aiming to avoid any conflicts described by the following criteria:
 - No student should have more than one class in any period
 - No teacher should teach more than one class in any period
 - No room should be used more than once in any period
 - No class should be taught more than once in any period
 - Each course should be preceded by all the courses in their ```precedingCourseNames``` array
 - Each course should be taught a number of times hat is in the weekly threshold between ```minPeriods``` and ```maxPeriods```

## Structure
  
The application describes the following classes and their attributes:  
 
### Course.h 
 - Contains the descriptors for a course, including it's name, the period interval, the attending students and the preceding courses;

### Lecture.h 
 - Contains all the lecture properties, including it's time of day, teacher and room. 


### gConstants.h
 - Contains all the global constants that define the algorithm properties.
 
### gVariables.h
 - Contains all the globally accessible variables for the algorithm.

### Utilities.h
 - Contains all the utility methods, responsible for the proper execution of the algorithm. The main logic behind the program and calculations.


## Lifecycle

The program initializes the work week with all the days and periods, containing Lecture objects in a 2D ```vector```. The initial evaluation is completed and the optimization begins in the ```while``` loop, which breaks when the score of the solution is maximized (0). The algorithm generates n number of new solutions by forking into n threads (n being the ```parallelisation``` parameter). Each  The proposal by each thread is a ```struct ModelData```, containing the solution model mutated from the previous best, the score and a model identifier. When all results are produced by the ```vector<ModelData> generate()``` method, they are returned for checking. The top performer is selected out of all the proposals. If it is higher than the highest globally up to this point, it is set as the highest. 

## Mutation
The mutation is executed by following a set of ordered rules:

 - Initially the course ranges are optimized by adding or subtracting lectures if occurrences for a course are out of range.
 - Secondly, the course precedings are optimized by swapping lectures around the model.
 - Finally, the duplicate occurrences are optimized by switching repeating lectures if there are any or random ones in other cases.

## Evaluation
The evaluation is based on subtracting 1 point for each violation of the listed in the introduction section.

### !Note

We experimented with storing all the failed solutions by compressing their values in lists, but that just slowed down the program exponentially as time moved on.

