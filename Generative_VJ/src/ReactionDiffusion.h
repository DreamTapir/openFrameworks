#pragma once
#ifndef ReactionDiffusion_h
#define ReactionDiffusion_h
#define BUFFER_SIZE 256

#include "SceneElement.cpp"
#include "ofxReactionDiffusion.h"

class ReactionDiffusion : public SceneElement {
public:
	ReactionDiffusion();
	void update() override;
	void draw() override;

	void audioIn(ofSoundBuffer& input) override;
private:
	void setBoxToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float size, ofColor face_color);
	void setBoxToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float height, float width, float depth, ofColor face_colorr);

	ofFbo			centerFbo, sideFbo;

	// AudioInput
	float smoothedVol;
	float scaledVol;
	vector <float> sound;
	ofSoundStream soundStream;

	float scale;

	ofxReactionDiffusion rd;
	ofVboMesh mesh;
	ofEasyCam cam;

	ofShader updatePosShader;

	ofMesh face, frame;
	vector<glm::vec3> location_list;
	vector<ofColor> color_list;

	ofLight light;
};
#endif