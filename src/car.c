#include "car.h"
#include "raymath.h"
#include <math.h>

Node nodes[NODES];
float speed = 0;
float rotation = 0;
Model carModel;

void InitCar() {
    Vector3 offsets[] = {
        {-0.5,0.0,-0.8}, { 0.5,0.0,-0.8}, { 0.5,0.0,0.8}, {-0.5,0.0,0.8},
        {-0.5,0.4,-0.8}, { 0.5,0.4,-0.8}, { 0.5,0.4,0.8}, {-0.5,0.4,0.8}
    };
    for(int i = 0; i < NODES; i++) {
        nodes[i].pos = offsets[i];
        nodes[i].vel = (Vector3){0, 0, 0};
        nodes[i].damage = 0;
    }
    speed = 0;
    rotation = 0;
}

Vector3 GetCarCenter() {
    Vector3 center = {0, 0, 0};
    for(int i = 0; i < NODES; i++) {
        center.x += nodes[i].pos.x;
        center.y += nodes[i].pos.y;
        center.z += nodes[i].pos.z;
    }
    center.x /= NODES;
    center.y /= NODES;
    center.z /= NODES;
    return center;
}

void UpdatePhysics() {
    float dt = GetFrameTime();
    if(dt > 0.033f) dt = 0.033f;

    float steer = (IsKeyDown(KEY_LEFT) - IsKeyDown(KEY_RIGHT)) * 0.05f;
    float accel = (IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN)) * 15.0f;

    speed += accel * dt;
    speed *= 0.98f;

    float effectiveSpeed = speed;
    if(fabsf(effectiveSpeed) < 1.0f && (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)))
        effectiveSpeed = (speed >= 0 ? 1.0f : -1.0f);

    rotation += steer * effectiveSpeed * dt;

    // Gravity
    for(int i = 0; i < NODES; i++)
        nodes[i].vel.y -= 15.0f * dt;

    // Euler integration
    for(int i = 0; i < NODES; i++) {
        nodes[i].pos.x += nodes[i].vel.x * dt;
        nodes[i].pos.y += nodes[i].vel.y * dt;
        nodes[i].pos.z += nodes[i].vel.z * dt;
    }

    // Spring constraints
    int springs[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
    for(int iter = 0; iter < 10; iter++) {
        for(int s = 0; s < 12; s++) {
            Vector3 delta = {
                nodes[springs[s][1]].pos.x - nodes[springs[s][0]].pos.x,
                nodes[springs[s][1]].pos.y - nodes[springs[s][0]].pos.y,
                nodes[springs[s][1]].pos.z - nodes[springs[s][0]].pos.z
            };
            float dist = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
            float target = (springs[s][0] < 4 && springs[s][1] < 4) ? 1.0f : 0.8f;
            float stiffness = 0.5f;
            float diff = (target - dist) / dist * stiffness;

            nodes[springs[s][0]].pos.x -= delta.x * diff;
            nodes[springs[s][0]].pos.y -= delta.y * diff;
            nodes[springs[s][0]].pos.z -= delta.z * diff;
            nodes[springs[s][1]].pos.x += delta.x * diff;
            nodes[springs[s][1]].pos.y += delta.y * diff;
            nodes[springs[s][1]].pos.z += delta.z * diff;
        }
    }

    // Ground collision
    for(int i = 0; i < NODES; i++) {
        if(nodes[i].pos.y < 0.0f) {
            nodes[i].pos.y = 0.0f;
            if(nodes[i].vel.y < 0) nodes[i].vel.y = 0;
        }
    }

    // Apply driving
    for(int i = 0; i < NODES; i++) {
        nodes[i].pos.x += -sin(rotation) * speed * dt;
        nodes[i].pos.z += -cos(rotation) * speed * dt;
    }
}

void DrawCar() {
    Vector3 carPos = GetCarCenter();
    float angleY = rotation * RAD2DEG;
    carModel.transform = MatrixMultiply(
        MatrixRotateX(DEG2RAD * -90),
        MatrixRotateY(DEG2RAD * (angleY + 180.0f))
    );
    DrawModel(carModel, carPos, 1.0f, WHITE);
}
