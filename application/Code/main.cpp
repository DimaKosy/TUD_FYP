#include "TED.hpp"

#define P_N 4
#define CAM_SPEED 500



int main(int argc, char ** arg){
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
    InitWindow(screenWidth, screenHeight, "TED");
    srand(8);

    // Setting world parameters
    fixedWorld World(screenWidth, screenHeight, P_N, P_N, time(0));
    // World.genNewWorld();


    
    Vector2 exclude[] = {
        {1,1},
        {1,2},
        {1,3},
        // {2,1},
        // {2,2},
        // {2,3},
        // {3,1},
        // {3,2},
        // {3,3},
        // {P_N-1,P_N-1}
    };
    // World.purge_grids_demo(exclude, sizeof(exclude) / sizeof(exclude[0]));   


    plate * followPlate = World.getGridCell(exclude[0].x,exclude[0].y)->getPlates().front();
    
    // for(int i = 0; i < 500; i++){
    //     World.moveStepPlates();
    //     World.updatePlatePositions();
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
            // World.moveAllPlates((Vector2){CAM_SPEED * GetFrameTime(),CAM_SPEED * GetFrameTime()});
            TimeStep++;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL)){
            // World.debugPlateVertexs();
            World.moveStepPlates();
            // World.moveAllPlates((Vector2){CAM_SPEED * GetFrameTime(),CAM_SPEED * GetFrameTime()});
            TimeStep++;
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
            DrawText(std::to_string(TimeStep).c_str(),0,20,20, WHITE);

            mouseColor = followPlate->internalTest(GetMousePosition())? RED:GREEN;

            // DrawCircle(GetMousePosition().x, GetMousePosition().y,15, mouseColor);
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