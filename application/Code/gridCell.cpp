#ifndef GRIDCELL_C
#define GRIDCELL_C
#include "TED.hpp"

class gridCell{
private:
    std::list<plate *> plates;
    Vector2 pos;
    int size_x;
    int size_y;
public:
    gridCell();
    
    gridCell(int size_x, int size_y, Vector2 pos);
    ~gridCell();
    
    Vector2 getSize();
    Vector2 getPos();

    void addPlate(plate * p);
    void addNewPlate(Vector2 platePos, Image m, Vector2 direction, float speed);
    const std::list<plate *> getPlates();
    void deletePlate(int i);
    void popPlate(plate * p);
};

gridCell::gridCell(){

}



// initialises the grid cell
gridCell::gridCell(int size_x, int size_y, Vector2 pos){
    // printf("init gridcell\n");
    this->size_x = size_x;
    this->size_y = size_y;
    // printf("Passed %f:%f\n",pos.x, pos.y);
    this->pos.x = pos.x;
    this->pos.y = pos.y;
}

// Get size og gridcell
Vector2 gridCell::getSize(){
    return (Vector2){this->size_x, this->size_y};
}

// get grid top left corner position
Vector2 gridCell::getPos(){
    return this->pos;
}

// adds plate to grid cell list
void gridCell::addPlate(plate * p){
    plates.push_back(p);
    // p->setGlobal(getPlateGlobalPos(p));
    
}

// initialises and adds plate to grid cell
void gridCell::addNewPlate(Vector2 platePos, Image m, Vector2 direction, float speed){
    printf("NEW PLATE: M %f:%f, speed %f\n",direction.x,direction.y,speed);
    this->plates.push_back( 
    new plate(
        m,
        (Vector2){platePos.x + pos.x, platePos.y + pos.y},
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



#endif