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
    gridCell ** outerGrid;
    gridCell * innerGrid;

public:
    world(int map_x, int map_y, int grid_x, int grid_y);
    ~world();

    //Generates a world texture
    void genNewWorld();
    void init_plates();
    void genGrid();

    // additional functions
    float distance(Vector2 p1, Vector2 p2);

    // Getting the grid coords
    Vector2 getGridIndex2D(int x, int y);

    // moves all plates
    void moveAllPlates(Vector2 pos);

    //1D index to 2D conversions
    Vector2 self_2D_index(int i);
    Vector2 to_2D_index(int i, int indexCount_x, int indexCount_y);
    
    //2D index to 1D conversions
    int self_1D_index(Vector2 pair);
    int to_1D_index(Vector2 pair, int indexCount_x, int indexCount_y);
    int self_1D_index(int x, int y);
    int to_1D_index(int x, int y, int indexCount_x, int indexCount_y);
    
    //render
    void render();

    //demo functions
    void purge_grids_demo(int except);
};

world::world(int map_x, int map_y, int grid_x, int grid_y){
    Image blankMap = GenImageColor(map_x, map_y, BLACK);
    

    this->map_x = map_x;
    this->map_y = map_y;

    this->grid_x = grid_x;
    this->grid_y = grid_y;

    this->outerGrid = (gridCell **) calloc ((grid_x+2)*(grid_y+2), sizeof(gridCell *));
    // this->innerGrid = (gridCell *) calloc (grid_x*grid_y, sizeof(gridCell));
    this->innerGrid = new gridCell[grid_x * grid_y];

    // for(int i = 0; i < grid_x*grid_y; i++){
    //     this->innerGrid[i] = (gridCell)calloc(1, sizeof(gridCell));
    // }

    //creates a pointer link between grid which spans outside of render area and inner grid
    genGrid();
    init_plates();
    
    // Print statement to make sure everything linked properly
    for(int y = 0; y < grid_y+2; y++){
        for(int x = 0; x < grid_x+2; x++){
            int i = to_1D_index(x,y,grid_x+2,grid_y+2);
            // printf("%2d",i);
            Vector2 coords = to_2D_index((int)((*(outerGrid+i)) - innerGrid),grid_x,grid_y);
            printf("|%d:%d|",coords.x, coords.y);
        }
        printf("\n");
    }

    
    this->worldMap = LoadTextureFromImage(blankMap); 
    UnloadImage(blankMap);
}

world::~world(){
    UnloadTexture(this->worldMap);
    free(this->outerGrid);
    for(int i = 0; i < grid_x * grid_x; i++){
        delete(&this->innerGrid[i]);
        free(this->innerGrid);
    }
}

void world::genGrid(){
    
    // links the center of the outer grid to the center of the inner grid
    for(int x = 1; x <= grid_x; x++){
        for(int y = 1; y <= grid_y; y++){
            // gets the appropriate index
            int outerGrid_i = to_1D_index(x, y, grid_x+2,grid_y+2);
            int innerGrid_i = to_1D_index(x-1, y-1, grid_x, grid_y);
            
            /*
            printf("%d:%d -> %d:%d\n", 
                to_2D_index(outerGrid_i,grid_x+2,grid_y+2).first,
                to_2D_index(outerGrid_i,grid_x+2,grid_y+2).second,
                to_2D_index(innerGrid_i,grid_x,grid_y).first,
                to_2D_index(innerGrid_i,grid_x,grid_y).second);
            */
            
            // links the index
            *(this->outerGrid + outerGrid_i) = (innerGrid + innerGrid_i);            
        }
    }

    //creates a pointer link with outer ring of outer Grid to permiter ring of inner Grid, X
    for(int x = 1; x <= grid_x; x++){
        // gets the appropriate index
        int outerGrid_i1 = to_1D_index(x, 0, grid_x+2,grid_y+2);
        int outerGrid_i2 = to_1D_index(x, grid_y+1, grid_x+2,grid_y+2);
        int innerGrid_i1 = to_1D_index(x-1, grid_y-1, grid_x, grid_y);
        int innerGrid_i2 = to_1D_index(x-1, 0, grid_x, grid_y);

        /*
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
        */ 

        // links the index
        *(this->outerGrid + outerGrid_i1) = &(innerGrid[innerGrid_i1]);
        *(this->outerGrid + outerGrid_i2) = &(innerGrid[innerGrid_i2]);

    }

    //creates a pointer link with outer ring of outer Grid to permiter ring of inner Grid, Y
    for(int y = 1; y <= grid_y; y++){
        // gets the appropriate index
        int outerGrid_i1 = to_1D_index(0, y, grid_x+2,grid_y+2);
        int outerGrid_i2 = to_1D_index(grid_x+1, y, grid_x+2,grid_y+2);
        int innerGrid_i1 = to_1D_index(grid_x-1, y-1, grid_x, grid_y);
        int innerGrid_i2 = to_1D_index(0, y-1, grid_x, grid_y);

        /*
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
        */

        // links the index
        *(this->outerGrid + outerGrid_i1) = &(this->innerGrid[innerGrid_i1]);
        *(this->outerGrid + outerGrid_i2) = &(this->innerGrid[innerGrid_i2]);
    }

    //corner links
    for(int x = 0; x <= 1; x++){
        for(int y = 0; y <= 1; y++){
            // gets the appropriate index
            int outerGrid_i = to_1D_index(x*(grid_x+1), y*(grid_y+1), grid_x+2,grid_y+2);
            int innerGrid_i = to_1D_index((!x)*(grid_x-1), (!y)*(grid_y-1), grid_x, grid_y);

            // links the index
            *(this->outerGrid + outerGrid_i) = (this->innerGrid + innerGrid_i);
        }
    }
}

