//
//  Attractor.cpp
//  magnets
//
//  Created by Danny on 6/4/18.
//
#include "Attractor.hpp"

//--------------------------------------------------------------

Attractor::Attractor(ofPoint pos, float s){
    position = pos;
    strength = s;
    mass = 20;
    G = 20;
}

//--------------------------------------------------------------



ofPoint Attractor::attract(Particle p){
    
    //calculate direction of force
    ofPoint force = position - p.position;
    float d = ofDist(p.position.x, p.position.y, position.x, position.y);
    
    
    d = (d, 2., 25.);
    force.normalize();
    
    float strength = (G * mass * p.mass) / (d * d);
    
    // put magnitude and direction together
    
    force *= strength;
    
    return force;

}

