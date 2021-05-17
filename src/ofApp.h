#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "ray.h"
#include "box.h"



class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent2(ofDragInfo dragInfo);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawAxis(ofVec3f);
    void initLightingAndMaterials();
    void savePicture();
    void toggleWireframeMode();
    void togglePointsDisplay();
    void toggleSelectTerrain();
    void setCameraTarget();
    bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
    bool raySelectWithOctree(ofVec3f &pointRet);
    glm::vec3 getMousePointOnPlane(glm::vec3 p , glm::vec3 n);
    
    ofEasyCam cam;
    ofxAssimpModelLoader mars, lander, rover;
    ofLight light;
    Box boundingBox, landerBounds;
    Box testBox;
    vector<Box> colBoxList;
    bool bLanderSelected = false;
    Octree octree;
    TreeNode selectedNode;
    glm::vec3 mouseDownPos, mouseLastPos;
    bool bInDrag = false;
    
    
    ofxIntSlider numLevels;
    ofxPanel gui;
    
    bool bAltKeyDown;
    bool bCtrlKeyDown;
    bool bWireframe;
    bool bDisplayPoints;
    bool bPointSelected;
    bool bHide;
    bool pointSelected = false;
    bool bDisplayLeafNodes = false;
    bool bDisplayOctree = false;
    bool bDisplayBBoxes = false;
    
    bool bLanderLoaded;
    bool bRoverLoaded;
    bool bTerrainSelected;
    
    ofVec3f selectedPoint;
    ofVec3f intersectPoint;
    
    vector<Box> bboxList;
    
    const float selectionRange = 4.0;
    
    //custom
    ofCamera groundCam;
    ofCamera sideCam;
    ofCamera trackCam;
    ofCamera *camera = NULL;
    
    ofImage background;
    
    Octree octrees;
    
    bool collided;
    
    void detectCollision();
    void soundPlayer();
    
    ofVec3f touchPoint;
    
    ImpulseForce impulseForce;
    ParticleSystem sys;
    ThrusterForce thruster;
    ParticleEmitter engine;
    Particle ship;
    
    ofSoundPlayer noise;
    bool soundFileLoaded = false;
    
};
