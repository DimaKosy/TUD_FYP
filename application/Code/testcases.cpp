/*
C21376161
These are a mess and where only used for quick testing
*/

#include "TED_TEST.hpp"

#define P_N 4

int main(int argc, char ** arg){
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 128*4;
    const int screenHeight = 128*4;

    SetTraceLogLevel(LOG_ERROR); 
    InitWindow(screenWidth, screenHeight, "TED");
    srand(8);

    // Setting world parameters
    fixedWorld World(screenWidth, screenHeight, P_N, P_N, time(0));
    // World.genNewWorld();


    
    Vector2 exclude2[] = {
        {1,1},
        {2,1}
    };
    World.purge_grids_demo(exclude2, 2);   
    
    

    SetTargetFPS(240);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        if (IsKeyDown(KEY_UP)){
            World.moveAllPlates((Vector2){0,-60 * GetFrameTime()});
        }
        if (IsKeyDown(KEY_DOWN)){
            World.moveAllPlates((Vector2){0,60 * GetFrameTime()});
        }

        if (IsKeyDown(KEY_RIGHT)){
            World.moveAllPlates((Vector2){60 * GetFrameTime(),0});
        }
        if (IsKeyDown(KEY_LEFT)){
            World.moveAllPlates((Vector2){-60 * GetFrameTime(),0});
        }

        if (IsKeyPressed(KEY_SPACE)){
            // World.translateWorld(-30,0);
            World.moveStepPlates();
        }

        if (IsKeyPressed(KEY_LEFT_ALT)){
            Vector2 exclude[] = {
                {1,1}
            };
            World.purge_grids_demo(exclude,1);
        }

        
        

        World.updatePlatePositions();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground((Color){ 0, 0, 0, 255 });
            World.render();
            DrawText(std::to_string(GetFPS()).c_str(),0,0,20, WHITE);
            // DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
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