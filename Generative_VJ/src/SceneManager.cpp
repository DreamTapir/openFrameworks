#ifndef SceneManager_cpp
#define SceneManager_cpp

#include "SceneElement.cpp"
#include "ofMain.h"

class SceneManager {
protected:
    int elementIndex = 0;
    vector<SceneElement*> elements;
public:

    virtual void setup() = 0;

    void init() {
        for (int i = 0; i < elements.size(); i++) {
            elements[i]->init();
        }
    }
    void update() {
        elements.at(elementIndex)->update();
    };

    void draw() {
        ofSetWindowTitle("FPS:" + ofToString(ofGetFrameRate()));
        elements.at(elementIndex)->draw();
    };

    void end() {
        for (int i = 0; i < elements.size(); i++) {
            elements[i]->end();
        }
    }
    void audioIn(ofSoundBuffer& input) {
        elements.at(elementIndex)->audioIn(input);
    };

    void changeElement(int index) {
        if (index >= elements.size()) return;
        elements[elementIndex]->stop();
        elementIndex = index;
        elements[elementIndex]->start();
    }

    /*   bool nextElement(){
           elements[elementIndex]->stop();
           elementIndex++;
           if(elementIndex >= elements.size()){
               elementIndex--;
               return false;
           }else{
               elements[elementIndex]->start();
               return true;
           }
       };*/

    virtual void keyPressed(int key) {
        elements.at(elementIndex)->keyPressed(key);
    }

    virtual void keyReleased(int key) {
        elements.at(elementIndex)->keyReleased(key);
    };
};
#endif 