// initialises the plates using a voronoi texture based off random points distributed in the grid
void world::init_plates(){
    // setting the grid cell size
    int grid_size_x = map_x/grid_x;
    int grid_size_y = map_y/grid_y;
    
    // loop through every grid 
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            // index conversion for inner grid
            int i = to_1D_index(x,y, grid_x, grid_y);
            
            // initialises and assigns a new grid cell
            printf("Should be %d:%d\n",x*grid_size_x,y*grid_size_y);

            gridCell temp;
            
            temp = gridCell(grid_size_x, grid_size_y,(Vector2){ x * grid_size_x, y * grid_size_y});

            
            printf("Is %f:%f\n",temp.pos.x, temp.pos.y);

            this->innerGrid[i] = temp;

            // adds a new plate to the grid cell with its center in a random position in the grid cell
            this->innerGrid[i].addNewPlate(
                    (Vector2){
                        (rand()%(grid_size_x)),
                        (rand()%(grid_size_y))
                    },
                    GenImageColor(map_x, map_y, BLACK)
                );
        }
    }

    // loops through every pixel
    for(int x = 0; x < this->map_x; x++){
        for(int y = 0; y < this->map_y; y++){

            // gets the index of the grid cell in which the pixel is in
            Vector2 curGrid = getGridIndex2D(x,y);

            
            //gets the distance between the pixel and the plate point
            int closestIndex = to_1D_index(curGrid,grid_x,grid_y);
            gridCell grid = this->innerGrid[closestIndex];
            plate * p = grid.getPlates().front();

            // float closestDistance = distance((Vector2){x,y},p->getPlateCenter());
            float closestDistance = distance((Vector2){x,y},grid.getPlateGlobalPos(p));

            // checks the neighbouring grid cells
            for(int lx = -1; lx <= 1; lx++){
                for(int ly = -1; ly <= 1; ly++){
                    // skips its own cell
                    if(lx == 0 && ly == 0){
                        continue;
                    }

                    // getting offset grid
                    Vector2 offset;
                    offset.x = (curGrid.x==0 && lx == -1)?-map_x:
                    (curGrid.x==grid_x-1 && lx == 1)?map_x:0;
                    offset.y = (curGrid.y==0 && ly == -1)?-map_y:
                    (curGrid.y==grid_y-1 && ly == 1)?map_y:0;
                    
                    // converts to grid cell index
                    int index = to_1D_index(curGrid.x + lx, curGrid.y + ly,grid_x,grid_y);
                    
                    // getting the current grid and plate
                    grid = this->innerGrid[index];
                    p = grid.getPlates().front();

                    // calculates the distance between the pixel and the plate center
                    // printf("%f\n",grid.getPlateGlobalPos(p));
                    float dist = distance((Vector2){x,y},grid.getPlateGlobalPos(p,offset));
                    
                    // checks if the distance is closer than the current closest distance
                    if(dist < closestDistance){

                        // sets the new closet distance
                        closestDistance = dist;
                        closestIndex = index;
                    }
                    
                }
            }
            
            // sets the pixel to be in the closet plate and changes the pixel color to match the plates
            p = this->innerGrid[closestIndex].getPlates().front();
            // Color c = p->color;
            // ImageDrawPixel(&p->localMap, x, y, c);

            p->setPixel(x,y);
        }
    }
    



    return;
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

    // Multiplies the pixels of the radial map and perlin map
    for(int i = 0; i < map_x*map_y; i++){

        pixels1[i].r = (pixels1[i].r * pixels2[i].r) / 255;
        pixels1[i].g = (pixels1[i].g * pixels2[i].g) / 255;
        pixels1[i].b = (pixels1[i].b * pixels2[i].b) / 255;
        pixels1[i].a = (pixels1[i].a * pixels2[i].a) / 255;
    }

    // draws pixels to map
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


