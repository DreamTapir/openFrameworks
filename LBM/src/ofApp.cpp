#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();

    // initial condition
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            u[i][j] = 0.;
            v[i][j] = 0.;
            rho[i][j] = 1.;
        }
    }

    cx[0] = 0.; cy[0] = 0.;
    cx[1] = 1.; cy[1] = 0.;
    cx[2] = 0.; cy[2] = 1.;
    cx[3] = -1.; cy[3] = 0.;
    cx[4] = 0.; cy[4] = -1.;
    cx[5] = 1.; cy[5] = 1.;
    cx[6] = -1.; cy[6] = 1.;
    cx[7] = -1.; cy[7] = -1.;
    cx[8] = 1.; cy[8] = -1.;

    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            u2 = u[i][j] * u[i][j] + v[i][j] * v[i][j];
            f[0][i][j] = rho[i][j] * (1. - 3. / 2. * u2) * 4. / 9.;
            for (k = 1; k < 5; k++) {
                tmp = cx[k] * u[i][j] + cy[k] * v[i][j];
                f[k][i][j] = rho[i][j] * (1. + 3. * tmp + 9. / 2. * tmp * tmp - 3. / 2. * u2) / 9.;
            }
            for (k = 5; k < 9; k++) {
                tmp = cx[k] * u[i][j] + cy[k] * v[i][j];
                f[k][i][j] = rho[i][j] * (1. + 3. * tmp + 9. / 2. * tmp * tmp - 3. / 2. * u2) / 36.;
            }

            lattice[i][j].pos = ofPoint(i * lattice[i][j].lwidth, j * lattice[i][j].lwidth);
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    ofClear(0);
    timer();
    solver();
    
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            lattice[i][j].lcolor = ofColor::fromHsb(120 + u[i][j] * v[i][j] * 5000, u[i][j] * v[i][j] * 30000000, u[i][j] * u[i][j] * v[i][j] * v[i][j] * 10000000000);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            ofFill();
            lattice[i][j].draw();
        }
    }
}

void ofApp::solver() {
    collision();
    force();
    streaming();
    bounce_back_boundary();
    physics();
}

void ofApp::collision() {
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {

            u2 = u[i][j] * u[i][j] + v[i][j] * v[i][j];
            f0[0][i][j] = rho[i][j] * (1. - 3. / 2. * u2) * 4. / 9.;

            for (k = 1; k < 5; k++) {
                tmp = cx[k] * u[i][j] + cy[k] * v[i][j];
                f0[k][i][j] = rho[i][j] * (1. + 3. * tmp + 9. / 2. * tmp * tmp - 3. / 2. * u2) / 9.;
            }

            for (k = 5; k < 9; k++) {
                tmp = cx[k] * u[i][j] + cy[k] * v[i][j];
                f0[k][i][j] = rho[i][j] * (1. + 3. * tmp + 9. / 2. * tmp * tmp - 3. / 2. * u2) / 36.;
            }
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            for (k = 0; k < 9; k++) {
                f[k][i][j] = f[k][i][j] - (f[k][i][j] - f0[k][i][j]) / tau;
            }
        }
    }
}

void ofApp::force() {
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            for (k = 0; k < 9; k++) {
                f[k][i][j] = f[k][i][j] + rho[i][j] * (cx[k] * gx + cy[k] * gy) / 6.;
            }
        }
    }
}

void ofApp::streaming() {
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            for (k = 0; k < 9; k++) {
                ftmp[k][i][j] = f[k][i][j];
            }
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            in = i + 1; if (i == nx - 1) in = 0;
            f[1][in][j] = ftmp[1][i][j];
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny - 1; j++) {
            jn = j + 1;
            f[2][i][jn] = ftmp[2][i][j];
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            in = i - 1; if (i == 0) in = nx - 1;
            f[3][in][j] = ftmp[3][i][j];
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 1; j < ny; j++) {
            jn = j - 1;
            f[4][i][jn] = ftmp[4][i][j];
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny - 1; j++) {
            in = i + 1; if (i == nx - 1) in = 0;
            jn = j + 1;
            f[5][in][jn] = ftmp[5][i][j];
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny - 1; j++) {
            in = i - 1; if (i == 0) in = nx - 1;
            jn = j + 1;
            f[6][in][jn] = ftmp[6][i][j];
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 1; j < ny; j++) {
            in = i - 1; if (i == 0) in = nx - 1;
            jn = j - 1;
            f[7][in][jn] = ftmp[7][i][j];
        }
    }
    for (i = 0; i < nx; i++) {
        for (j = 1; j < ny; j++) {
            in = i + 1; if (i == nx - 1) in = 0;
            jn = j - 1;
            f[8][in][jn] = ftmp[8][i][j];
        }
    }
}

void ofApp::bounce_back_boundary() {
    for (i = 0; i < nx; i++) {
        // Bounce Back
        f[2][i][   0] = f[4][i][   0];
        f[5][i][   0] = f[7][i][   0];
        f[6][i][   0] = f[8][i][   0];
        f[4][i][ny-1] = f[2][i][ny-1];
        f[7][i][ny-1] = f[5][i][ny-1];
        f[8][i][ny-1] = f[6][i][ny-1];
    }
}

void ofApp::physics() {
    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            rho[i][j] = f[0][i][j]; u[i][j] = 0; v[i][j] = 0;
            for (k = 1; k < 9; k++) {
                rho[i][j] = rho[i][j] + f[k][i][j];
                u[i][j] = u[i][j] + f[k][i][j] * cx[k];
                v[i][j] = v[i][j] + f[k][i][j] * cy[k];
            }
            if (rho[i][j] != 0) {
                u[i][j] = u[i][j] / rho[i][j];
                v[i][j] = v[i][j] / rho[i][j];
            }
            else {
                u[i][j] = 0; v[i][j] = 0;
            }
        }
    }
}

void ofApp::timer() {

    if (ofGetElapsedTimef() - lastUpdate > 1) {
        counter++;
        lastUpdate = ofGetElapsedTimef();

        if (0 == counter % 600) {
            setup();
        }
        else {
            if (0 == counter % 10) {
                int randomI = ofRandom(2, 126);
                int randomJ = ofRandom(2, 126);
                u[randomI][randomJ] = 0.3;
                v[randomI][randomJ] = 0.1;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    setup();
}