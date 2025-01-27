#ifndef WORLD_C
#define WORLD_C
#include "TED.hpp"

#define MAX_SPEED 2
#define PH_COUNT 8
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
    Vector3 getPerpindicularBisector(Vector2 p1, Vector2 p2);
    Vector2 getIntersector(Vector3 p1, Vector3 p2);

    // additional functions
    float distance(Vector2 p1, Vector2 p2);

    // Getting the grid coords
    Vector2 getGridIndex2D(int x, int y);

    // moves all plates
    void moveStepPlates();
    void moveAllPlates(Vector2 pos);
    void updatePlatePositions();
    
    // access wrapped neighbour
    void accessWrappedEdge(void (fixedWorld::* func)(int x, int y, int offset_x, int offset_y));

    //render
    void render();

    //demo functions
    void purge_grids_demo(int e_x, int e_y);
};

fixedWorld::fixedWorld(int map_x, int map_y, int grid_x, int grid_y, int seed){
    Image blankMap = GenImageColor(map_x, map_y, BLACK);
    

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
    
    this->worldMap = LoadTextureFromImage(blankMap); 
    UnloadImage(blankMap);
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
    

    printf("Grids: [%d][%d]\n",grid_x, grid_y);
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
                    GenImageColor(map_x, map_y, BLACK),
                    Vector2Normalize((Vector2){rand()%100, rand()%100}),
                    3 + (rand()%MAX_SPEED)
                );
        }
    }

    printf("Starting pixel assign\n");

    // loops through every pixel
    for(int x = 0; x < this->map_x; x++){
        for(int y = 0; y < this->map_y; y++){
            // if(y != 0){
            //     printf("FAILED");
            //     x = this->map_x;
                
            //     break;;
            // }
            // gets the index of the grid cell in which the pixel is in
            Vector2 curGrid = getGridIndex2D(x,y);

            
            //gets the distance between the pixel and the plate point
            int CI_x = curGrid.x, CI_y = curGrid.y;
            grid_temp = this->grid[CI_x][CI_y];


            if(grid_temp == NULL){
                printf("FAILED");
            }
            
            
            if (!grid_temp->getPlates().empty()) {
                p = grid_temp->getPlates().front();
            }
            else{
                printf("FAILED");
            }


            // float closestDistance = distance((Vector2){x,y},p->getPlateCenter());
            float closestDistance = distance((Vector2){x,y},p->getPos());

            // printf("DIST %f\n",closestDistance);

            // checks the neighbouring grid cells
            for(int lx = -1; lx <= 1; lx++){
                for(int ly = -1; ly <= 1; ly++){
                    // skips its own cell
                    if(lx == 0 && ly == 0){
                        continue;
                    }

                    // getting offset grid
                    int off_x = (grid_size_x  + (int)curGrid.x + lx) % grid_x;
                    int off_y = (grid_size_y  + (int)curGrid.y + ly) % grid_y;
                    
                    
                    // getting the current grid and plate
                    grid_temp = this->grid[off_x][off_y];
                    // grid_temp = this->grid[CI_x][CI_y];

                    if (!grid_temp->getPlates().empty()) {
                        p = grid_temp->getPlates().front();
                    }
                    else{
                        printf("FAILED");
                    }

                    // gets closest xy point for wrapped world

                    int x2, y2;
                    x2 = p->getPos().x;
                    y2 = p->getPos().y;

                    float dx = abs(x - x2);
                    if (dx > map_x/2){
                        dx = map_x - dx;
                    }

                    float dy = abs(y - y2);
                    if (dy > map_y/2){
                        dy = map_y - dy;
                    }

                    // calculates the distance between the pixel and the plate center
                    float dist = sqrtf(dx*dx + dy*dy);



                    // printf("\n\nPIX: %d:%d\n",x,y);
                    // printf("Off Grids: [%d:%d][%d:%d]\n",curGrid.x,curGrid.y,off_x, off_y);
                    // printf("DIST %f - %f\n",closestDistance, dist);


                    // checks if the distance is closer than the current closest distance
                    if(dist < closestDistance){

                        // sets the new closet distance
                        closestDistance = dist;
                        CI_x = off_x;
                        CI_y = off_y;
                    }
                    
                }
            }
            
            // sets the pixel to be in the closet plate and changes the pixel color to match the plates
            p = this->grid[CI_x][CI_y]->getPlates().front();

            // printf("%d,%d\n",CI_x,CI_y);

            // Color c = p->color;
            // ImageDrawPixel(&p->localMap, x, y, c);

            p->setPixel(x,y);

        }
    }
    printf("Finished pixel assign\n");

    initPlateHull();
    return;
}

