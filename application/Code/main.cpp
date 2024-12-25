#include <iostream>
#include <raylib.h>
#include "world.cpp"

int main(int argc, char ** arg){
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 512;
    const int screenHeight = 512;

    SetTraceLogLevel(LOG_ERROR); 
    InitWindow(screenWidth, screenHeight, "TED");
    srand(0);

    // Setting world parameters
    world World(screenWidth, screenHeight, 3, 3);
    // World.genNewWorld();

    // World.purge_grids_demo(4);    
    

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        World.moveAllPlates((Vector2){-1,-1});

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