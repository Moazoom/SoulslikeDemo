#include "raylib.h"
#include "rlgl.h"

Camera3D camera;

int main(void) {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // making window an stuff
    InitWindow(screenWidth, screenHeight, "heueuhuhuhhhhh");

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

    // 3d camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 4.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 90.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };

    // Lock and hide cursor for free-look camera
    DisableCursor();

    // asset loading
    Model playerModel = LoadModel("resources/player.obj");
    Vector3 playerPosition = {0.0f, 0.0f, 0.0f};

    // gaem loop
    while (!WindowShouldClose()) {
        // vamera time
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // 2. Begin drawing frame
        BeginDrawing();

            ClearBackground(BLACK);

            // Switch to 3D world space
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

            EndMode3D(); // Back to 2D UI space

            DrawText("pluhhh", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    // Clean up window and OpenGL resources
    CloseWindow();
    return 0;
}