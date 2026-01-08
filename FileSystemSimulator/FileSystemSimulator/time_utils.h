#pragma once
#include <ctime>
#include <iostream>
#include <string>
using std::string;

class FullTime{
public:
	int day;
	int month;
	int year;
	int hour;
	int min;
	string dayOfWeek;
	string monthName;
	FullTime();
	void setFullTime(); 
	std::string getFullTimeStr();
};

FullTime getCurrentTime();

