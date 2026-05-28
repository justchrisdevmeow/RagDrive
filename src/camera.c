#include "camera.h"
#include "car.h"
#include <math.h>

Camera3D cam = { 0 };
CamMode camMode = CAM_THIRD_CLOSE;

void InitGameCamera() {
    cam.up   = (Vector3){0, 1, 0};
    cam.fovy = 60;
}

void UpdateCamera() {
    Vector3 carPos = GetCarCenter();

    float fwd_x = -sin(rotation);
    float fwd_z = -cos(rotation);

    switch(camMode) {
        case CAM_FIRST_PERSON:
            cam.position = (Vector3){
                carPos.x + fwd_x * 0.2f,
                carPos.y + 0.3f,
                carPos.z + fwd_z * 0.2f
            };
            cam.target = (Vector3){
                carPos.x + fwd_x * 5.0f,
                carPos.y + 0.3f,
                carPos.z + fwd_z * 5.0f
            };
            break;

        case CAM_THIRD_CLOSE:
            cam.position = (Vector3){
                carPos.x - fwd_x * 3.0f,
                carPos.y + 1.5f,
                carPos.z - fwd_z * 3.0f
            };
            cam.target = carPos;
            break;

        case CAM_THIRD_FAR:
            cam.position = (Vector3){
                carPos.x - fwd_x * 8.0f,
                carPos.y + 4.0f,
                carPos.z - fwd_z * 8.0f
            };
            cam.target = carPos;
            break;
    }
}
