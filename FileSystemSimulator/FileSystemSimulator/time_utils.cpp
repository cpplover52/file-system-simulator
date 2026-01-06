#define _CRT_SECURE_NO_WARNINGS
#include "time_utils.h"
#include <ctime>
#include <string>
#include <iostream>
std::string calculateFullTime() {
    std::time_t t = time(NULL);
    std::tm* now = std::localtime(&t);
    int year = now->tm_year + 1900;
    int month = now->tm_mon + 1;
    int day = now->tm_mday;
    int hour = now->tm_hour;
    int min = now->tm_min;
    int dayIndexOfWeek = now->tm_wday;
    std::string monthName, dayOfWeek;
    switch (month) {
    case 1:
        monthName = "Ocak";
        break;
    case 2:
        monthName = "Subat";
        break;
    case 3:
        monthName = "Mart";
        break;
    case 4:
        monthName = "Nisan";
        break;
    case 5:
        monthName = "Mayis";
        break;
    case 6:
        monthName = "Haziran";
        break;
    case 7:
        monthName = "Temmuz";
        break;
    case 8:
        monthName = "Agustos";
        break;
    case 9:
        monthName = "Eylul";
        break;
    case 10:
        monthName = "Ekim";
        break;
    case 11:
        monthName = "Kasým";
        break;
    case 12:
        monthName = "Aralik";
        break;
    default:
        break;
    }
    switch (dayIndexOfWeek) {
    case 0:
        dayOfWeek = "Pazartesi";
        break;
    case 1:
        dayOfWeek = "Sali";
        break;
    case 2:
        dayOfWeek = "Carsamba";
        break;
    case 3:
        dayOfWeek = "Persembe";
        break;
    case 4:
        dayOfWeek = "Cuma";
        break;
    case 5:
        dayOfWeek = "Cumartesi";
        break;
    case 6:
        dayOfWeek = "Pazar";
        break;
    }
    std::string fullTime = std::to_string(now->tm_mday) + " " +
        monthName + " " +
        std::to_string(now->tm_year + 1900) + " " +
        dayOfWeek + " " +
        std::to_string(now->tm_hour) + ":" +
        std::to_string(now->tm_min); // Ornek: 12 Ocak Pazartesi 14:00
    return fullTime;
}