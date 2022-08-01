#ifndef SceneElement_cpp
#define SceneElement_cpp

#include "ofMain.h"]

class SceneElement {
public:
    virtual void init() {};
    virtual void start() {};
    virtual void stop() {};
    virtual void update() {};
    virtual void draw() = 0;
    virtual void end() {};
    virtual void audioIn(ofSoundBuffer& input) {};

    virtual void keyPressed(int key) {};
    virtual void keyReleased(int key) {};
};
#endif