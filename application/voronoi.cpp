#include <raylib.h>
#include <stdlib.h>
#include <math.h>

#define NUM_SITES 30
#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600

float distance(Vector2 p1, Vector2 p2) {
    return sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

int main(void) {
    InitWindow(IMAGE_WIDTH, IMAGE_HEIGHT, "Voronoi Texture in Raylib");
    SetTargetFPS(60);

    Vector2 sites[NUM_SITES];
    Color colors[NUM_SITES];

    // Randomly generate site positions and colors
    for (int i = 0; i < NUM_SITES; i++) {
        sites[i].x = GetRandomValue(0, IMAGE_WIDTH);
        sites[i].y = GetRandomValue(0, IMAGE_HEIGHT);
        colors[i] = (Color){GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255};
    }

    // Create an image to store the Voronoi texture
    Image image = GenImageColor(IMAGE_WIDTH, IMAGE_HEIGHT, WHITE);

    // Create the Voronoi texture
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            Vector2 pixelPos = {x, y};
            int closestSiteIndex = 0;
            float closestDistance = distance(pixelPos, sites[0]);
            
            // Find the closest site to the current pixel
            for (int i = 1; i < NUM_SITES; i++) {
                float dist = distance(pixelPos, sites[i]);
                if (dist < closestDistance) {
                    closestDistance = dist;
                    closestSiteIndex = i;
                }
            }

            // Set the pixel color based on the closest site
            ImageDrawPixel(&image, x, y, colors[closestSiteIndex]);
        }
    }

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(texture, 0, 0, WHITE);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
