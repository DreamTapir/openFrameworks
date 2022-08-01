#pragma once
#ifndef RotateSphere_h
#define RotateSphere_h
#define BUFFER_SIZE 256

#include "SceneElement.cpp"

class RotateSphere : public SceneElement {
public:
	RotateSphere();
	void update() override;
	void draw() override;

	void audioIn(ofSoundBuffer& input) override;
private:
	ofEasyCam cam;
	ofFbo			centerFbo, sideFbo;

	// AudioInput
	float smoothedVol;
	float scaledVol;
	vector <double> sound;

	ofSoundStream soundStream;

	// WarpSphere
	ofMesh face, frame;

	// TriangleSphere
	ofMesh mesh, meshframe;
	vector<ofMeshFace> triangle_list;
	int x = 0;
	int y = 0;
};
#endif