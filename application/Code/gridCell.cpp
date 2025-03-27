#ifndef GRIDCELL_C
#define GRIDCELL_C
#include "TED.hpp"

#define GRID_B 0.185

class gridCell{
private:
    std::list<plate *> plates;
    Vector2 pos;
    int size_x;
    int size_y;
    int grid_seed;
public:
    gridCell();
    
    gridCell(int size_x, int size_y, Vector2 pos,int seed);
    ~gridCell();
    
    Vector2 getSize();
    Vector2 getPos();

    void regenGridCell(Vector2 pos);

    void addPlate(plate * p);
    void addNewPlate(Vector2 platePos, Vector2 direction, float speed);
    const std::list<plate *> getPlates();
    void deletePlate(int i);
    void popPlate(plate * p);

    // debug
    void DebugRender();
};

// gridCell::gridCell(){

// }



// initialises the grid cell
gridCell::gridCell(int size_x, int size_y, Vector2 pos,int seed){
    // printf("init gridcell\n");
    this->size_x = size_x;
    this->size_y = size_y;
    // printf("Passed %f:%f\n",pos.x, pos.y);
    this->pos.x = pos.x;
    this->pos.y = pos.y;
    this->grid_seed = seed;
}

// Get size og gridcell
Vector2 gridCell::getSize(){
    return (Vector2){this->size_x, this->size_y};
}

// get grid top left corner position
Vector2 gridCell::getPos(){
    return this->pos;
}

// regens the plate, changing position and generating anew plate
void gridCell::regenGridCell(Vector2 pos){
    this->pos.x = pos.x;
    this->pos.y = pos.y;

    for(int i = 0; i < plates.size(); i++){
        delete plates.front();
    }

    // addNewPlate()
}

// adds plate to grid cell list
void gridCell::addPlate(plate * p){
    plates.push_back(p);
    // p->setGlobal(getPlateGlobalPos(p));
    
}

// initialises and adds plate to grid cell
void gridCell::addNewPlate(Vector2 platePos, Vector2 direction, float speed){
    printf("NEW PLATE: M %f:%f, speed %f\n",direction.x,direction.y,speed);
    this->plates.push_back( 
    new plate(
        (Vector2){(platePos.x * (1 - (GRID_B * 2))) + pos.x + (size_x * GRID_B), (platePos.y * (1 - (GRID_B * 2))) + pos.y + (size_y * GRID_B)},
        direction,
        speed
    ));
}

// gets list of plates
const std::list<plate *> gridCell::getPlates(){
    return this->plates;
}

void gridCell::deletePlate(int i){
    delete plates.front();
    plates.pop_front();
}

void gridCell::popPlate(plate * p){
    plates.remove(p);
}

gridCell::~gridCell(){
    printf("Closing grid\n");
}

void gridCell::DebugRender(){
    Color Col = plates.size() > 0 ? GREEN : RED;
    DrawRectangleLines((int)pos.x,(int)pos.y,size_x, size_y, Col);
}

#endif