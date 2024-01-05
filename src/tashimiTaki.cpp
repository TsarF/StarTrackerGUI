#include "tashimiTaki.h"
#include "Eigen/Dense"

// time -> in units of clock angle: 12h = pi; 6h = pi/2; 3h = pi/4...
// transform is a matrix 
void ComputerPerfectTransform2Star(Eigen::Matrix3<float> &transform, float time0, float time1, Eigen::Vector2<float> star1, Eigen::Vector2<float> telescope1, 
float time2, Eigen::Vector2<float> star2, Eigen::Vector2<float> telescope2)
{ // X IS THE HORIZONTAL ANGLE / RIGHT ASCENTION // Y IS THE ELEVATION / DECLINATION
    float k = 1.002737908f; // SEE http://takitoshimi.starfree.jp/matr[0]/matr[0]_method_rev_e.pdf page 33

    Eigen::Matrix3<float> l;
    Eigen::Matrix3<float> L;

    l.col(0)[0] = cosf(telescope1[1]) * cosf(telescope1[0]);
    l.col(0)[1] = cosf(telescope1[1]) * sinf(telescope1[0]);
    l.col(0)[2] = sinf(telescope1[1]);

    L.col(0)[0] = cosf(star1[1]) * cosf( star1[0] - k * (time1 - time0) );
    L.col(0)[1] = cosf(star1[1]) * sinf( star1[0] - k * (time1 - time0) );
    L.col(0)[2] = sinf(star1[1]);

    l.col(1)[0] = cosf(star2[1]) * cosf(telescope2[0]);
    l.col(1)[1] = cosf(star2[1]) * sinf(telescope2[0]);
    l.col(1)[2] = sinf(telescope2[1]);

    L.col(1)[0] = cosf(star2[1]) * cosf(star2[0] - k * (time2 - time0));
    L.col(1)[1] = cosf(star2[1]) * sinf(star2[0] - k * (time2 - time0));
    L.col(1)[2] = sinf(star2[1]);

    float lnormalizedDirectionCosine = 1 / (sqrtf( 
    (l.col(0)[1] * l.col(1)[2] - l.col(0)[2] * l.col(1)[1]) * (l.col(0)[1] * l.col(1)[2] - l.col(0)[2] * l.col(1)[1]) + 
    (l.col(0)[2] * l.col(1)[0] - l.col(0)[0] * l.col(1)[2]) * (l.col(0)[2] * l.col(1)[0] - l.col(0)[0] * l.col(1)[2]) + 
    (l.col(0)[0] * l.col(1)[1] - l.col(0)[1] * l.col(1)[0]) * (l.col(0)[0] * l.col(1)[1] - l.col(0)[1] * l.col(1)[0])
    ));

    l.col(2)[0] = lnormalizedDirectionCosine * (l.col(0)[1] * l.col(1)[2] - l.col(0)[2] * l.col(1)[1]);
    l.col(2)[1] = lnormalizedDirectionCosine * (l.col(0)[2] * l.col(1)[0] - l.col(0)[0] * l.col(1)[2]);
    l.col(2)[2] = lnormalizedDirectionCosine * (l.col(0)[0] * l.col(1)[1] - l.col(0)[1] * l.col(1)[0]);

    float LnormalizedDirectionCosine = 1 / (sqrtf( 
    (L.col(0)[1] * L.col(1)[2] - L.col(0)[2] * L.col(1)[1]) * (L.col(0)[1] * L.col(1)[2] - L.col(0)[2] * L.col(1)[1]) + 
    (L.col(0)[2] * L.col(1)[0] - L.col(0)[0] * L.col(1)[2]) * (L.col(0)[2] * L.col(1)[0] - L.col(0)[0] * L.col(1)[2]) + 
    (L.col(0)[0] * L.col(1)[1] - L.col(0)[1] * L.col(1)[0]) * (L.col(0)[0] * L.col(1)[1] - L.col(0)[1] * L.col(1)[0])
    ));

    L.col(2)[0] = LnormalizedDirectionCosine * (L.col(0)[1] * L.col(1)[2] - L.col(0)[2] * L.col(1)[1]);
    L.col(2)[1] = LnormalizedDirectionCosine * (L.col(0)[2] * L.col(1)[0] - L.col(0)[0] * L.col(1)[2]);
    L.col(2)[2] = LnormalizedDirectionCosine * (L.col(0)[0] * L.col(1)[1] - L.col(0)[1] * L.col(1)[0]);

    transform = l * L.inverse();
}
