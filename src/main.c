#include "raylib.h"
#include <math.h>

// Soft-body nodes
typedef struct { Vector2 pos, old; float damage; } Node;
Node nodes[6];

// Rigid car properties
float rotation = 0, speed = 0;

void UpdatePhysics() {
    // === DRIVING ===
    float steer = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * 0.05f;
    float accel = (IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN)) * 5.0f;
    
    speed += accel;
    speed *= 0.98f;
    rotation += steer * speed * 0.02f;
    
    // Move soft-body nodes
    for(int i=0;i<6;i++) {
        nodes[i].pos.x += sin(rotation) * speed;
        nodes[i].pos.y += cos(rotation) * speed;
        
        // Verlet integration
        Vector2 vel = {nodes[i].pos.x - nodes[i].old.x, nodes[i].pos.y - nodes[i].old.y};
        nodes[i].old = nodes[i].pos;
        nodes[i].pos.x += vel.x;
        nodes[i].pos.y += vel.y + 500 * GetFrameTime() * GetFrameTime();
    }
    
    // === SPRING CONSTRAINTS ===
    int springs[6][2] = {{0,1},{1,2},{2,3},{3,0},{0,2},{1,3}};
    for(int s=0;s<6;s++) {
        Vector2 delta = {nodes[springs[s][1]].pos.x - nodes[springs[s][0]].pos.x,
                        nodes[springs[s][1]].pos.y - nodes[springs[s][0]].pos.y};
        float dist = sqrt(delta.x*delta.x + delta.y*delta.y);
        float target = 0.5f;
        float stiffness = 0.5f * (1.0f - (nodes[springs[s][0]].damage + nodes[springs[s][1]].damage) * 0.5f);
        float diff = (target - dist) / dist * stiffness;
        nodes[springs[s][0]].pos.x -= delta.x * diff;
        nodes[springs[s][0]].pos.y -= delta.y * diff;
        nodes[springs[s][1]].pos.x += delta.x * diff;
        nodes[springs[s][1]].pos.y += delta.y * diff;
    }
    
    // === COLLISION + DAMAGE ===
    for(int i=0;i<6;i++) {
        if(nodes[i].pos.x < 50 || nodes[i].pos.x > 750 || 
           nodes[i].pos.y < 50 || nodes[i].pos.y > 550) {
            nodes[i].damage += 0.05f;
            if(nodes[i].damage > 1.0f) nodes[i].damage = 1.0f;
        }
        if(nodes[i].pos.x < 50) nodes[i].pos.x = 50;
        if(nodes[i].pos.x > 750) nodes[i].pos.x = 750;
        if(nodes[i].pos.y < 50) nodes[i].pos.y = 50;
        if(nodes[i].pos.y > 550) nodes[i].pos.y = 550;
    }
}

void Draw() {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    
    // Draw soft-body car
    int springs[6][2] = {{0,1},{1,2},{2,3},{3,0},{0,2},{1,3}};
    for(int s=0;s<6;s++) {
        Color c = RED;
        float avgDamage = (nodes[springs[s][0]].damage + nodes[springs[s][1]].damage) * 0.5f;
        c.g = (unsigned char)(255 * (1 - avgDamage));
        c.b = (unsigned char)(255 * (1 - avgDamage));
        DrawLineV(nodes[springs[s][0]].pos, nodes[springs[s][1]].pos, c);
    }
    for(int i=0;i<6;i++) {
        Color c = YELLOW;
        c.g = (unsigned char)(255 * (1 - nodes[i].damage));
        c.b = (unsigned char)(255 * (1 - nodes[i].damage));
        DrawCircleV(nodes[i].pos, 6, c);
    }
    
    // Draw wheels
    DrawCircleV(nodes[4].pos, 8, BLACK);
    DrawCircleV(nodes[5].pos, 8, BLACK);
    
    DrawText("Arrow keys drive | R reset | Crash into walls = damage", 10, 10, 20, WHITE);
    DrawText(TextFormat("Speed: %.1f | Damage: %.0f%%", speed, nodes[0].damage * 100), 10, 35, 20, WHITE);
    DrawFPS(10, 60);
    
    EndDrawing();
}

int main() {
    InitWindow(800, 600, "RagDrive - game prototype");
    SetTargetFPS(60);
    
    // Init car
    for(int i=0;i<6;i++) {
        nodes[i].pos = nodes[i].old = (Vector2){400,300};
        nodes[i].damage = 0;
    }
    
    while(!WindowShouldClose()) {
        if(IsKeyPressed(KEY_R)) {
            for(int i=0;i<6;i++) {
                nodes[i].pos = nodes[i].old = (Vector2){400,300};
                nodes[i].damage = 0;
            }
            rotation = 0;
            speed = 0;
        }
        
        UpdatePhysics();
        Draw();
    }
    
    CloseWindow();
    return 0;
}
