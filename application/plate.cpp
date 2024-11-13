#ifndef PLATE_C
#define PLATE_C
#include "TED.hpp"

class plate{
private:
    
    
public:
    int x;
    int y;

    plate(std::pair<int,int> coords);
    ~plate();
    void render();
};

plate::plate(std::pair<int,int> coords){
    this->x = coords.first;
    this->y = coords.second;
}

plate::~plate(){
}

void plate::render(){
}
#endif