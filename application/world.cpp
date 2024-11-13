#ifndef WORLD_C
#define WORLD_C
#include "TED.hpp"

class world{
private:
    int map_x;
    int map_y;
    int grid_x;
    int grid_y;

    Texture2D worldMap;
    plate *** outerGrid;
    plate ** innerGrid;

public:
    world(int map_x, int map_y, int grid_x, int grid_y);
    ~world();

    //Generates a world texture
    void genNewWorld();
    void genPlates();
    //1D index to 2D conversions
    std::pair<int,int> self_2D_index(int i);
    std::pair<int,int> to_2D_index(int i, int indexCount_x, int indexCount_y);
    
    //2D index to 1D conversions
    int self_1D_index(std::pair<int,int> pair);
    int to_1D_index(std::pair<int,int> pair, int indexCount_x, int indexCount_y);
    int self_1D_index(int x, int y);
    int to_1D_index(int x, int y, int indexCount_x, int indexCount_y);
    
    //render
    void render();
};

world::world(int map_x, int map_y, int grid_x, int grid_y){
    

    this->map_x = map_x;
    this->map_y = map_y;

    this->grid_x = grid_x;
    this->grid_y = grid_y;

    this->outerGrid = (plate ***) calloc ((grid_x+2)*(grid_y+2), sizeof(plate **));
    this->innerGrid = (plate **) calloc (grid_x*grid_y, sizeof(plate *));


    //creates a pointer link between grid which spans outside of render area and inner grid
    printf("center\n");
    
    for(int x = 1; x <= grid_x; x++){
        for(int y = 1; y <= grid_y; y++){
            int outerGrid_i = to_1D_index(x, y, grid_x+2,grid_y+2);
            int innerGrid_i = to_1D_index(x-1, y-1, grid_x, grid_y);
            
            printf("%d:%d -> %d:%d\n", 
                to_2D_index(outerGrid_i,grid_x+2,grid_y+2).first,
                to_2D_index(outerGrid_i,grid_x+2,grid_y+2).second,
                to_2D_index(innerGrid_i,grid_x,grid_y).first,
                to_2D_index(innerGrid_i,grid_x,grid_y).second);

            *(this->outerGrid + outerGrid_i) = (innerGrid + innerGrid_i);            
        }
    }

    //creates a pointer link with outer ring of outer Grid to permiter ring of inner Grid
    printf("X sides\n");
    for(int x = 1; x <= grid_x; x++){
        int outerGrid_i1 = to_1D_index(x, 0, grid_x+2,grid_y+2);
        int outerGrid_i2 = to_1D_index(x, grid_y+1, grid_x+2,grid_y+2);
        int innerGrid_i1 = to_1D_index(x-1, grid_y-1, grid_x, grid_y);
        int innerGrid_i2 = to_1D_index(x-1, 0, grid_x, grid_y);

        printf("%d:%d -> %d:%d\n", 
            to_2D_index(outerGrid_i1,grid_x+2,grid_y+2).first,
            to_2D_index(outerGrid_i1,grid_x+2,grid_y+2).second,
            to_2D_index(innerGrid_i1,grid_x,grid_y).first,
            to_2D_index(innerGrid_i1,grid_x,grid_y).second);
        printf("%d:%d -> %d:%d\n", 
            to_2D_index(outerGrid_i2,grid_x+2,grid_y+2).first,
            to_2D_index(outerGrid_i2,grid_x+2,grid_y+2).second,
            to_2D_index(innerGrid_i2,grid_x,grid_y).first,
            to_2D_index(innerGrid_i2,grid_x,grid_y).second);

        *(this->outerGrid + outerGrid_i1) = &(innerGrid[innerGrid_i1]);
        *(this->outerGrid + outerGrid_i2) = &(innerGrid[innerGrid_i2]);

    }

    printf("Y sides\n");
    for(int y = 1; y <= grid_y; y++){
        int outerGrid_i1 = to_1D_index(0, y, grid_x+2,grid_y+2);
        int outerGrid_i2 = to_1D_index(grid_x+1, y, grid_x+2,grid_y+2);
        int innerGrid_i1 = to_1D_index(grid_x-1, y-1, grid_x, grid_y);
        int innerGrid_i2 = to_1D_index(0, y-1, grid_x, grid_y);

        printf("%d:%d -> %d:%d\n", 
            to_2D_index(outerGrid_i1,grid_x+2,grid_y+2).first,
            to_2D_index(outerGrid_i1,grid_x+2,grid_y+2).second,
            to_2D_index(innerGrid_i1,grid_x,grid_y).first,
            to_2D_index(innerGrid_i1,grid_x,grid_y).second);
        printf("%d:%d -> %d:%d\n", 
            to_2D_index(outerGrid_i2,grid_x+2,grid_y+2).first,
            to_2D_index(outerGrid_i2,grid_x+2,grid_y+2).second,
            to_2D_index(innerGrid_i2,grid_x,grid_y).first,
            to_2D_index(innerGrid_i2,grid_x,grid_y).second);

        *(this->outerGrid + outerGrid_i1) = &(this->innerGrid[innerGrid_i1]);
        *(this->outerGrid + outerGrid_i2) = &(this->innerGrid[innerGrid_i2]);
    }

    //corner links
    for(int x = 0; x <= 1; x++){
        for(int y = 0; y <= 1; y++){
            int outerGrid_i = to_1D_index(x*(grid_x+1), y*(grid_y+1), grid_x+2,grid_y+2);
            int innerGrid_i = to_1D_index((!x)*(grid_x-1), (!y)*(grid_y-1), grid_x, grid_y);

            *(this->outerGrid + outerGrid_i) = (this->innerGrid + innerGrid_i);
        }
    }
    
    // Print statement to make sure everything linked properly
    for(int y = 0; y < grid_y+2; y++){
        for(int x = 0; x < grid_x+2; x++){
            int i = to_1D_index(x,y,grid_x+2,grid_y+2);
            printf("%2d",i);
            std::pair<int,int> coords = to_2D_index((int)((*(outerGrid+i)) - innerGrid),grid_x,grid_y);
            printf("|%d:%d|",coords.first, coords.second);
        }
        printf("\n");
    }

    // for(int i = 0; i < (grid_x+2) * (grid_y+2); i++){
    //     std::pair<int,int> coords = to_2D_index((int)(*(outerGrid+i) - innerGrid),grid_x,grid_y);
        
    //     printf("|%d:%d|",coords.first, coords.second);

    //     if(i%(grid_x+2) == grid_x+1){
    //         printf("\n");
    //     }
    // }



    Image blankMap = GenImageColor(map_x, map_y, BLACK);
    this->worldMap = LoadTextureFromImage(blankMap); 
}

