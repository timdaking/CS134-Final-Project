#include "ofApp.h"
#include "Util.h"
#include "glm/gtx/intersect.hpp"

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
    
    background.load("images/space.jpg");
    
    bWireframe = false;
    bDisplayPoints = false;
    bAltKeyDown = false;
    bCtrlKeyDown = false;
    bLanderLoaded = true;
    bTerrainSelected = true;
    //    ofSetWindowShape(1024, 768);
    cam.setDistance(10);
    cam.setNearClip(.1);
    cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
    ofSetVerticalSync(true);
    cam.disableMouseInput();
    ofEnableSmoothing();
    ofEnableDepthTest();
    
    // setup rudimentary lighting
    //
    initLightingAndMaterials();
    
    //load up mars and lander model
    mars.loadModel("geo/mars-low-5x-v2.obj");
    //mars.loadModel("geo/mars-terrain-v2.obj");
    mars.setScale(5, 5, 5);
    mars.setScaleNormalization(false);
    //lander.loadModel("geo/lander.obj");
    lander.loadModel("geo/starship.fbx");
    lander.setScale(0.03, 0.03, 0.03);
    lander.setScaleNormalization(false);
    
    //camera module
    camera = &cam;
    groundCam.setOrientation(ofVec3f(-90, 0, 0));
    groundCam.setNearClip(.1);
    sideCam.setNearClip(.1);
    trackCam.setPosition(0, 1, 0);
    trackCam.setNearClip(.1);
    
    octrees.create(mars.getMesh(0), 7);
    collided = false;
    
    cam.setDistance(10);
    cam.setNearClip(.1);
    cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetBackgroundColor(ofColor::black);
    
    //sound system
    if (noise.load("sounds/thruster2.mp3")) {
        noise.setLoop(true);
        soundFileLoaded = true;
    }
    
    //engine emitter
    engine.setRate(600);
    engine.setParticleRadius(.010);
    engine.setEmitterType(DiscEmitter);
    engine.setLifespan(0.5);
    engine.visible = false;
    rocket.radius = 0.00010;
    
    //rocket lander particles
    rocket.lifespan = 50;
    rocket.position.set(0, 10, 0);
    lander.setPosition(rocket.position.x, rocket.position.y, rocket.position.z);
    sys.add(rocket);
    sys.addForce(&thrust);
    sys.addForce(&impulseForce);
    
    //engine forces
    engine.sys->addForce(new TurbulenceForce(ofVec3f(-2, -1, -3), ofVec3f(1, 2, 5)));
    engine.sys->addForce(new ImpulseRadialForce(10));
    engine.sys->addForce(new CyclicForce(20));
    engine.setGroupSize(10);
    
    //rocket's gravity force
    sys.addForce(new GravityForce(ofVec3f(0, -.01, 0)));
    
    //fuel system, 120000ms = 120s
    fuel = 120000;
    thrustTime = 0;
    ofResetElapsedTimeCounter();
    
    //lighting system
    areaLight.setup();
    areaLight.enable();
    areaLight.setAreaLight(10, 10);
    areaLight.setPosition(100,100,100);
    
    landing1.setup();
    landing1.enable();
    landing1.setSpotlight();
    landing1.setScale(.05);
    landing1.setSpotlightCutOff(15);
    landing1.setAttenuation(2, .001, .001);
    landing1.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    landing1.setDiffuseColor(ofFloatColor(1, 1, 1));
    landing1.setSpecularColor(ofFloatColor(1, 1, 1));
    landing1.rotate(-10, ofVec3f(1, 0, 0));
    landing1.rotate(-45, ofVec3f(0, 1, 0));
    landing1.setPosition(10, 6, 6);
    
    landing2.setup();
    landing2.enable();
    landing2.setSpotlight();
    landing2.setScale(.05);
    landing2.setSpotlightCutOff(15);
    landing2.setAttenuation(2, .001, .001);
    landing2.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    landing2.setDiffuseColor(ofFloatColor(1, 1, 1));
    landing2.setSpecularColor(ofFloatColor(1, 1, 1));
    landing2.rotate(-10, ofVec3f(1, 0, 0));
    landing2.rotate(-45, ofVec3f(0, 1, 0));
    landing2.setPosition(15, 6, 6);
    
    landing3.setup();
    landing3.enable();
    landing3.setSpotlight();
    landing3.setScale(.05);
    landing3.setSpotlightCutOff(15);
    landing3.setAttenuation(2, .001, .001);
    landing3.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    landing3.setDiffuseColor(ofFloatColor(1, 1, 1));
    landing3.setSpecularColor(ofFloatColor(1, 1, 1));
    landing3.rotate(-10, ofVec3f(1, 0, 0));
    landing3.rotate(-45, ofVec3f(0, 1, 0));
    landing3.setPosition(20, 6, 6);
    
    
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
    
    altitudes = sys.particles[0].position.y+6;
    sys.update();
    engine.update();
    engine.setPosition(sys.particles[0].position);
    lander.setPosition(sys.particles[0].position.x, sys.particles[0].position.y+2, sys.particles[0].position.z);
    lander.update();
    detectCollision();
    
    // Camera
    //
    groundCam.setPosition(sys.particles[0].position + ofVec3f(0.1, 0, 0.1));
    sideCam.setPosition(sys.particles[0].position + ofVec3f(-1.5, 0, 0));
    trackCam.lookAt(lander.getPosition());
}
//--------------------------------------------------------------
void ofApp::draw() {
    
    if(fuel > 0){
        bLanderLoaded = true;
        gameOver = false;
        ofSetColor(255, 255, 255);
        ofDisableDepthTest();
        background.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
        ofEnableDepthTest();
        
        camera->begin();
        ofPushMatrix();
        
        //areaLight.draw();
        landing1.draw();
        
        
        
        //draw particle and engine
        sys.draw();
        engine.draw();
        
        if (bWireframe) {                    // wireframe mode  (include axis)
            ofDisableLighting();
            ofSetColor(ofColor::red);
            mars.drawWireframe();
            lander.drawWireframe();
            if (bRoverLoaded) {
                rover.drawWireframe();
            }
        }
        else {
            ofEnableLighting();              // shaded mode
            mars.drawFaces();
            lander.drawFaces();
            
            if (bRoverLoaded) {
                rover.drawFaces();
            }
        }
        
        ofNoFill();
        camera->end();
    }
    else{
        bLanderLoaded = false;
        gameOver = true;
        fuel = 0;
        noise.stop();
        engine.stop();
        string noFuel;
        noFuel += "Game Over, No More Fuel! Click R to Reset";
        ofDrawBitmapString(noFuel, ofPoint(ofGetWindowWidth()/2, ofGetWindowHeight()/2));
        
    }
    string fuelAmount;
    fuelAmount += "Fuel: " + std::to_string(fuel) + " ms remaining";
    ofDrawBitmapString(fuelAmount, ofPoint(10, 20));
    
    string landed;
    if(collided == true){
        landed += "Landing Status: landed";
        ofDrawBitmapString(landed, ofPoint(10, 40));
    }
    else{
        landed += "Landing Status: Not Landed";
        ofDrawBitmapString(landed, ofPoint(10, 40));
    }
    
    string altitude;
    if(collided == true){
        altitude += "Altitude: 0";
        ofDrawBitmapString(altitude, ofPoint(10, 60));
    }
    else{
        altitude += "Altitude: " + std::to_string(altitudes);
        ofDrawBitmapString(altitude, ofPoint(10, 60));
    }
}


