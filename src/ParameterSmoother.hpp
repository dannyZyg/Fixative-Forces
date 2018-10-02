//
//  ParameterSmoother.hpp
//  magnetsKinect
//
//  Created by Danny on 23/4/18.
//

// Parameter Smoother class - original code from https://www.youtube.com/watch?v=BdJRSqgEqPQ, adapted to accept ofPoint

#pragma once

#ifndef ParameterSmoother_hpp
#define ParameterSmoother_hpp

#include <stdio.h>
#include "ofMain.h"

#endif /* ParameterSmoother_hpp */


class ParameterSmoother{
    
public:
    ParameterSmoother(float smoothingTimeMS, float frameRate);

    inline ofPoint process(ofPoint input){
            z1 = (input.x * b ) + (z1 * a);
            z2 = (input.y * b ) + (z2 * a);
            ofPoint result(z1, z2);
            return result;
    }
    
private:
    float a, b, z1, z2;

};

struct smoothValue {
    ofPoint targetValue;
    ofPoint currentValue;
    ParameterSmoother* smoother;
};
