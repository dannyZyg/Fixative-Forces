//
//  ParticleSystem.cpp
//  magnets
//
//  Created by Danny on 7/4/18.
//

#include "ParticleSystem.hpp"

//-------------------------------------------------------
ParticleSystem::ParticleSystem()
{
    modeCounter = 1;
    waveCounter = 0;

}

//--------------------------------------------------------------
void ParticleSystem::setup(int _numOfParticles){

    numOfParticles = _numOfParticles;
    step = 0;
    
    for (int x=0; x<numOfParticles; x++) {
        Particle p;
        particles.push_back(p);
    }
}

//--------------------------------------------------------------
void ParticleSystem::update(){
    
    //function taking care of mode changes
    changeMode();
    
    //calculate blob centroid
    cent = body.getCentroid2D();
    
    float p = ofMap(sin(step), -1, 1, 0, 1);
    

    for (int x=0; x<particles.size(); x++) {
        
        spacing = 1./numOfParticles;
        
        linePoint = body.getPointAtPercent(spacing * x);
    
        //These need to be run in all modes
        particles[x].receiveFlow(flowX, flowY);
        particles[x].update();
        particles[x].checkEdges();
        
        
        // Main section for programming different behaviours in the modes
        
        if(attractorPull){
            
            //get a point on the blob, and pull particle to the point
            
            linePoint = body.getPointAtPercent(spacing * x);
            Attractor a(linePoint, 10);
            ofPoint force = a.attract(particles[x]);
            particles[x].applyForce(force);
       
        }
        
        if(returnToBody){
            
            //get a point on the blob, and accelerate to the point

            if(particles[x].velocity.x <= 0.3 && particles[x].velocity.y <= 0.3){
                linePoint = body.getPointAtPercent(spacing * x);
                particles[x].accelerateTowardsTarget(linePoint);
            
            }
        }
        
        
        if(followLeader){
            
            // Leader moves along the blob from beginning to end and back, other particles accelerate to the particle ahead of them
            
            if(x == 0){
                
                if(followOnLine){
                    
                    linePoint = body.getPointAtPercent(p);
                    particles[x].position = linePoint;
                }
            }
            
            if(x > 0){
                linePoint = body.getPointAtPercent(spacing * x);
                ofPoint followLeader;
                followLeader = (particles[x - 1].position);
                particles[x].accelerateTowardsTarget(followLeader);
            }

        }
  
        
        if(centPull){
            
            // gravitational force created at blob centroid, additional downward gravitational force comes and goes (sin).
            
            ofPoint centroid = body.getCentroid2D();
            Attractor a(centroid, 10);
            ofPoint force = a.attract(particles[x]);
            particles[x].applyForce(force);
    
            float yGrav = ofMap(sin(ofGetFrameNum() * 0.01), -1, 1, 0., 2.);
            ofPoint gravity(0, yGrav);
            particles[x].applyForce(gravity);
            
        }
    }
    step+= 0.01;
    
}

//--------------------------------------------------------------
void ParticleSystem::draw(){
    
    for (int x=0; x<particles.size(); x++) {
        
        //colour settings for different modes
        
        if(modeCounter == 1){
            spacing = 1./numOfParticles;
            col1 = ofColor(25, 198, 141);
            col2 = ofColor(80, 21, 96);
            colLerp = col1.getLerped(col2, spacing * x);
            ofSetColor(colLerp);
            
        }

        if(modeCounter == 2){
            
            spacing = 1./numOfParticles;
            linePoint = body.getPointAtPercent(spacing * x);
            float dist = ofDist(linePoint.x, linePoint.y, particles[x].position.x, particles[x].position.y);
            float distMap = ofMap(dist, 0, 150, 0., 1., true);
            col1 = ofColor(53, 22, 229);
            col2 = ofColor(244, 109, 36);
            colLerp = col1.getLerped(col2, distMap);
            ofSetColor(colLerp);
            
        }
        if(modeCounter == 3){
            spacing = 1./numOfParticles;
            col1 = ofColor(211 , 28, 28);
            col2 = ofColor(239 , 165, 4);
            colLerp = col1.getLerped(col2, particles[x].randomLerpOffset);
            ofSetColor(colLerp);
        }
        if(modeCounter == 4){
            
            
            spacing = 1./numOfParticles;
            col1 = ofColor(83 , 7, 158);
            col2 = ofColor(158 , 7, 30);
            colLerp = col1.getLerped(col2, particles[x].randomLerpOffset);
            ofSetColor(colLerp);
            
        }
    
        //draw all particles
        particles[x].draw();
    }

}

//--------------------------------------------------------------

//Function that receives the polyline "largestBlob" from ofApp.cpp
void ParticleSystem::receivePoints(ofPolyline blob){
    body = blob;
    
}
//--------------------------------------------------------------
void ParticleSystem::receiveFlow(float x, float y){
    
    //recieves optical flow from particle system, which comes from ofApp.cpp
    
    flowX = x;
    flowY = y;
}
//--------------------------------------------------------------

//function used as keyboard shortcut for debugging
void ParticleSystem::modeSwitch(){
    
    modeCounter++;
    if(modeCounter > 4){
        modeCounter = 1;
        
    }
    cout<<"MODE = " <<modeCounter<<endl;
}

//--------------------------------------------------------------

void ParticleSystem::changeMode(){
    
    if(modeCounter > 4){
        modeCounter = 1;
        cout<<"MODE = " <<modeCounter<<endl;
    }
    
    // listens for big changes in optical flow. After 10 big waves or movements, mode will change

    if(flowX >= 3. || flowX <= -3.) waveCounter++;
    if(waveCounter > 10)modeCounter ++;
    if(waveCounter > 10 )waveCounter = 0;
    
    // Using the various Bools, customise each mode!
    
    //Mode 1 = Follow leader along outline
    if(modeCounter == 1){
        
        followLeader = true;
        centPull = false;
        followOnLine = true;
        returnToBody = false;
        attractorPull = false;
        
    }
    
    //Mode 2 - Particles try to accelerate towards their given position on the blob
    if(modeCounter == 2){
        
        followLeader = false;
        centPull = false;
        followOnLine = false;
        returnToBody = true;
        attractorPull = false;

    }
    
    //Mode 3 - Attractor Pull - particles cling to their position on the blob
    if(modeCounter == 3){
        
        followLeader = false;
        centPull = false;
        followOnLine = false;
        returnToBody = false;
        attractorPull = true;

    }
    
    // Mode 4 - Centroid pull + noised gravity
    
    if(modeCounter == 4){
        
        followLeader = false;
        centPull = true;
        followOnLine = false;
        returnToBody = false;
        attractorPull = false;
    }
    
}

//--------------------------------------------------------------

//Function to be called in ofApp.cpp to send mode value out
int ParticleSystem::getMode(){
    
    int modeOut = modeCounter;
    return modeOut;
    
}




