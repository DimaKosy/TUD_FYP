#ifndef WORLD_C
#define WORLD_C
#include "TED.hpp"

#define MAX_SPEED 2
#define DEPTH 3
#define PH_COUNT 8
#define COL_SEARCH_RAD 2

class fixedWorld{
private:
    int map_x;
    int map_y;
    int grid_x;
    int grid_y;

    int world_seed;

    Texture2D worldMap;
    gridCell *** grid;

    void renderCall(int x, int y, int  offset_x, int offset_y);

public:
    fixedWorld(int map_x, int map_y, int grid_x, int grid_y, int seed);
    ~fixedWorld();

    //Generates a world texture
    void init_plates();

    // generates the initial plate hulls
    void initPlateHull();
    
    // additional functions
    float distance(Vector2 p1, Vector2 p2);

    // Getting the grid coords
    Vector2 getGridIndex2D(int x, int y);

    //Get gridcell
    gridCell * getGridCell(int x, int y);

    // moves all plates
    void moveStepPlates();
    void moveAllPlates(Vector2 pos);
    void updatePlatePositions();
    
    // access wrapped neighbour
    void accessWrappedEdge(void (fixedWorld::* func)(int x, int y, int offset_x, int offset_y));

    //render
    void render();

    //demo functions
    void purge_grids_demo(Vector2 exclude[], int length);
    void debugPlateVertexs();
};

fixedWorld::fixedWorld(int map_x, int map_y, int grid_x, int grid_y, int seed){
    

    this->map_x = map_x;
    this->map_y = map_y;

    this->grid_x = grid_x;
    this->grid_y = grid_y;

    this->world_seed = seed;

    // initialise the grid
    this->grid = new gridCell ** [grid_x];
    for (int i = 0; i < grid_x; i++) {
        this->grid[i] = new gridCell *[grid_y];
    }

    //creates a pointer link between grid which spans outside of render area and inner grid
    init_plates();
}

fixedWorld::~fixedWorld(){
    UnloadTexture(this->worldMap);
    // free the grid
    for (int i = 0; i < grid_x; ++i) {
        delete[] this->grid[i];
    }
    delete[] this->grid;
}

// initialises the plates using a voronoi texture based off random points distributed in the grid
void fixedWorld::init_plates(){
    gridCell * grid_temp;
    plate * p;
    

    // printf("Grids: [%d][%d]\n",grid_x, grid_y);
    // setting the grid cell size
    int grid_size_x = map_x/grid_x;
    int grid_size_y = map_y/grid_y;
    
    // loop through every grid 
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            
            // initialises and assigns a new grid cell
            // printf("Should be %d:%d\n",x*grid_size_x,y*grid_size_y);
            this->grid[x][y] = new gridCell(grid_size_x, grid_size_y,(Vector2){ x * grid_size_x, y * grid_size_y}, world_seed);;

            // adds a new plate to the grid cell with its center in a random position in the grid cell
            this->grid[x][y]->addNewPlate(
                    (Vector2){
                        (rand()%(grid_size_x)),
                        (rand()%(grid_size_y))
                    },
                    Vector2Normalize((Vector2){rand()%100, rand()%100}),
                    3 + (rand()%MAX_SPEED)
                );
        }
    }

    initPlateHull();
    
    return;
}

