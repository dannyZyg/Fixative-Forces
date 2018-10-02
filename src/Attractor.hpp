//
//  Attractor.hpp
//  magnets
//
//  Created by Danny on 6/4/18.
//
#pragma once

#ifndef Attractor_hpp
#define Attractor_hpp

#include <stdio.h>
#include "ofMain.h"
#include "Particle.hpp"

#endif /* Attractor_hpp */


class Attractor{
  
    public:
    Attractor(ofPoint pos, float s);
    
    //functions
    ofPoint attract(Particle p);
    
    //variables
    ofPoint position;
    float strength;
    float distance;
    float mass;
    float G;
    
    
    
    
    
};
