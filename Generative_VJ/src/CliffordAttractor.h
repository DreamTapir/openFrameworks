#pragma once
#ifndef CliffordAttractor_h
#define CliffordAttractor_h
#define numParticle 200000
#define BUFFER_SIZE 256

#include "SceneElement.cpp"

class CliffordAttractor : public SceneElement {
public:
	CliffordAttractor();
	void update() override;
	void draw() override;

	void audioIn(ofSoundBuffer& input) override;

private:
	ofEasyCam cam;

	float a, b, c, d, e, f, x, y, pointSize;
	float xn, yn;
	ofVboMesh vboMesh;
	ofColor color;
	ofVec2f points[numParticle];

	ofFbo			centerFbo, sideFbo;

	int base_radius;
	int number_of_satellite;

	vector<glm::vec3> base_noise_seed_list;
	vector<vector<glm::vec3>> satellite_location_list;
	vector<vector<glm::vec2>> satellite_noise_seed_list;
	vector<glm::vec3> location_list;
	vector<glm::vec3> velocity_list;

	ofMesh face, frame;

	// scene stuff
	ofLight light;

	// boxes
	vector<ofVec3f> posns;
	vector<ofColor> cols;
	ofVboMesh boxMesh;

	static const unsigned NUM_BOXES = 20;

	// AudioInput
	float smoothedVol;
	float scaledVol;
	vector <float> sound;

	ofSoundStream soundStream;
};
#endif