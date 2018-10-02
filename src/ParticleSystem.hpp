//
//  ParticleSystem.hpp
//  magnets
//
//  Created by Danny on 7/4/18.
//

#pragma once

#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <stdio.h>
#include "ofMain.h"
#include "Particle.hpp"
#include "Attractor.hpp"
#include "ParameterSmoother.hpp"
#endif /* ParticleSystem_hpp */


class ParticleSystem{
  
public:
    
    ParticleSystem();
    void setup(int _numOfParticles);
    void update();
    void draw();
    void receivePoints(ofPolyline blob);
    void receiveFlow(float x, float y);
    void changeMode();

    vector <Particle> particles;

    int numOfParticles;
    
    ofPolyline body;
    ofPoint linePoint;
    ofPoint cent;

    bool followLeader;
    
    float angle;
    float step;
    float spacing;
    float flowX,flowY;
    
    bool centPull;
    void modeSwitch();
    bool followOnLine;
    bool returnToBody;
    bool attractorPull;

    int modeCounter;
    int waveCounter;
    
    ofColor col1;
    ofColor col2;
    ofColor colLerp;
    
    int getMode();
    
    
  
    

};