void fixedWorld::initPlateHull(){
    // creating winding order
    int lx[9] = {-1, 0, 1, 1, 1, 0,-1,-1,-1};
    int ly[9] = {-1,-1,-1, 0, 1, 1, 1, 0,-1};

    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){
            // printf("GRID %d:%d\n",x,y);
            plate *p = grid[x][y]->getPlates().front();
            Vector3 pb_array[9];

            for(int l = 0; l < PH_COUNT; l++){

                // gets offsets for wrapped points
                int offset_x = (x + lx[l] == -1) ? -map_x : x + lx[l] == grid_x ? map_x : 0;
                int offset_y = (y + ly[l] == -1) ? -map_y : y + ly[l] == grid_y ? map_y : 0;

                plate *p2 = grid[(grid_x + x + lx[l])%grid_x][(grid_y + y + ly[l])%grid_y]->getPlates().front();

                pb_array[l] = getPerpindicularBisector(p->getPos(), (Vector2){p2->getPos().x + offset_x, p2->getPos().y + offset_y});


                float mx = (p->getPos().x + p2->getPos().x + offset_x) / 2.0;
                float my = (p->getPos().y + p2->getPos().y + offset_y) / 2.0;
            }

            for(int i = 0; i < PH_COUNT; i++){
                Vector2 i1 = getIntersector(pb_array[i%PH_COUNT],pb_array[(i+1)%PH_COUNT]); //Current and next
                Vector2 i2 = getIntersector(pb_array[(PH_COUNT + i - 1)%PH_COUNT],pb_array[(i+1)%PH_COUNT]); // Previous and Next
                Vector2 i3 = getIntersector(pb_array[i],pb_array[(i+2)%PH_COUNT]); // Current and Two ahead
                
                
                

                // checks for valid Vectors
                Vector2 res = {0, 0};
                if(std::isnan(static_cast<double>(i1.x))
                && std::isnan(static_cast<double>(i2.x))
                && std::isnan(static_cast<double>(i3.x))
                ){
                    printf("FAILED MULTI\n");
                }

                // printf("SET 0\n");
                // finds closest Vector
                float d1 = std::isnan(static_cast<double>(i1.x))?__FLT_MAX__:distance(p->getPos(), i1);
                float d2 = std::isnan(static_cast<double>(i2.x))?__FLT_MAX__:distance(p->getPos(), i2);
                float d3 = std::isnan(static_cast<double>(i3.x))?__FLT_MAX__:distance(p->getPos(), i3);
                
                if(d1 <= d2 && d1 <= d3){
                    res = i1;
                    // printf("PRIMARY %f:%f:%f\n",d1,d2,d3);
                }
                if(d2 <= d1 && d2 <= d3){
                    res = i2;
                    // printf("SECONDARY %f:%f:%f\n",d1,d2,d3);
                }
                if(d3 <= d2 && d3 <= d1){
                    res = i3;
                    // printf("TERTIARY %f:%f:%f\n",d1,d2,d3);
                }
                // res = d1 < d2 ? i1 : i2;
            
                // printf("OUT %f:%f\n",res.x, res.y);
                res = {res.x - p->getPos().x, res.y - p->getPos().y};
                
                bool temp_skipper = false;
                for(Vector2 v : p->getHull()){
                    if(res.x == v.x && res.y == v.y){
                        temp_skipper = true;
                        break;
                    }
                }
                if(!temp_skipper){
                    p->getHull().push_back(res);
                }
                
            }
        }
    }

    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){
            gridCell * pts = grid[x][y];
            for(plate * pt : pts->getPlates()){
                // printf("REGENBOUND %d, %d\n",x,y);
                pt->regenBoundingBox();
                pt->regenBoundingBox();
                pt->initHeightMesh(DEPTH);
            }
        }
    }
}

// Gets the grid by map coords
Vector2 fixedWorld::getGridIndex2D(int mx, int my){
    int gx = (mx/(map_x/grid_x));
    int gy = (my/(map_y/grid_y));

    if (gx < 0 || gx >= grid_x || gy < 0 || gy >= grid_y) {
        printf("OUT OF BOUNDS %d:%d with %d:%d @ %d:%d\n",mx,my,gx,gy,(map_x/grid_x),(map_y/grid_y));
    }

    gx = std::max(0, std::min(gx, grid_x - 1));
    gy = std::max(0, std::min(gy, grid_y - 1));
    
    return (Vector2){gx,gy};
}

gridCell * fixedWorld::getGridCell(int x, int y){
    return grid[x][y];
}

void fixedWorld::moveStepPlates(){
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            gridCell * pt = grid[x][y];

            for(plate * p : pt->getPlates()){

                // for(auto v:p->getHull()){
                //     printf("(%f,%f),",v.x + p->getPos().x,v.y + p->getPos().y);
                // }
    
                // printf("(%f,%f)\n", p->getHull().front().x  + p->getPos().x, p->getHull().front().y  + p->getPos().y);



                Vector2 v = p->getPos();
                p->movePlateWrapped(map_x, map_y);
                p->DeformBackfill();
                // p->AngleFilter();
                p->DebugRect = GREEN;
                Vector2 temp = p->regenBoundingBox();
                p->getMesh()->stretchDeform({
                    temp.x,
                    temp.y
                });
            }
        }
    }

    // debugPlateVertexs();
    
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            gridCell * pt = grid[x][y];

            for(plate * p : pt->getPlates()){
                Vector2 v = p->getPos();
                // p->movePlateWrapped(map_x, map_y);

                for(int x1 = -COL_SEARCH_RAD; x1 <= COL_SEARCH_RAD; x1++){
                    for(int y1 = -COL_SEARCH_RAD; y1 <= COL_SEARCH_RAD; y1++){
                        gridCell * pt1 = grid[(grid_x + x + x1) % grid_x][(grid_y + y + y1) % grid_y];
                        Vector2 offset = {0,0};

                        offset.x = (x + x1 < 0) ? -map_x : x + x1 >= grid_x ? map_x : 0;
                        offset.y = (y + y1 < 0) ? -map_y : y + y1 >= grid_y ? map_y : 0;
                        
                        for(plate * p1 : pt1->getPlates()){
                            // skips if the id of the plate is higher than the other plate or the same, avoids duplicate checks
                           
                            if(p >= p1){
                                continue;
                            }

                            // printf("OFFSET %f,%f\n",offset.x, offset.y);
                            // p->selfCollisionCheck(p1);
                            if(p->selfAABBCollisionCheck(p1, offset)){
                                // printf("\n\n");
                                
                                p->DebugRect = RED;
                                p1->DebugRect = RED;
                                p->selfCollisionDeformation(p1, offset);
                                // printf("Colliding\n");                                

                                
                            }

                            
                        }
                    }
                }
            }
        }
    }

    // for(int x = 0; x < grid_x; x++){
    //     for(int y = 0; y < grid_y; y++){

    //         gridCell * pt = grid[x][y];

    //         for(plate * p : pt->getPlates()){
    //             Vector2 v = p->getPos();
    //             // p->movePlateWrapped(map_x, map_y);

    //             for(int x1 = -COL_SEARCH_RAD; x1 <= COL_SEARCH_RAD; x1++){
    //                 for(int y1 = -COL_SEARCH_RAD; y1 <= COL_SEARCH_RAD; y1++){
    //                     gridCell * pt1 = grid[(grid_x + x + x1) % grid_x][(grid_y + y + y1) % grid_y];


    //                     for(plate * p1 : pt1->getPlates()){
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
}