//
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {
    
    ofPushMatrix();
    ofTranslate(location);
    
    ofSetLineWidth(1.0);
    
    // X Axis
    ofSetColor(ofColor(255, 0, 0));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
    
    
    // Y Axis
    ofSetColor(ofColor(0, 255, 0));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));
    
    // Z Axis
    ofSetColor(ofColor(0, 0, 255));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));
    
    ofPopMatrix();
}


void ofApp::keyPressed(int key) {
    
    switch (key) {
        case 'B':
        case 'b':
            bDisplayBBoxes = !bDisplayBBoxes;
            break;
        case 'C':
        case 'c':
            if (cam.getMouseInputEnabled()) cam.disableMouseInput();
            else cam.enableMouseInput();
            break;
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
        case 'H':
        case 'h':
            break;
        case 'L':
        case 'l':
            bDisplayLeafNodes = !bDisplayLeafNodes;
            break;
        case 'O':
        case 'o':
            bDisplayOctree = !bDisplayOctree;
            break;
        case 'r':
            cam.reset();
            if(gameOver == true){
                gameOver = false;
                fuel = 120000;
                lander.setPosition(rocket.position.x, rocket.position.y, rocket.position.z);
            }
            break;
        case 's':
            savePicture();
            break;
        case 't':
            setCameraTarget();
            break;
        case 'u':
            break;
        case 'v':
            togglePointsDisplay();
            break;
        case 'V':
            break;
        case 'w':
            toggleWireframeMode();
            break;
        case OF_KEY_ALT:
            cam.enableMouseInput();
            bAltKeyDown = true;
            break;
        case OF_KEY_CONTROL:
            bCtrlKeyDown = true;
            break;
        case OF_KEY_SHIFT:
            break;
        case OF_KEY_DEL:
            break;
        case OF_KEY_UP:
            if(gameOver == true){
                engine.stop();
            }
            else{
                thrustTime = ofGetElapsedTimeMillis();
                soundPlayer();
                thrust.add(ofVec3f(0, .5, 0));
                engine.setVelocity(ofVec3f(0, -5, 0));
                engine.start();
                noise.play();
            }
            break;
        case OF_KEY_DOWN:
            if(gameOver == true){
                engine.stop();
            }
            else{
                thrustTime = ofGetElapsedTimeMillis();
                soundPlayer();
                thrust.add(ofVec3f(0, -.5, 0));
                engine.setVelocity(ofVec3f(0, -5, 0));
                engine.start();
                noise.play();
            }
            break;
        case OF_KEY_LEFT:
            if(gameOver == true){
                engine.stop();
            }
            else{
                thrustTime = ofGetElapsedTimeMillis();
                soundPlayer();
                thrust.add(ofVec3f(-.5, 0, 0));
                engine.setVelocity(ofVec3f(5, -5, 0));
                engine.start();
                noise.play();
            }
            break;
        case OF_KEY_RIGHT:
            if(gameOver == true){
                engine.stop();
            }
            else{
                thrustTime = ofGetElapsedTimeMillis();
                soundPlayer();
                thrust.add(ofVec3f(.5, 0, 0));
                engine.setVelocity(ofVec3f(-5, -5, 0));
                engine.start();
                noise.play();
            }
            break;
        case 'z':
            if(gameOver == true){
                engine.stop();
            }
            else{
                thrustTime = ofGetElapsedTimeMillis();
                soundPlayer();
                thrust.add(ofVec3f(0, 0, 0.5));
                engine.setVelocity(ofVec3f(0, -5, -5));
                engine.start();
                noise.play();
            }
            break;
        case 'x':
            if(gameOver == true){
                engine.stop();
            }
            else{
                thrustTime = ofGetElapsedTimeMillis();
                soundPlayer();
                thrust.add(ofVec3f(0, 0, -0.5));
                engine.setVelocity(ofVec3f(0, -5, 5));
                engine.start();
                noise.play();
            }
            break;
        case OF_KEY_F1:
            camera = &cam;
            break;
        case OF_KEY_F2:
            camera = &groundCam;
            break;
        case OF_KEY_F3:
            camera = &sideCam;
            break;
        case OF_KEY_F4:
            camera = &trackCam;
            break;
        default:
            break;
    }
}

