
//  Kevin M. Smith - CS 134 SJSU

#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter() {
    sys = new ParticleSystem();
    createdSys = true;
    init();
}

ParticleEmitter::ParticleEmitter(ParticleSystem *s) {
    if (s == NULL)
    {
        cout << "fatal error: null particle system passed to ParticleEmitter()" << endl;
        ofExit();
    }
    sys = s;
    createdSys = false;
    init();
}

ParticleEmitter::~ParticleEmitter() {

    // deallocate particle system if emitter created one internally
    //
    if (createdSys) delete sys;
}

void ParticleEmitter::init() {
    rate = 1;
    velocity = ofVec3f(0, 20, 0);
    lifespan = 3;
    mass = 1;
    randomLife = false;
    lifeMinMax = ofVec3f(2, 4);
    started = false;
    oneShot = false;
    fired = false;
    lastSpawned = 0;
    radius = 1;
    particleRadius = .1;
    visible = true;
    type = DirectionalEmitter;
    groupSize = 1;
    damping = .99;
    //i changed it to blue
    particleColor = ofColor::blue;
    position = ofVec3f(0,0,0);
}



void ParticleEmitter::draw() {
    if (visible) {
        switch (type) {
        case DirectionalEmitter:
            ofDrawSphere(position, radius/10);  // just draw a small sphere for point emitters
            break;
        case SphereEmitter:
                ofDrawCircle(position, radius/10);
                break;
        case RadialEmitter:
            ofDrawSphere(position, radius/10);  // just draw a small sphere as a placeholder
            break;
                
                //i added this -luis
        case DiscEmitter:
            ofDrawSphere(position, radius/10);  // just draw a small sphere as a placeholder
            break;
                //case for disc emitter
        default:
            break;
        }
    }
    sys->draw();
}
void ParticleEmitter::start() {
    if (started) return;
    started = true;
    lastSpawned = ofGetElapsedTimeMillis();
}

void ParticleEmitter::stop() {
    started = false;
    fired = false;
}
void ParticleEmitter::update() {

    float time = ofGetElapsedTimeMillis();

    if (oneShot && started) {
        if (!fired) {

            // spawn a new particle(s)
            //
            for (int i = 0; i < groupSize; i++) {
                spawn(time);
            }

            lastSpawned = time;
        }
        fired = true;
        stop();
    }

    else if (((time - lastSpawned) > (1000.0 / rate)) && started) {

        // spawn a new particle(s)
        //
        for (int i= 0; i < groupSize; i++)
            spawn(time);
    
        lastSpawned = time;
    }

    sys->update();
}

// spawn a single particle.  time is current time of birth
//
void ParticleEmitter::spawn(float time) {

    Particle particle;

    // set initial velocity and position
    // based on emitter type
    //
    switch (type) {
    case RadialEmitter:
      {
        ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
        float speed = velocity.length();
        particle.velocity = dir.getNormalized() * speed;
        particle.position.set(position);
      }
    break;
    case SphereEmitter:
        break;
    case DirectionalEmitter:
        particle.velocity = velocity;
        particle.position.set(position);
        break;
            
            // I added this -luis
    case DiscEmitter:   // x-z plane
      {
          ofVec3f dir = ofVec3f(ofRandom(-1, 1), 0, ofRandom(-1, 1));
         // float speed = velocity.length();
          particle.velocity = velocity;
          ofVec3f newPosition = ofVec3f(ofRandom(-1, 1), 0, ofRandom(-1, 1));
          newPosition = newPosition.getNormalized();
          particle.position.set((newPosition * radius)/5 + position);
          
         // Removed my code as it is part of the assignment - KMS
          
      }
        break;
    }

    // other particle attributes
    //
    if (randomLife) {
        particle.lifespan = ofRandom(lifeMinMax.x, lifeMinMax.y);
    }
    else particle.lifespan = lifespan;
    particle.birthtime = time;
    particle.radius = particleRadius;
    particle.mass = mass;
    particle.damping = damping;
    particle.color = particleColor;

    // add to system
    //
    sys->add(particle);
}
