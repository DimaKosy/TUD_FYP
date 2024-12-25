#ifndef PLATE_C
#define PLATE_C
#include "TED.hpp"

class plate{
private:
    Vector2 centerPos;
    Vector2 pos;
    Texture2D mapTexture;
    
public:
    Image localMap;
    Color color;

    plate(Vector2 centerPos, Image localMap);
    ~plate();
    Vector2 getPlateCenter();
    Vector2 getPos();
    Vector2 getSize();
    void movePlate(Vector2 pos);
    void setPos(Vector2 pos);
    void setPixel(int x, int y);
    void render(int pos_x, int pos_y);
};

// initialises the plate
plate::plate(Vector2 centerPos, Image localMap){
    this->centerPos = centerPos;
    this->pos = (Vector2){0,0};
    this->localMap = localMap;
    this->color = (Color){
        .r = (unsigned char)(rand()%256),
        .g = (unsigned char)(rand()%256),
        .b = (unsigned char)(rand()%256),
        .a = 255
    };
    
}

plate::~plate(){
    UnloadImage(this->localMap);
    UnloadTexture(mapTexture);
}

Vector2 plate::getPlateCenter(){
    return (Vector2){
        pos.x + centerPos.x,
        pos.y + centerPos.y
    };
}

Vector2 plate::getPos(){
    return (Vector2){
        pos.x,
        pos.y
    };
}


void plate::movePlate(Vector2 pos){
    this->pos.x += pos.x;
    this->pos.y += pos.y;
    // printf("M %f:%f\n",this->pos.x,this->pos.y);
}

void plate::setPos(Vector2 pos){
    this->pos = pos;
}

void plate::setPixel(int x, int y){
    ImageDrawPixel(&this->localMap,x,y,this->color);
}

// renders the plate
void plate::render(int pos_x, int pos_y){
    UnloadTexture(mapTexture);
    mapTexture = LoadTextureFromImage(this->localMap);
    // printf("REN %f:%d,, %f:%d\n",pos.x,pos_x,pos.y,pos_y);
    DrawTexture(mapTexture,pos.x+pos_x,pos.y+pos_y, WHITE);

    
}
#endif