void fixedWorld::moveAllPlates(Vector2 pos){
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){


            gridCell * pt = grid[x][y];
            
            for(plate * p : pt->getPlates()){
                Vector2 v = p->getPos();
                p->setPos((Vector2){
                    fmod((map_x + v.x +pos.x),map_x),
                    fmod((map_y + v.y +pos.y),map_y)
                });

            }

        }
    }
}

void fixedWorld::updatePlatePositions(){
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            gridCell * pt = grid[x][y];
            auto plate_list = pt->getPlates();
            for(plate * p : plate_list){

                Vector2 t = p->getPos();
                
                Vector2 gridCord = getGridIndex2D(t.x,t.y);

                pt->popPlate(p);
                grid[(int)gridCord.x][(int)gridCord.y]->addPlate(p);
                
            }

        }
    }
}

float fixedWorld::distance(Vector2 p1, Vector2 p2) {
    return sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

void fixedWorld::accessWrappedEdge(void (fixedWorld::* func)(int x, int y, int offset_x, int offset_y)){
    // edges with x||y of 0 or x||y of gridsize-1 must link to their wrapped neighbouring edge

    for(int x = -1; x < grid_x + 1; x++){
        for(int y = -1; y < grid_y + 1; y++){
            if(x != 1 || y != 1){
                // printf("Skipping\n");
                // continue;   
            }
            int rx = (grid_x + x)%grid_x;
            int ry = (grid_y + y)%grid_y;

            int offset_x = x == -1 ? -map_x : x == grid_x ? map_x : 0;
            int offset_y = y == -1 ? -map_y : y == grid_y ? map_y : 0;

            (this->*func)(rx, ry, offset_x, offset_y);
        }
    }
}

// goes through every grid cell and every plate thats in that cell and renders it
void fixedWorld::render(){      

    // for(int x = 0; x < this->grid_x; x++){
    //     for(int y = 0; y < this->grid_y; y++){
    //         grid[x][y]->DebugRender();
    //     }        
    // }
    
    accessWrappedEdge(renderCall);

    

}

void fixedWorld::renderCall(int x, int y, int  offset_x, int offset_y){
            auto pt = grid[x][y]->getPlates();
            

            if(pt.empty()){
                return;
            }
            
            BeginBlendMode(BLEND_ADDITIVE);
            for(plate * p : pt){
                // (*pi)->render(x_offset, y_offset);
                p->render(offset_x, offset_y);
                
            }
            EndBlendMode();
}

void fixedWorld::debugPlateVertexs(){
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            gridCell * pt = grid[x][y];

            for(plate * p : pt->getPlates()){
                for(auto v : p->getHull()){
                    printf("(%f,-%f),",v.x + p->getPos().x,v.y +p->getPos().y);
                }

                printf("\b \n");
            }
        }
    }

}

void fixedWorld::purge_grids_demo(Vector2 exclude [], int length){
    bool temp;
    //loop through all the grid cells
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            // bool to check if the grid is in the exclude list
            temp = false;
            
            // loops through exclude list
            for(int i = 0; i < length; i++){
                if(exclude[i].x == x && exclude[i].y == y){
                    temp = true;
                    break;
                }
            }

            // continues to next iteration if its in the list
            if(temp == true){
                continue;
            }

            // get the grid cell
            gridCell * pt = grid[x][y];
            printf("D%d:%d %d\n",x,y,pt->getPlates().size());
            
            // delete the plates in the cell
            for(int p = 0; p < pt->getPlates().size(); p++){
                printf("deleting\n");
                pt->deletePlate(p);
                printf("Size %d\n", pt->getPlates().size());
            }
        }        
    }
}

#endif