#include "TED.hpp"

#define P_N 6
#define CAM_SPEED 500



int main(int argc, char ** arg){
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 128*6;
    const int screenHeight = 128*6;
    bool render_bool = true;

    SetTraceLogLevel(LOG_ERROR); 
    InitWindow(screenWidth, screenHeight, "TED");
    srand(8);

    // Setting world parameters
    fixedWorld World(screenWidth, screenHeight, P_N, P_N, time(0));
    // World.genNewWorld();


    
    Vector2 exclude2[] = {
        // {0,3},
        {0,2},
        {0,0},
        // {1,2}
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
            World.moveAllPlates((Vector2){0,-CAM_SPEED * GetFrameTime()});
        }
        if (IsKeyDown(KEY_DOWN)){
            World.moveAllPlates((Vector2){0,CAM_SPEED * GetFrameTime()});
        }

        if (IsKeyDown(KEY_RIGHT)){
            World.moveAllPlates((Vector2){CAM_SPEED * GetFrameTime(),0});
        }
        if (IsKeyDown(KEY_LEFT)){
            World.moveAllPlates((Vector2){-CAM_SPEED * GetFrameTime(),0});
        }

        if (IsKeyPressed(KEY_SPACE)){
            // World.translateWorld(-30,0);
            // World.debugPlateVertexs();
            World.moveStepPlates();
        }

        if (IsKeyDown(KEY_LEFT_ALT)){
            Vector2 exclude[] = {
                {1,1}
            };
            // World.debugPlateVertexs();
            World.moveStepPlates();
            // World.purge_grids_demo(exclude,1);
        }

        if (IsKeyPressed(KEY_RIGHT_ALT)){
            render_bool = !render_bool;
        }

        
        

        World.updatePlatePositions();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground((Color){ 0, 0, 0, 255 });
            if(render_bool){
                World.render();
            }
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