void fixedWorld::initPlateHull(){
    // creating winding order
    int lx[9] = {-1, 0, 1, 1, 1, 0,-1,-1,-1};
    int ly[9] = {-1,-1,-1, 0, 1, 1, 1, 0,-1};

    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){
            plate *p = grid[x][y]->getPlates().front();
            Vector3 pb_array[9];

            for(int l = 0; l < PH_COUNT; l++){

                // gets offsets for wrapped points
                int offset_x = (x + lx[l] == -1) ? -map_x : x + lx[l] == grid_x ? map_x : 0;
                int offset_y = (y + ly[l] == -1) ? -map_y : y + ly[l] == grid_y ? map_y : 0;

                plate *p2 = grid[(grid_x + x + lx[l])%grid_x][(grid_y + y + ly[l])%grid_y]->getPlates().front();

                pb_array[l] = getPerpindicularBisector(p->getPos(), (Vector2){p2->getPos().x + offset_x, p2->getPos().y + offset_y});


                float mx = (p->getPos().x + p2->getPos().x) / 2.0;
                float my = (p->getPos().y + p2->getPos().y) / 2.0;
                p->mpoints.push_back((Vector2){mx  - p->getPos().x, my - p->getPos().y});
            }

            for(int i = 0; i < PH_COUNT; i++){
                Vector2 i1 = getIntersector(pb_array[i%PH_COUNT],pb_array[(i+1)%PH_COUNT]);
                Vector2 i2 = getIntersector(pb_array[(PH_COUNT + i - 1)%PH_COUNT],pb_array[(i+1)%PH_COUNT]);
                
                
                

                // checks for valid Vectors
                Vector2 res = {0, 0}; ;
                if(std::isnan(static_cast<double>(i1.x))
                && std::isnan(static_cast<double>(i2.x))){
                    printf("FAILED BOTH\n");
                }
                printf("%f:%f\n",i1.x,i2.x);
                if(std::isnan(static_cast<double>(i1.x))){
                    printf("SET 2\n");
                    res = i2;
                }
                else if(std::isnan(static_cast<double>(i2.x))){
                    printf("SET 1\n");
                    res = i1;
                }
                else{
                    printf("SET 0\n");
                    // finds closest Vector
                    int d1 = distance(p->getPos(), i1); //(Vector2){0,0}
                    int d2 = distance(p->getPos(), i2); //(Vector2){0,0}
                    
                    res = d1 < d2 ? i1 : i2;
                }
                printf("OUT %f:%f\n",res.x, res.y);
                res = {res.x - p->getPos().x, res.y - p->getPos().y};

                p->getHull().push_back(res);
            }


        }
    }
}

Vector3 fixedWorld::getPerpindicularBisector(Vector2 p1, Vector2 p2){
    // getting midpoint
    float mx = (p1.x + p2.x) / 2.0;
    float my = (p1.y + p2.y) / 2.0;

    // slopes
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    
    if (dy == 0) { // Vertical line segment
        return (Vector3){1,0,mx};
    }
    if (dx == 0) { // Horizontal line segment
        return (Vector3){0,1,my};
    }

    float slope = -dx/dy;
    
    return (Vector3){-slope,1,my - slope * mx};
}

Vector2 fixedWorld::getIntersector(Vector3 p1, Vector3 p2){

    
    double det = p1.x * p2.y - p2.x * p1.y;

    if (det == 0) {
        // Lines are parallel
        printf("FAILED INTERSECT\n");
        return (Vector2){NAN, NAN};
    }
    return (Vector2){
        (p2.y * p1.z - p1.y * p2.z) / det,
        (p1.x * p2.z - p2.x * p1.z) / det
    };
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

void fixedWorld::moveStepPlates(){
    
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){

            gridCell * pt = grid[x][y];

            for(plate * p : pt->getPlates()){
                Vector2 v = p->getPos();
                p->movePlateWrapped(map_x, map_y);
            }

        }
    }
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

void fixedWorld::purge_grids_demo(int e_x, int e_y){
    
    for(int x = 0; x < grid_x; x++){
        for(int y = 0; y < grid_y; y++){
            if(x == e_x && y == e_y){
                continue;
            }
            gridCell * pt = grid[x][y];
            printf("D %d\n",pt->getPlates().size());
            
            for(int p = 0; p < pt->getPlates().size(); p++){
                printf("deleting\n");
                pt->deletePlate(p);
                printf("Size %d\n", pt->getPlates().size());
            }
        }        
    }
}

#endif