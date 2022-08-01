#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;

    settings.setSize(1280, 1280);
    settings.setPosition(ofVec2f(0, 0));
    settings.decorated = false;
    shared_ptr<ofAppBaseWindow> mainwindow = ofCreateWindow(settings);

    shared_ptr<ofApp> mainApp(new ofApp());
    ofRunApp(mainwindow, mainApp);
    ofRunMainLoop();
}
