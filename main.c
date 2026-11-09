#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <stdlib.h>

Camera3D camera;

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // making window an stuff
    InitWindow(screenWidth, screenHeight, "heueuhuhuhhhhh");
    DisableCursor();

    // 3d camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 4.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // store camera angle radians as a vector lol
    Vector2 cameraAngle = { 0 };
    float cameraDistance = 10.0f;
    Vector3 cameraForwards = { 0 };
    Vector3 cameraRight = { 0 };

    float mouseSensitivity = 0.001f;

    //skybox
    Image skyboxImage = LoadImage("resources/skybox/skybox_cross.png");
    TextureCubemap skyboxTexture = LoadTextureCubemap(skyboxImage, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(skyboxImage);
    
    Shader skyboxShader = LoadShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    
    skyboxShader.locs[SHADER_LOC_MAP_CUBEMAP] = GetShaderLocation(skyboxShader, "environmentMap");
    SetShaderValue(skyboxShader, GetShaderLocation(skyboxShader, "environmentMap"), (int[1]){ MATERIAL_MAP_CUBEMAP }, SHADER_UNIFORM_INT);
    
    Model skybox = LoadModelFromMesh(GenMeshCube(2.0f, 2.0f, 2.0f));
    skybox.materials[0].shader = skyboxShader;
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = skyboxTexture;

    // player
    Model playerModel = LoadModel("resources/player.obj");
    Vector3 playerPosition = {0.0f, 0.0f, 0.0f};
    Vector3 playerCenter = { 0 };
    float playerSpeed = 0.05f;

    // world stars
    Texture2D star = LoadTexture("resources/star.png");
    Vector3 starPositions[100] = { 0 };
    for (int i = 0; i < 100; i++){
        Vector3 temp;
        temp.x = (rand() % 200) - 100;
        temp.y = (rand() % 20);
        temp.z = (rand() % 200) - 100;
        starPositions[i] = temp;
    }

    // gaem loop
    while (!WindowShouldClose()) {
        // camera time
        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // caputre mouse
        cameraAngle.x -= GetMouseDelta().x * mouseSensitivity;
        cameraAngle.y += GetMouseDelta().y * mouseSensitivity;

        // figure out front and right relative to camera
        cameraForwards = Vector3Normalize((Vector3){sinf(cameraAngle.x), 0.0f, cosf(cameraAngle.x)});
        cameraRight = Vector3Normalize((Vector3){cosf(cameraAngle.x), 0.0f, -sinf(cameraAngle.x)});

        // move player forward relative to camera
        if (IsKeyDown(KEY_W)) playerPosition = Vector3Add(playerPosition, Vector3Scale(cameraForwards, playerSpeed));
        if (IsKeyDown(KEY_A)) playerPosition = Vector3Add(playerPosition, Vector3Scale(cameraRight, playerSpeed));
        if (IsKeyDown(KEY_S)) playerPosition = Vector3Subtract(playerPosition, Vector3Scale(cameraForwards, playerSpeed));
        if (IsKeyDown(KEY_D)) playerPosition = Vector3Subtract(playerPosition, Vector3Scale(cameraRight, playerSpeed));

        // update camera pos and angle
        playerCenter = (Vector3){playerPosition.x, playerPosition.y + 3.0f, playerPosition.z};
        camera.target = playerCenter;
        camera.position.x = playerCenter.x - sinf(cameraAngle.x) * cosf(cameraAngle.y) * cameraDistance;
        camera.position.y = playerCenter.y + sinf(cameraAngle.y) * cameraDistance;
        camera.position.z = playerCenter.z - cosf(cameraAngle.x) * cosf(cameraAngle.y) * cameraDistance;

        // drawing time
        BeginDrawing();

            ClearBackground(BLACK);

            // 3d stuff
            BeginMode3D(camera);

                //skybox drawing behind deapth buffer
                rlDisableBackfaceCulling();
                rlDisableDepthMask();
                DrawModel(skybox, camera.position, 1.0f, WHITE);
                rlEnableDepthMask();
                rlEnableBackfaceCulling();

                // drawing other things
                DrawGrid(20, 1.0f);
                DrawModel(playerModel, playerPosition, 1.0f, RED);
                DrawModelWires(playerModel, playerPosition, 1.0f, WHITE);

                // Origin axes helper (Red=X, Green=Y, Blue=Z)
                DrawLine3D((Vector3){0}, (Vector3){3,0,0}, RED);
                DrawLine3D((Vector3){0}, (Vector3){0,3,0}, GREEN);
                DrawLine3D((Vector3){0}, (Vector3){0,0,3}, BLUE);

                // drawing stars
                for (int i = 0; i < 100; i++){
                    DrawBillboard(camera, star, starPositions[i], 0.25f, WHITE);
                }

            EndMode3D(); // Back to 2D UI space

            DrawText("pluhhh", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    // Clean up window and OpenGL resources
    CloseWindow();
    return 0;
}