void ofApp::toggleWireframeMode() {
    bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
    bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
    bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {
    ofResetElapsedTimeCounter();
    switch (key) {
            
        case OF_KEY_ALT:
            cam.disableMouseInput();
            bAltKeyDown = false;
            break;
        case OF_KEY_CONTROL:
            bCtrlKeyDown = false;
            break;
        case OF_KEY_SHIFT:
            break;
        case OF_KEY_UP:
            noise.stop();
            engine.stop();
            thrust.set(ofVec3f(0, 0, 0));
            fuel -= thrustTime;
            ofResetElapsedTimeCounter();
            break;
        case OF_KEY_DOWN:
            noise.stop();
            engine.stop();
            thrust.set(ofVec3f(0, 0, 0));
            fuel -= thrustTime;
            ofResetElapsedTimeCounter();
            break;
        case OF_KEY_LEFT:
            noise.stop();
            engine.stop();
            thrust.set(ofVec3f(0, 0, 0));
            fuel -= thrustTime;
            ofResetElapsedTimeCounter();
            break;
        case OF_KEY_RIGHT:
            noise.stop();
            engine.stop();
            thrust.set(ofVec3f(0, 0, 0));
            fuel -= thrustTime;
            ofResetElapsedTimeCounter();
            break;
        case 'z':
            noise.stop();
            engine.stop();
            thrust.set(ofVec3f(0, 0, 0));
            fuel -= thrustTime;
            ofResetElapsedTimeCounter();
            break;
        case 'x':
            noise.stop();
            engine.stop();
            thrust.set(ofVec3f(0, 0, 0));
            fuel -= thrustTime;
            ofResetElapsedTimeCounter();
            break;
        default:
            break;
            
    }
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
    
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
    // if moving camera, don't allow mouse interaction
    //
    if (cam.getMouseInputEnabled()) return;
    
    // if moving camera, don't allow mouse interaction
    //
    if (cam.getMouseInputEnabled()) return;
    
    // if rover is loaded, test for selection
    //
    if (bLanderLoaded) {
        glm::vec3 origin = cam.getPosition();
        glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
        glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
        
        ofVec3f min = lander.getSceneMin() + lander.getPosition();
        ofVec3f max = lander.getSceneMax() + lander.getPosition();
        
        Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
        bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
        if (hit) {
            bLanderSelected = true;
            mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
            mouseLastPos = mouseDownPos;
            bInDrag = true;
        }
        else {
            bLanderSelected = false;
        }
    }
    else {
        ofVec3f p;
        raySelectWithOctree(p);
    }
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
    ofVec3f mouse(mouseX, mouseY);
    ofVec3f rayPoint = cam.screenToWorld(mouse);
    ofVec3f rayDir = rayPoint - cam.getPosition();
    rayDir.normalize();
    Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
                  Vector3(rayDir.x, rayDir.y, rayDir.z));
    
    pointSelected = octree.intersect(ray, octree.root, selectedNode);
    
    if (pointSelected) {
        pointRet = octree.mesh.getVertex(selectedNode.points[0]);
    }
    return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
    // if moving camera, don't allow mouse interaction
    //
    if (cam.getMouseInputEnabled()) return;
    
    if (bInDrag) {
        
        glm::vec3 landerPos = lander.getPosition();
        
        glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
        glm::vec3 delta = mousePos - mouseLastPos;
        
        landerPos += delta;
        lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
        mouseLastPos = mousePos;
        
        ofVec3f min = lander.getSceneMin() + lander.getPosition();
        ofVec3f max = lander.getSceneMax() + lander.getPosition();
        
        Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
        
        colBoxList.clear();
        octree.intersect(bounds, octree.root, colBoxList);
        
        //overlap test
        /*
         if (bounds.overlap(testBox)) {
         cout << "overlap" << endl;
         }
         else {
         cout << "OK" << endl;
         }*/
        
        
    }
    else {
        ofVec3f p;
        raySelectWithOctree(p);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//
void ofApp::setCameraTarget() {
    
}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {
    
    static float ambient[] =
    { .5f, .5f, .5, 1.0f };
    static float diffuse[] =
    { 1.0f, 1.0f, 1.0f, 1.0f };
    
    static float position[] =
    {5.0, 5.0, 5.0, 0.0 };
    
    static float lmodel_ambient[] =
    { 1.0f, 1.0f, 1.0f, 1.0f };
    
    static float lmodel_twoside[] =
    { GL_TRUE };
    
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //    glEnable(GL_LIGHT1);
    glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
    ofImage picture;
    picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    picture.save("screenshot.png");
    cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {
    
    ofVec3f point;
    mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
    if (lander.loadModel(dragInfo.files[0])) {
        lander.setScaleNormalization(false);
        //        lander.setScale(.1, .1, .1);
        //    lander.setPosition(point.x, point.y, point.z);
        lander.setPosition(1, 1, 0);
        
        bLanderLoaded = true;
        for (int i = 0; i < lander.getMeshCount(); i++) {
            bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
        }
        
        cout << "Mesh Count: " << lander.getMeshCount() << endl;
    }
    else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
    ofVec2f mouse(mouseX, mouseY);
    ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
    ofVec3f rayDir = rayPoint - cam.getPosition();
    rayDir.normalize();
    return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (lander.loadModel(dragInfo.files[0])) {
        bLanderLoaded = true;
        lander.setScaleNormalization(false);
        lander.setPosition(0, 0, 0);
        cout << "number of meshes: " << lander.getNumMeshes() << endl;
        bboxList.clear();
        for (int i = 0; i < lander.getMeshCount(); i++) {
            bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
        }
        
        //        lander.setRotation(1, 180, 1, 0, 0);
        
        // We want to drag and drop a 3D object in space so that the model appears
        // under the mouse pointer where you drop it !
        //
        // Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
        // once we find the point of intersection, we can position the lander/lander
        // at that location.
        //
        
        // Setup our rays
        //
        glm::vec3 origin = cam.getPosition();
        glm::vec3 camAxis = cam.getZAxis();
        glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
        glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
        float distance;
        
        bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
        if (hit) {
            // find the point of intersection on the plane using the distance
            // We use the parameteric line or vector representation of a line to compute
            //
            // p' = p + s * dir;
            //
            glm::vec3 intersectPoint = origin + distance * mouseDir;
            
            // Now position the lander's origin at that intersection point
            //
            glm::vec3 min = lander.getSceneMin();
            glm::vec3 max = lander.getSceneMax();
            float offset = (max.y - min.y) / 2.0;
            lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);
            
            // set up bounding box for lander while we are at it
            //
            landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
        }
    }
    
    
}

//  intersect the mouse ray with the plane normal to the camera
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
    // Setup our rays
    //
    glm::vec3 origin = cam.getPosition();
    glm::vec3 camAxis = cam.getZAxis();
    glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
    glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
    float distance;
    
    bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);
    
    if (hit) {
        // find the point of intersection on the plane using the distance
        // We use the parameteric line or vector representation of a line to compute
        //
        // p' = p + s * dir;
        //
        glm::vec3 intersectPoint = origin + distance * mouseDir;
        
        return intersectPoint;
    }
    else return glm::vec3(0, 0, 0);
}

//sound player
void ofApp::soundPlayer() {
    if (soundFileLoaded)
        noise.play();
}

// collision detection
void ofApp::detectCollision() {
    touchPoint = sys.particles[0].position+6;
    ofVec3f velocity = sys.particles[0].velocity;
    //cout<<velocity<<endl;
    cout<<touchPoint<<endl;
    if (octrees.intersect(touchPoint, octrees.root)) {
        collided = true;
        impulseForce.apply(1.5 * (-velocity * 2));
    }
    else{
        collided = false;
    }
    
    if (octrees.intersect(touchPoint, octrees.root) && velocity.y<-7){
        impulseForce.apply(50 * (-velocity * 4));
    }
}
