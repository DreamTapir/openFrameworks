#include "lattice.h"

lattice::lattice() {
	lwidth = 10;
	lcolor = ofColor(0, 0, 0);
}

void lattice::draw() {
	ofSetColor(lcolor);
	ofCircle(pos.x, pos.y, 5);
}