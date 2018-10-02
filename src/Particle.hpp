//
//  Particle.hpp
//  magnets
//
//  Created by Danny on 6/4/18.
//


#pragma once

#ifndef Particle_hpp
#define Particle_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ParameterSmoother.hpp"

#endif /* Particle_hpp */



class Particle{
    
    public:
        Particle();
    
    
    //member variables
    
    vector<ofPoint> shapePoints;
    ofPoint position, velocity, acceleration, friction;
    float cF;
    float distMult, mass, angle;
    float radius;
    float maxSpeed, maxForce;
    float flowX, flowY;
    float randomFlowOffset;
    float randomOffset;
    float randomLerpOffset;
    float flowStep;
    ofColor c;
    smoothValue smoothedFlow;

    //member functions
    
    void update();
    void draw();
    void checkEdges();
    void applyForce(ofPoint force);
    void accelerateTowardsTarget(ofVec3f target);
    void getAngle(float a);
    void receiveTarget(ofPoint target);
    void receiveFlow(float x, float y);

 

    };
