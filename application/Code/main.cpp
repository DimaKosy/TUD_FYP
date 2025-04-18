#include "TED.hpp"


#define CAM_SPEED 100



int main(int argc, char * arg[]){

    // P_N = std::stoi(arg[1]);
    //  = std::stoi(arg[2]);
    // P_N = std::stoi(arg[3]);




    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 128*8;
    const int screenHeight = 128*8;
    bool render_bool = true;
    int TimeStep = 0;
    Color mouseColor = WHITE;

    clock_t start, stop;

    start = clock();

    printf("Start %d\n",start);

    SetTraceLogLevel(LOG_ERROR); 
    InitWindow(32, 32, "TED");
    srand(32);

    // Setting world parameters
    fixedWorld * World = new fixedWorld(screenWidth, screenHeight, P_N, P_N, time(0));
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    // BeginTextureMode(target);

    
    //for debugging
    Vector2 exclude[] = {
        // {0,0},
        // {1,0},
        // {1,1},
        // {1,2},
        // {1,3},
        // {2,1},
        // {2,2},
        // {2,3},
        // {3,1},
        // {3,2},
        // {3,3},
        // {0,P_N-1}
    };
    // World->purge_grids_demo(exclude, sizeof(exclude) / sizeof(exclude[0]));   


    // plate * followPlate = World->getGridCell(exclude[0].x,exclude[0].y)->getPlates().front();
    
    // for(int i = 0; i < 100; i++){
    //     World->moveStepPlates();
    //     World->updatePlatePositions();
    //     TimeStep++;
    // }

    stop = clock();
    printf("End %d\n",stop);
    printf("Total %d\n",stop - start);

    SetTargetFPS(240);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------

        if (IsKeyDown(KEY_UP)){
            World->moveAllPlates((Vector2){0,-CAM_SPEED * GetFrameTime()});
        }
        if (IsKeyDown(KEY_DOWN)){
            World->moveAllPlates((Vector2){0,CAM_SPEED * GetFrameTime()});
        }

        if (IsKeyDown(KEY_RIGHT)){
            World->moveAllPlates((Vector2){CAM_SPEED * GetFrameTime(),0});
        }
        if (IsKeyDown(KEY_LEFT)){
            World->moveAllPlates((Vector2){-CAM_SPEED * GetFrameTime(),0});
        }

        if (IsKeyPressed(KEY_SPACE)){
            // World->translateWorld(-30,0);
            // World->debugPlateVertexs();
            World->moveStepPlates();
            // World->moveAllPlates((Vector2){CAM_SPEED * GetFrameTime(),CAM_SPEED * GetFrameTime()});
            TimeStep++;
        }

        if (IsKeyPressed(KEY_RIGHT_ALT)){
            render_bool = !render_bool;
        }

        start = clock();
        World->moveStepPlates();
        TimeStep++;


        if(TimeStep%80 == 0){
            // BeginTextureMode(target);
            // ClearBackground(RAYWHITE);
            //     World->render();
            // EndTextureMode();


            Image img = LoadImageFromTexture(target.texture);

            ImageBlurGaussian(&img, 2);

            ExportImage(img, "output.png");
            UnloadImage(img);
            delete World;
            World = new fixedWorld(screenWidth, screenHeight, P_N, P_N, time(0));
            // World->purge_grids_demo(exclude, sizeof(exclude) / sizeof(exclude[0]));

            BeginTextureMode(target);
    
            ClearBackground((Color){ 0, 0, 0, 255 });

            EndTextureMode();
        }

        World->updatePlatePositions();

        stop = clock();

        // Draw
        //----------------------------------------------------------------------------------

        
        
        BeginTextureMode(target);
    
        // ClearBackground((Color){ 0, 0, 0, 255 });
        if(render_bool){
            World->render();
        }
        
        
        EndTextureMode();


        BeginDrawing();
        ClearBackground(BLACK);
        // DrawTexture(target.texture, 0, 0, WHITE); // Draw the texture to screen
        // DrawTexturePro(target.texture, 
        //     {0, 0, (float)target.texture.width, -(float)target.texture.height }, 
        //     {0, 0, (float)target.texture.width, (float)target.texture.height}, 
        //     (Vector2){0,0}, 
        //     0.0f, WHITE);

        // DrawText(std::to_string(TimeStep).c_str(),0,0,20, WHITE);
        // DrawText(std::to_string(GetFPS()).c_str(),0,20,20, WHITE);
        EndDrawing();
        
        // printf("Time %dms\n",stop - start);
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


//convert to
//x_c * i_x + i_y = i

//convert from
// i / x_c = i_x
// i - i_x * x_ c = i_y 