#pragma once
#ifndef TransMesh_h
#define TransMesh_h
#define BUFFER_SIZE 256

#include "SceneElement.cpp"

class TransMesh : public SceneElement {
public:
	TransMesh();
	void update() override;
	void draw() override;

	void audioIn(ofSoundBuffer& input) override;
private:
	ofEasyCam cam;
	ofMesh mesh;

	ofFbo			centerFbo, sideFbo;

	// AudioInput
	float smoothedVol;
	float scaledVol;
	vector <float> sound;
	ofSoundStream soundStream;

	// boxes
	vector<ofVec3f> posns;
	vector<ofColor> cols;
	ofVboMesh boxMesh;
	static const unsigned NUM_BOXES = 100;
};
#endif