world::~world(){
    UnloadTexture(this->worldMap);
}

void world::genPlates(){

}

void world::genNewWorld(){
    int diagnonalLength = (int)sqrt(pow(map_x,2)+pow(map_y,2));
    int gap_x = (diagnonalLength-map_x)/2;
    int gap_y = (diagnonalLength-map_y)/2;


    Image blankMap = GenImageColor(map_x, map_y, WHITE);
    Image radialMap = GenImageGradientRadial(diagnonalLength, diagnonalLength, 0,(Color){255,255,255,255},(Color){0,0,0,255});
    Image perlinMap = GenImagePerlinNoise(map_x,map_y,0,0,2);
    

    //crop radial map to fit
    ImageCrop(&radialMap,(Rectangle){gap_x,gap_y,map_x, map_y});


    Color* pixels1 = LoadImageColors(radialMap);
    Color* pixels2 = LoadImageColors(perlinMap);


    for(int i = 0; i < map_x*map_y; i++){

        pixels1[i].r = (pixels1[i].r * pixels2[i].r) / 255;
        pixels1[i].g = (pixels1[i].g * pixels2[i].g) / 255;
        pixels1[i].b = (pixels1[i].b * pixels2[i].b) / 255;
        pixels1[i].a = (pixels1[i].a * pixels2[i].a) / 255;
    }

    for(int i = 0; i < map_x * map_y; i++) {
        ImageDrawPixel(&blankMap, i % map_x, i / map_x, pixels1[i]); // Draw each pixel
    }


    this->worldMap = LoadTextureFromImage(blankMap);   

    UnloadImage(blankMap);
    UnloadImage(radialMap);
    UnloadImage(perlinMap);

    UnloadImageColors(pixels1);
    UnloadImageColors(pixels2);
}

//1D index to 2D conversions
std::pair<int,int> world::self_2D_index(int i){
    return to_2D_index(i, this->map_x, this-> map_y);
}

std::pair<int,int> world::to_2D_index(int i, int indexCount_x = 1, int indexCount_y = 1){
    int x = i % indexCount_x;
    int y = i / indexCount_x;
    return std::pair(x,y);
}

//2D index to 1D conversions
int world::self_1D_index(std::pair<int,int> pair){
    return to_1D_index(pair.first, pair.second, this->map_x, this->map_y);
}

int world::to_1D_index(std::pair<int,int> pair, int indexCount_x, int indexCount_y){
    return to_1D_index(pair.first, pair.second, indexCount_x, indexCount_y);
}

int world::self_1D_index(int x, int y){
    return to_1D_index(x, y, this->map_x, this->map_y);
}

int world::to_1D_index(int x, int y, int indexCount_x, int indexCount_y){
    //modulo with - wrap
    x = x % indexCount_x;
    x = (indexCount_x + x) % indexCount_x;
    y = y % indexCount_y;
    y = (indexCount_y + y) % indexCount_y;


    return indexCount_x*y + x;
}

void world::render(){
    DrawTexture(worldMap,0,0,WHITE);
    
    
}

#endif