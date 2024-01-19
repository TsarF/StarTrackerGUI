#pragma once

#include <string>
#include "Eigen/Dense"

bool validateStringToDegrees(std::string str);
float stringToDegrees(std::string str);


struct CalibrationData_t
{
    bool isValid = false;
    Eigen::Vector3f star1;
    Eigen::Vector3f star2;
    Eigen::Vector3f star1Observed;
    Eigen::Vector3f star2Observed;
};
