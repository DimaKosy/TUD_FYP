#include <iostream>
#include <raylib.h>


int main(int argc, char ** arg){
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 2*512;
    const int screenHeight = 2*512;
    int x = 0;

    SetTraceLogLevel(LOG_ERROR); 
    InitWindow(screenWidth, screenHeight, "TED");
    srand(0);

    Image n = GenImageColor(screenWidth/2, screenHeight/2, WHITE);
    Texture2D t = LoadTextureFromImage(n);
    

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        x++;
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground((Color){ 0, 0, 0, 255 });
            
            DrawTexture(t,x,0,WHITE);
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