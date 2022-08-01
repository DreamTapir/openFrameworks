#include "CustomRect.h"

CustomRect::CustomRect(int _num) {
	num = _num;

	float notes[] = { 1.0, 5.0 / 4.0, 4.0 / 3.0, 3.0 / 2.0 };
	float base[] = { 8.0, 4.0, 2.0, 1.0, 0.5 };

	lfo = ofRandom(0.5, 2.0);
}

void CustomRect::update() {
	radius = sin(ofGetElapsedTimef() * 10.0 * lfo) * 10 + 4;

	float dist = ofDist(getPosition().x, getPosition().y, ofGetWidth() / 2, ofGetHeight() / 2);

}