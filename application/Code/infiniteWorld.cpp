#ifndef WORLD_INF_C
#define WORLD_INF_C
#include "TED.hpp"

class infiniteWorld{
private:
    int map_x;
    int map_y;
    int grid_x;
    int grid_y;
    int offset_x;
    int offset_y;
    int world_x;
    int world_y;
    

    Texture2D worldMap;
    gridCell *** grid;
public:
    infiniteWorld(int map_x, int map_y, int grid_x, int grid_y);
    ~infiniteWorld();

    void translateWorld(int x, int y);

    void render();
};

infiniteWorld::infiniteWorld(int map_x, int map_y, int grid_x, int grid_y){
    Image blankMap = GenImageColor(map_x, map_y, BLACK);
    

    this->map_x = map_x;
    this->map_y = map_y;

    this->grid_x = grid_x + 2;
    this->grid_y = grid_y + 2;

    this->offset_x = 0;
    this->offset_y = 0;
    this->world_x = 0;
    this->world_y = 0;

    // initialise the grid
    this->grid = new gridCell ** [this->grid_x];
    for (int i = 0; i < this->grid_x; i++) {
        this->grid[i] = new gridCell *[this->grid_y];
    }
    
    this->worldMap = LoadTextureFromImage(blankMap); 
    UnloadImage(blankMap);
}

infiniteWorld::~infiniteWorld(){
    UnloadTexture(this->worldMap);
    // free the grid
    for (int i = 0; i < grid_x; ++i) {
        delete[] this->grid[i];
    }
    delete[] this->grid;
}

void infiniteWorld::translateWorld(int x, int y){
    this->world_x += x;
    this->world_y += y;

    this->offset_x = floor(world_x/(map_x/grid_x));
    this->offset_y = floor(world_y/(map_y/grid_y));
}

void infiniteWorld::render(){
    for(int y = 0; y < grid_y; y++){
        for(int x = 0; x < grid_x; x++){
            printf("|%d,%d|",x + offset_x,y+offset_y);
        }
        printf("\n");
    }
    printf("|%d,%d|",world_x,world_y);
    printf("\n");
}

#endif