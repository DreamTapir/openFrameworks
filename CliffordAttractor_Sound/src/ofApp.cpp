#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);
    ofEnableDepthTest();

    this->sound.load("music.mp3");
    this->sound.play();
    this->sound.setLoop(true);

    this->n_bands_to_get = 120;

    for (int j = 0; j < this->n_bands_to_get; j++) {
        this->fft_smoothed.push_back(0);
    }

    a = -2.0;
    b = 2.0;
    c = -2.0;
    d = 2.0;
    e = 200;
    f = 200;
    g = 200;
    x = 0.0;
    y = 0.0;
    z = 0;
    pointSize = 1.0;

    rotate = 0;

    cam.setDistance(1000);
}

//--------------------------------------------------------------
void ofApp::update(){
    ofClear(0);
    ofSoundUpdate();

    this->color = ofColor::fromHsb(30, 0, 255);

    float* val = ofSoundGetSpectrum(this->n_bands_to_get);

    for (int i = 0; i < this->n_bands_to_get; i++) {

        this->fft_smoothed[i] *= 0.96f;
        if (this->fft_smoothed[i] < val[i]) {

            this->fft_smoothed[i] = val[i];
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

    vboMesh.clear();

    cam.begin();

    ofRotate(rotate++);

    for (int i = 0; i < numParticle; i++) {

        xn = sin(fft_smoothed[5] * fft_smoothed[5] * 10 * z) + fft_smoothed[10] * fft_smoothed[10] * 10 * cos(fft_smoothed[5] * fft_smoothed[5] * 10 * x);
        yn = sin(fft_smoothed[12] * fft_smoothed[12] * 10 * x) + fft_smoothed[15] * fft_smoothed[15] * 10 * cos(fft_smoothed[12] * fft_smoothed[12] * 10 * y);
        zn = sin(fft_smoothed[20] * fft_smoothed[20] * 10 * y) + fft_smoothed[18] * fft_smoothed[18] * 10 * cos(fft_smoothed[20] * fft_smoothed[20] * 10 * z);

        x = xn;
        y = yn;
        z = zn;

        points[i].x = x * e;
        points[i].y = y * f;
        points[i].z = z * g;

        vboMesh.addVertex(ofVec3f(points[i].x, points[i].y, points[i].z));
        vboMesh.addColor(color);
    }

    glPointSize(pointSize);
    vboMesh.setMode(OF_PRIMITIVE_POINTS);
    vboMesh.draw();

    ofPopMatrix();

    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
