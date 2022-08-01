#pragma once
#ifndef HexagonMesh_h
#define HexagonMesh_h
#define BUFFER_SIZE 256

#include "SceneElement.cpp"

class HexagonMesh : public SceneElement {
public:
	HexagonMesh();
	void update() override;
	void draw() override;

	void audioIn(ofSoundBuffer& input) override;
private:
	ofFbo			centerFbo, sideFbo;

	// AudioInput
	float smoothedVol;
	float scaledVol;
	vector <double> sound;

	ofSoundStream soundStream;

	// HexagonToMesh
	void setHexagonToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float radius, float height);

	ofEasyCam cam;
	ofMesh face, frame;
};
#endif