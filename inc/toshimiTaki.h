#pragma once

#include "Eigen/Dense"

void ComputerPerfectTransform2Star(Eigen::Matrix3<float> &transform, float time0, float time1, Eigen::Vector2<float> star1, Eigen::Vector2<float> telescope1, 
float time2, Eigen::Vector2<float> star2, Eigen::Vector2<float> telescope2);
