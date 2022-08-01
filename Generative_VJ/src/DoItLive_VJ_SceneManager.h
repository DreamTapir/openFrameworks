#ifndef DoItLive_VJ_SceneManager_h
#define DoItLive_VJ_SceneManager_h

#include "SceneManager.cpp"
#include "ofxPostGlitch.h"
#include "ofxPostProcessing.h"
#include "CliffordAttractor.h"
#include "TransMesh.h"
#include "PointFlow.h"
#include "ReactionDiffusion.h"
#include "HexagonMesh.h"
#include "RotateSphere.h"

class DoItLive_VJ_SceneManager : public SceneManager {
public:
    void setup();
    void keyPressed(int key) override;
    void keyReleased(int key) override;

    ofEasyCam cam;

    // PostGlitch
    ofxPostGlitch	Glitch;
    ofFbo			Fbo;

    // scene stuff
    ofxPostProcessing post;
    ofLight light;
};
#endif