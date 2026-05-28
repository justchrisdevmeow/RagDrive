#pragma once
#include "raylib.h"

typedef enum {
    CAM_FIRST_PERSON = 1,
    CAM_THIRD_CLOSE  = 2,
    CAM_THIRD_FAR    = 3
} CamMode;

extern Camera3D cam;
extern CamMode camMode;

void InitGameCamera();
void UpdateGameCamera();
