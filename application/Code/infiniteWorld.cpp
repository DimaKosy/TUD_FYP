#ifndef WORLD_INF_C
#define WORLD_INF_C
#include "TED.hpp"

#define MAX_SPEED 2

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
    int grid_size_x;
    int grid_size_y;

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

    this->grid_size_x = map_x/grid_x;
    this->grid_size_y = map_y/grid_y;

    // initialise the grid
    this->grid = new gridCell ** [this->grid_x];
    for (int i = 0; i < this->grid_x; i++) {
        this->grid[i] = new gridCell *[this->grid_y];
    }

    for(int x = 0; x < this->grid_x; x++){
        for(int y = 0; y < this->grid_y; y++){

            
            // initialises and assigns a new grid cell
            this->grid[x][y] = new gridCell(grid_size_x, grid_size_y,(Vector2){ (x-1) * grid_size_x, (y-1) * grid_size_y});;

                        this->grid[x][y]->addNewPlate(
                    (Vector2){
                        (rand()%(grid_size_x)),
                        (rand()%(grid_size_y))
                    },
                    GenImageColor(map_x, map_y, BLACK),
                    Vector2Normalize((Vector2){rand()%100, rand()%100}),
                    3 + (rand()%MAX_SPEED)
                );
        }
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

    // index for comparing displacement
    int ox = floor(world_x/(map_x/grid_x));
    int oy = floor(world_y/(map_y/grid_y));
    
    // looped index for accessing the grid array
    int cx = (grid_x +(ox)%grid_x)%grid_x;
    int cy = (grid_y +(oy)%grid_y)%grid_y;

    printf("O: %d,%d\n",this->offset_x, this->offset_y);            

    switch (ox - this->offset_x){
        case -1://leftmost
            cx = (grid_x + (-1 + this->offset_x)%grid_x)%grid_x; //gets the index of the grid that needs to be modified for loop
            printf("Accessing left %d:%d\n",cx,cy);

            for(int iy = 0; iy < grid_y; iy++){
                // grid[cx][iy];
                printf("%3d:%3d\n",cx,iy);
            }

        break;
        case 1: //rightmost
            cx = (grid_x + (grid_x + this->offset_x)%grid_x)%grid_x; //gets the index of the grid that needs to be modified for loop
            printf("Accessing right %d:%d\n",cx,cy);
            
            for(int iy = 0; iy < grid_y; iy++){
                // grid[cx][iy];
                printf("%3d:%3d\n",cx,iy);
            }
        break;
        default:
        break;
    }

    switch (oy - this->offset_y){
       case -1://topmost
            cy = (grid_y + (-1 + this->offset_y)%grid_y)%grid_y; //gets the index of the grid that needs to be modified for loop
            printf("Accessing top %d:%d\n",cx,cy);

            for(int ix = 0; ix < grid_y; ix++){
                // grid[ix][cy];
                printf("%d:%d\t",ix,cy);
                printf("P= %f:%f\n",this->offset_x, this->offset_y);
            }
        break;
        case 1: //Bottommost
            cy = (grid_y + (grid_y + this->offset_y)%grid_y)%grid_y; //gets the index of the grid that needs to be modified for loop
            printf("Accessing bottom %d:%d\n",cx,cy);
            for(int ix = 0; ix < grid_y; ix++){
                // grid[ix][cy];
                printf("%d:%d\n",ix,cy);
            }
        break;
        default:
        break;
    }

    this->offset_x = ox;
    this->offset_y = oy;
}

void infiniteWorld::render(){
    for(int y = 0; y < grid_y; y++){
        for(int x = 0; x < grid_x; x++){
            int cx = (grid_x +(x +this->offset_x)%grid_x)%grid_x;
            int cy = (grid_y +(y +this->offset_y)%grid_y)%grid_y;

            Vector2 v = this->grid[cx][cy]->getPos();
            printf("%f:%f",v.x,v.y);
            printf("|");
        }
        printf("\n");
    }
    printf("|%d,%d|",world_x,world_y);
    printf("\n");
}

#endif