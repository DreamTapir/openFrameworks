#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	ofBackground(255);
	ofEnableDepthTest();

	this->line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

	fft = ofxFft::create(BUFFER_SIZE, OF_FFT_WINDOW_RECTANGULAR, OF_FFT_FFTW);
	audioInput = new float[BUFFER_SIZE];
	fftOutput = new float[fft->getBinSize()];
	ofSoundStreamSetup(0, 1, this, 44100, BUFFER_SIZE, 4);

	color = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	peak();

	ofSeedRandom(39);

	this->face.clear();
	this->line.clear();

	float phi_deg_step = 2;
	float theta_deg_step = 4;

	float R = 250;
	float base_r = R * 0.25;
	float r = 0;

	for (float phi_deg = 0; phi_deg < 360; phi_deg += phi_deg_step) {

		for (float theta_deg = 0; theta_deg < 360; theta_deg += theta_deg_step) {

			auto index = this->face.getNumVertices();
			vector<glm::vec3> vertices;

			r = ofMap(ofNoise(glm::vec4(this->make_point(R, base_r, theta_deg - theta_deg_step * 0.5, phi_deg - phi_deg_step * 0.5) * 0.006, ofGetFrameNum() * 0.03 + ofGetFrameNum() * 0.03 * fftOutput[25] * 10)), 0, 1, base_r * 0.2, base_r * 2);
			vertices.emplace_back(glm::vec3(this->make_point(R, r, theta_deg - theta_deg_step * 0.5, phi_deg - phi_deg_step * 0.5)));

			r = ofMap(ofNoise(glm::vec4(this->make_point(R, base_r, theta_deg + theta_deg_step * 0.5, phi_deg - phi_deg_step * 0.5) * 0.006, ofGetFrameNum() * 0.03 + ofGetFrameNum() * 0.03 * fftOutput[25] * 100)), 0, 1, base_r * 0.2, base_r * 2);
			vertices.emplace_back(glm::vec3(this->make_point(R, r, theta_deg + theta_deg_step * 0.5, phi_deg - phi_deg_step * 0.5)));

			r = ofMap(ofNoise(glm::vec4(this->make_point(R, base_r, theta_deg - theta_deg_step * 0.5, phi_deg + phi_deg_step * 0.5) * 0.006, ofGetFrameNum() * 0.03 + ofGetFrameNum() * 0.03 * fftOutput[25] * 30)), 0, 1, base_r * 0.2, base_r * 2);
			vertices.emplace_back(glm::vec3(this->make_point(R, r, theta_deg - theta_deg_step * 0.5, phi_deg + phi_deg_step * 0.5)));

			r = ofMap(ofNoise(glm::vec4(this->make_point(R, base_r, theta_deg + theta_deg_step * 0.5, phi_deg + phi_deg_step * 0.5) * 0.006, ofGetFrameNum() * 0.03 + ofGetFrameNum() * 0.03 * fftOutput[25] * 50)), 0, 1, base_r * 0.2, base_r * 2);
			vertices.emplace_back(glm::vec3(this->make_point(R, r, theta_deg + theta_deg_step * 0.5, phi_deg + phi_deg_step * 0.5)));

			this->face.addVertices(vertices);
			this->line.addVertices(vertices);

			this->face.addIndex(index + 0); this->face.addIndex(index + 1); this->face.addIndex(index + 3);
			this->face.addIndex(index + 0); this->face.addIndex(index + 3); this->face.addIndex(index + 2);

			this->line.addIndex(index + 0); this->line.addIndex(index + 1);
			this->line.addIndex(index + 1); this->line.addIndex(index + 3);
			this->line.addIndex(index + 0); this->line.addIndex(index + 2);
			this->line.addIndex(index + 2); this->line.addIndex(index + 3);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	this->cam.begin();

	ofSetColor(255);
	this->line.draw();

	ofSetColor(ofColor::fromHsb(color, 255,255));
	this->face.draw();

	this->cam.end();

	float ht = 1000.0f;
	int binSize = fft->getBinSize();
	for (int i = 0; i < binSize; i++) {
		float barY = fftOutput[i] * ht;
		ofLine(i, ofGetHeight(), i, ofGetHeight() - barY);
	}
}

void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {
	memcpy(audioInput, input, sizeof(float) * bufferSize);
	fft->setSignal(audioInput);
	memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * fft->getBinSize());
}

void ofApp::peak() {
	float peak = 0;
	int binSize = fft->getBinSize();
	for (int i = 0; i < binSize; i++) {
		if (peak < fftOutput[i]) {
			peak = fftOutput[i];
		}
	}
	color = 170 - peak * 5000;
}

//--------------------------------------------------------------
glm::vec3 ofApp::make_point(float R, float r, float u, float v) {
	
	u *= DEG_TO_RAD;
	v *= DEG_TO_RAD;

	auto x = (R + r * cos(u)) * cos(v);
	auto y = (R + r * cos(u)) * sin(v);
	auto z = r * sin(u);

	return glm::vec3(x, y, z);
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
