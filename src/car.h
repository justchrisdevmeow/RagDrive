#pragma once
#include "raylib.h"

#define NODES 8

typedef struct {
    Vector3 pos, vel;
    float damage;
} Node;

extern Node nodes[NODES];
extern float speed;
extern float rotation;
extern Model carModel;

void InitCar();
void UpdatePhysics();
void DrawCar();
Vector3 GetCarCenter();
