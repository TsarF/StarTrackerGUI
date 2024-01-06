#include <cstdio>

#include "tashimiTaki.h"
#include "utils.h"

int main()
{
    Eigen::Matrix3f transform;
    ComputerPerfectTransform2Star(transform, 0.0f, 0.0f, d2rv2f({ 0.0f, 0.0f }), d2rv2f({ 0.0f, 0.0f }), 0.0f, d2rv2f({ 90.0f, 0.0f }), d2rv2f({ 90.0f, 0.0f }));
    printf("%s\n", Matrix3ToString(transform).c_str());

    ComputerPerfectTransform2Star(transform, 0.0f, 0.0f, d2rv2f({ 0.0f, 0.0f }), d2rv2f({ 90.0f, 0.0f }), 0.0f, d2rv2f({ 90.0f, 0.0f }), d2rv2f({ 90.0f, 90.0f }));
    printf("%s\n", Matrix3ToString(transform).c_str());

    printf("%s\n", Vector3ToString(r2dv3f(transform * d2rv3f(Eigen::Vector3f(90.0f, 90.0f, 0.0f)))).c_str());
}