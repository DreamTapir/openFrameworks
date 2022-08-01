#pragma once
#ifndef _BOID
#define _BOID

class boids {
private:
    float r1;
    float r2;
    float r3;

    float x, y;
    float vx, vy;
    boids** others;
    int num_others;

public:
    boids(float x, float y, float vx, float vy, boids* others[], int num_others);
    void boo();
    void update();
    void draw();

    void sep();
    void align();
    void coh();
};

#endif