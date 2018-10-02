//
//  ParameterSmoother.cpp
//  magnetsKinect
//
//  Created by Danny on 23/4/18.
//
#include "ParameterSmoother.hpp"

ParameterSmoother::ParameterSmoother(float smoothingTimeMS, float frameRate){
    a = exp( - TWO_PI / smoothingTimeMS * 0.001 * frameRate);
    b = 1 - a;
    z1 = 0.0;
    z2 = 0.0;
}

