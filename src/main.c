#include "raylib.h"
#include <math.h>

#define NODES 8

typedef struct { Vector3 pos, vel; float damage; } Node;
Node nodes[NODES];

float speed = 0, rotation = 0;
Camera3D cam = { 0 };
Model carModel, mapModel;

void InitCar() {
    Vector3 offsets[] = {
        {-0.5,0.0,-0.8}, { 0.5,0.0,-0.8}, { 0.5,0.0,0.8}, {-0.5,0.0,0.8},
        {-0.5,0.4,-0.8}, { 0.5,0.4,-0.8}, { 0.5,0.4,0.8}, {-0.5,0.4,0.8}
    };
    for(int i=0;i<NODES;i++) {
        nodes[i].pos = offsets[i];
        nodes[i].vel = (Vector3){0,0,0};
        nodes[i].damage = 0;
    }
    
    speed = 0;
    rotation = 0;
    
    cam.position = (Vector3){5,3,5};
    cam.target = (Vector3){0,0,0};
    cam.up = (Vector3){0,1,0};
    cam.fovy = 60;
}

void UpdatePhysics() {
    float dt = GetFrameTime();
    if(dt > 0.033f) dt = 0.033f;
    
    float steer = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * 0.05f;
    float accel = (IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN)) * 15.0f;
    
    speed += accel * dt;
    speed *= 0.98f;
    rotation += steer * speed * dt;
    
    // GRAVITY
    for(int i=0;i<NODES;i++) {
        nodes[i].vel.y -= 15.0f * dt;
    }
    
    // euler integration
    for(int i=0;i<NODES;i++) {
        nodes[i].pos.x += nodes[i].vel.x * dt;
        nodes[i].pos.y += nodes[i].vel.y * dt;
        nodes[i].pos.z += nodes[i].vel.z * dt;
    }
    
    // sprinng constraints
    int springs[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
    for(int iter=0; iter<10; iter++) {
        for(int s=0;s<12;s++) {
            Vector3 delta = { nodes[springs[s][1]].pos.x - nodes[springs[s][0]].pos.x,
                             nodes[springs[s][1]].pos.y - nodes[springs[s][0]].pos.y,
                             nodes[springs[s][1]].pos.z - nodes[springs[s][0]].pos.z };
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
    
    // grounnd collision
    for(int i=0;i<NODES;i++) {
        if(nodes[i].pos.y < 0.0f) {
            nodes[i].pos.y = 0.0f;
            if(nodes[i].vel.y < 0) nodes[i].vel.y = 0;
        }
    }
    
    // apply driving
    for(int i=0;i<NODES;i++) {
        nodes[i].pos.x += sin(rotation) * speed * dt;
        nodes[i].pos.z += cos(rotation) * speed * dt;
    }
    
    cam.target = nodes[0].pos;
}

void DrawCar() {
    // calc car center position (average of all nodes)
    Vector3 carPos = {0,0,0};
    for(int i=0;i<NODES;i++) {
        carPos.x += nodes[i].pos.x;
        carPos.y += nodes[i].pos.y;
        carPos.z += nodes[i].pos.z;
    }
    carPos.x /= NODES;
    carPos.y /= NODES;
    carPos.z /= NODES;
    
    // calc rotation angle (from driving direction)
    float angleY = rotation * RAD2DEG;
    
    // draw 3D model
    DrawModelEx(carModel, carPos, (Vector3){0,1,0}, angleY, (Vector3){1,1,1}, WHITE);
}

int main() {
    InitWindow(1024, 768, "RagDrive - Soft Body Car");
    SetTargetFPS(60);
    
    // Load models
    carModel = LoadModel("car.glb");
    mapModel = LoadModel("map.glb");
    
    InitCar();
    
    while(!WindowShouldClose()) {
        if(IsKeyPressed(KEY_R)) InitCar();
        UpdatePhysics();
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        BeginMode3D(cam);
        
        // draw map
        DrawModel(mapModel, (Vector3){0,0,0}, 1.0f, WHITE);
        
        // draw car
        DrawCar();
        
        // Optional: draw wireframe debug (comment out if not needed)
        // int springs[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
        // for(int s=0;s<12;s++) {
        //     DrawLine3D(nodes[springs[s][0]].pos, nodes[springs[s][1]].pos, RED);
        // }
        
        EndMode3D();
        
        DrawText("Arrow keys drive | R reset", 10, 10, 20, WHITE);
        DrawText(TextFormat("Speed: %.1f", speed), 10, 35, 20, WHITE);
        DrawFPS(10, 60);
        EndDrawing();
    }
    
    UnloadModel(carModel);
    UnloadModel(mapModel);
    CloseWindow();
    return 0;
}
