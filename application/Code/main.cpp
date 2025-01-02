#include <iostream>
#include <raylib.h>
#include "world.cpp"

int main(int argc, char ** arg){
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 512*1;
    const int screenHeight = 512*1;

    SetTraceLogLevel(LOG_ERROR); 
    InitWindow(screenWidth, screenHeight, "TED");
    srand(0);

    // Setting world parameters
    infiniteWorld World(screenWidth, screenHeight, 4, 4);
    // World.genNewWorld();

    // World.purge_grids_demo(1,1);    

    SetTargetFPS(1);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // if (IsKeyDown(KEY_UP)){
        //     World.moveAllPlates((Vector2){0,-10});
        // }
        // if (IsKeyDown(KEY_DOWN)){
        //     World.moveAllPlates((Vector2){0,10});
        // }

        // if (IsKeyDown(KEY_RIGHT)){
        //     World.moveAllPlates((Vector2){10,0});
        // }
        // if (IsKeyDown(KEY_LEFT)){
        //     World.moveAllPlates((Vector2){-10,0});
        // }

        if (IsKeyDown(KEY_SPACE)){
            World.translateWorld(10,0);
            // World.moveStepPlates();
        }

        
        

        // World.updatePlatePositions();

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