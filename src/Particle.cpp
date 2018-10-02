//
//  Particle.cpp
//  magnets
//
//  Created by Danny on 6/4/18.
//

#include "Particle.hpp"
//--------------------------------------------------------------

Particle::Particle(){
    
    // Initial parameters for the particle, mostly random
    position = ofPoint(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()));
    velocity = ofPoint(0,0);
    acceleration = ofPoint(0,0);
    maxSpeed = 5;
    maxForce = 5;
    mass = ofRandom(0.2, 4);
    distMult = 1;
    c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
    angle = ofDegToRad(ofRandom(0, 360));
    randomFlowOffset = ofRandom(1., 3.);
    randomOffset = ofRandom(0.05, 0.3);
    randomLerpOffset = ofRandom(1.);
    flowStep = 0;
    int numOfShapePoints = 10;
    
    ofSetColor(c);
    smoothedFlow.smoother = new ParameterSmoother(5., ofGetFrameRate());
    
    // Generate random points for the shape when constructor is used.
    for(int i = 0; i < numOfShapePoints; i ++){
        ofPoint temp(ofRandom(-10, 10), ofRandom(-10,10));
        shapePoints.push_back(temp);
    }
    
    }
    
//--------------------------------------------------------------

void Particle::update(){
    

    // Use smoothing to slowly add flow forces to particles
   
    smoothedFlow.currentValue = smoothedFlow.smoother -> process(smoothedFlow.targetValue);
    smoothedFlow.targetValue.set(flowX * randomFlowOffset * 0.5, - flowY * randomFlowOffset * 0.5);
    applyForce(smoothedFlow.currentValue);
    
    // Add friction
    friction = velocity;
    friction.normalize();
    cF = -0.1;
    friction *= cF;
    applyForce(friction);
    
    // calculate new position taking into account vel, acc, maxspeed
    velocity += acceleration;
    position += velocity;
    velocity.limit(maxSpeed);
    acceleration *= 0;

}
//--------------------------------------------------------------

void Particle::draw(){
    
    ofPushMatrix();
    ofPushStyle();

    ofTranslate(position);
    
    // rotate the shape with sin, and a random offset
    // add rotation speed using values from optical flow
    
    flowStep = ofMap(flowX, -5, 5, -1, 1);
    float a = ofMap(sin(ofGetFrameNum() * randomOffset + flowStep), -1, 1, 0, 270);
    ofRotate(a);
    
    //draw shape
    ofBeginShape();
    for(int i = 0; i < shapePoints.size(); i++){
        ofVertex(shapePoints[i]);
    }
    ofEndShape();
    ofPopStyle();
    ofPopMatrix();
}

//--------------------------------------------------------------

void Particle::applyForce(ofPoint force){
    ofPoint f = ofPoint(force);
    f /= mass;
    acceleration += f;
}

//--------------------------------------------------------------

void Particle::accelerateTowardsTarget(ofVec3f _target){
    
    ofVec3f target = _target;
    acceleration = target - position ;
    acceleration.limit(maxSpeed);

}

//--------------------------------------------------------------

// Wraparound when particles leave canvas

void Particle::checkEdges() {
    if (position.x < - radius) position.x = ofGetWidth() + radius;
    if (position.y < - radius) position.y = ofGetHeight() + radius;
    if (position.x > ofGetWidth() + radius) position.x = - radius;
    if (position.y > ofGetHeight() + radius) position.y = - radius;
}

//--------------------------------------------------------------
void Particle::receiveFlow(float x, float y){
    flowX = x;
    flowY = y;
}




