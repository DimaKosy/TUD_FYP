#ifndef GRIDCELL_C
#define GRIDCELL_C
#include "TED.hpp"

class gridCell{
private:
    std::list<plate *> plates;
    
    int size_x;
    int size_y;
public:
    gridCell();
    
    gridCell(int size_x, int size_y, Vector2 pos);
    ~gridCell();
    Vector2 pos;
    Vector2 getSize();

    void addPlate(plate * p);
    void addNewPlate(Vector2, Image);
    const std::list<plate *> getPlates();
    Vector2 getPlateGlobalPos(plate * p);
    Vector2 getPlateGlobalPos(plate * p, Vector2 offset);
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
    printf("Passed %f:%f\n",pos.x, pos.y);
    this->pos.x = pos.x;
    this->pos.y = pos.y;
}

// Get size og gridcell
Vector2 gridCell::getSize(){
    return (Vector2){this->size_x, this->size_y};
}


// adds plate to grid cell list
void gridCell::addPlate(plate * p){
    plates.push_back(p);
}

// initialises and adds plate to grid cell
void gridCell::addNewPlate(Vector2 pos, Image m){
    this->plates.push_back( 
    new plate(
        pos,
        m
    ));
}

// gets list of plates
const std::list<plate *> gridCell::getPlates(){
    return this->plates;
}

Vector2 gridCell::getPlateGlobalPos(plate * p){
    return (Vector2){
        pos.x + p->getPlateCenter().x,
        pos.y + p->getPlateCenter().y
        };
}

Vector2 gridCell::getPlateGlobalPos(plate * p, Vector2 offset){
    return (Vector2){
        pos.x + p->getPlateCenter().x + offset.x,
        pos.y + p->getPlateCenter().y + offset.y
        };
}

void gridCell::deletePlate(int i){
    delete plates.front();
    plates.pop_front();
}

void gridCell::popPlate(plate * p){
    plates.remove(p);
}

gridCell::~gridCell(){
}



#endif