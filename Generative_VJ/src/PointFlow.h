#pragma once
#ifndef PointFlow_h
#define PointFlow_h
#define BUFFER_SIZE 256

#include "SceneElement.cpp"

class PointFlow : public SceneElement {
public:
	PointFlow();
	void update() override;
	void draw() override;

	void audioIn(ofSoundBuffer& input) override;
private:
	ofFbo			centerFbo, sideFbo;

	// AudioInput
	float smoothedVol;
	float scaledVol;
	vector <float> sound;
	ofSoundStream soundStream;

	ofEasyCam cam;

	int base_radius;
	int number_of_satellite;

	vector<glm::vec3> base_noise_seed_list;
	vector<vector<glm::vec3>> satellite_location_list;
	vector<vector<glm::vec2>> satellite_noise_seed_list;
	vector<glm::vec3> location_list;
	vector<glm::vec3> velocity_list;

	ofMesh face, frame;
	ofMesh ringface, ringframe;
};
#endif