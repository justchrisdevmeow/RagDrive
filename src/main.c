#include "raylib.h"
#include "raymath.h"
#include "car.h"
#include "camera.h"

Model mapModel;

int main() {
    InitWindow(1024, 768, "RagDrive - Soft Body Car");
    SetTargetFPS(60);

    carModel = LoadModel("car.glb");
    mapModel  = LoadModel("map.glb");
    mapModel.transform = MatrixRotateX(DEG2RAD * -90);

    InitCar();
    InitGameCamera();

    while(!WindowShouldClose()) {
        // Input
        if(IsKeyPressed(KEY_R))     InitCar();
        if(IsKeyPressed(KEY_ONE))   camMode = CAM_FIRST_PERSON;
        if(IsKeyPressed(KEY_TWO))   camMode = CAM_THIRD_CLOSE;
        if(IsKeyPressed(KEY_THREE)) camMode = CAM_THIRD_FAR;

        // Update
        UpdatePhysics();
        UpdateGameCamera();

        // Draw
        BeginDrawing();
        ClearBackground(DARKGRAY);

        BeginMode3D(cam);
            DrawModel(mapModel, (Vector3){0,0,0}, 1.0f, WHITE);
            if(camMode != CAM_FIRST_PERSON) DrawCar();
        EndMode3D();

        DrawText("Arrow keys: drive  |  R: reset  |  1/2/3: camera", 10, 10, 20, WHITE);
        DrawText(TextFormat("Speed: %.1f", speed), 10, 35, 20, WHITE);
        DrawText(TextFormat("Cam: %s",
            camMode == CAM_FIRST_PERSON ? "First Person" :
            camMode == CAM_THIRD_CLOSE  ? "Third Person (Close)" : "Third Person (Far)"
        ), 10, 60, 20, WHITE);
        DrawFPS(10, 85);

        EndDrawing();
    }

    UnloadModel(carModel);
    UnloadModel(mapModel);
    CloseWindow();
    return 0;
}