// Gets the grid by map coords
Vector2 world::getGridIndex2D(int mx, int my){
    int gx = mx/((map_x+1)/grid_x);
    int gy = my/((map_y+1)/grid_y);
    
    return (Vector2){gx,gy};
}

void world::moveAllPlates(Vector2 pos){
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){
            
            int i = to_1D_index(x,y,grid_x,grid_y);

            gridCell * pt = &innerGrid[i];

            for(plate * p : pt->getPlates()){
                Vector2 v = p->getPos();
                p->setPos((Vector2){
                    (int)(v.x +pos.x)%map_x,
                    (int)(v.y +pos.y)%map_y
                });

            }

        }
    }

    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){
            
            int i = to_1D_index(x,y,grid_x,grid_y);

            gridCell * pt = &innerGrid[i];
            auto platesCopy = pt->getPlates(); 
            for(plate * p : platesCopy){

                Vector2 t = pt->getPlateGlobalPos(p);
                
                // printf("T: %f:%f\n",t.x,t.y);
                Vector2 gridCord = getGridIndex2D(t.x,t.y);


                // printf("Grid %f:%f\n", gridCord.x,gridCord.y);

                pt->popPlate(p);
                // printf("\n");
                // // printf("\n%d:%d\n",sx,sy);
                // printf("Grid:%d ->",i);
                i = to_1D_index(gridCord.x,gridCord.y,grid_x,grid_y);
                // printf("%d\n",i);
                innerGrid[i].addPlate(p);
                
            }
        }
    }
}

float world::distance(Vector2 p1, Vector2 p2) {
    return sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

//1D index to 2D conversions
Vector2 world::self_2D_index(int i){
    return to_2D_index(i, this->map_x, this-> map_y);
}

Vector2 world::to_2D_index(int i, int indexCount_x = 1, int indexCount_y = 1){
    int x = i % indexCount_x;
    int y = i / indexCount_x;
    return (Vector2){x,y};
}

//2D index to 1D conversions
int world::self_1D_index(Vector2 pair){
    return to_1D_index(pair.x, pair.y, this->map_x, this->map_y);
}

int world::to_1D_index(Vector2 pair, int indexCount_x, int indexCount_y){
    return to_1D_index(pair.x, pair.y, indexCount_x, indexCount_y);
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
    // printf("%d: %d\n", x,y);

    return indexCount_x*y + x;
}

// goes through every grid cell and every plate thats in that cell and renders it
void world::render(){
    // DrawTexture(worldMap,0,0,WHITE);        

    for(int x = 0; x < grid_x+2; x++){
        for(int y = 0; y < grid_y+2; y++){
            // printf("Opening %d, %d\n",x,y);
            int i = to_1D_index(x,y,grid_x+2,grid_y+2);
            auto p = (*(outerGrid[i])).getPlates();
            
            // offset for plates on edge
            int x_offset = x==0?-map_x:
            x==grid_x+1?map_x:0;
            int y_offset = y==0?-map_y:
            y==grid_y+1?map_y:0;




            //  0 || Max
            //  -map||+map

            

            if(p.empty()){
                continue;
            }
            
            auto pi = p.begin();
            BeginBlendMode(BLEND_ADDITIVE);
            for(int j = 0; j < p.size(); j++){
                // printf("S:%d:%d %d\n",x,y,p.size());
                

                if((*pi) == nullptr){
                    // printf("NULL\n");
                    std::next(pi);
                    continue;
                }
                // printf("S %d:%d\n",(*pi)->color.a, (*pi)->color.r);
                // printf("O %d:%d\n",x_offset,y_offset);
                (*pi)->render(x_offset, y_offset);
                // (*pi)->render(0,0);
                
                std::next(pi);
            }
            EndBlendMode();

            // draws a circle at plate center
            
            // pi = p.begin();
            // for(int j = 0; j < p.size(); j++){
            //     DrawCircle((*pi)->pos.x,(*pi)->pos.y,5,(Color){255,0,0,255});
            //     ++pi;
            // }
        }
    }
}

void world::purge_grids_demo(int except){
    
    for(int i = 0; i < (grid_x*grid_y)  && i < (grid_x*grid_y); i++ ){
        if(i == except){
            continue;
        }
        gridCell * pt = &innerGrid[i];
        printf("D %d\n",pt->getPlates().size());
        
        for(int p = 0; p < pt->getPlates().size(); p++){
            printf("deleting\n");
            pt->deletePlate(p);
            printf("Size %d\n", pt->getPlates().size());
        }
        
    }
}

#endif