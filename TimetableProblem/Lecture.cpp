#include "Lecture.h"

Lecture::Lecture(Course course, int room, std::string teacher, int day, int period)
{
	_course = course;
	_room = room;
	_teacher = teacher;
	_day = day;
    _period = period;
}

Lecture::Lecture() {

	_day = 0;
    _period = 0;
    _room = 0;

}

void Lecture::setTime(vector<int> time) {
    _day = time.at(0);
    _period = time.at(1);
}

vector<int> Lecture::getTime() {
    return { _day, _period };
}

std::string Lecture::info()
{
	std::string info = "";
	info += "\t\tRoom " + std::to_string(_room) + ", ";
	info += _teacher + ", ";
	info += _course.getName() + " [ ";
	for (int i = 0; i < studentsPerClass; i++)
	{
		info += std::to_string(_course.getStudents()[i]) + " ";
	}
	info += "]\n";
	return info;
}

bool Lecture::operator==(const Lecture rhs) const {
    bool props = _course == rhs._course && _room == rhs._room && _teacher.compare(rhs._teacher) == 0;
    bool time = _day == rhs._day && _period == rhs._period;

	return props && time;
}
