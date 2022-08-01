#pragma once

#include "ofMain.h"
#include "math.h"
#include "lattice.h"

# define DIM 1280


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void solver();
		void collision();
		void force();
		void streaming();
		void bounce_back_boundary();
		void physics();

		void timer();

		void mousePressed(int x, int y, int button);

		double u[DIM][DIM], v[DIM][DIM], rho[DIM][DIM];
		double f[9][DIM][DIM], f0[9][DIM][DIM], ftmp[9][DIM][DIM], cx[9], cy[9];
		double time = 0., tau = 0.54, gx = 0.00001, gy = 0.0, mu = (tau - 0.5) / 3, tmp, u2, rotv;
		unsigned int nx = 128, ny = 128, i, j, k, in, jn;

		lattice lattice[DIM][DIM];

		int counter = 0;
		float lastUpdate = 0;
};
