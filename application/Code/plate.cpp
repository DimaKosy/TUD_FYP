#ifndef PLATE_C
#define PLATE_C
#include "TED.hpp"

class plate{
private:
    Vector2 globalPos;
    Vector2 direction;
    std::list<Vector2> hull;
    
    float speed;
    Texture2D mapTexture;

    
public:
    Image localMap;
    Color color;
    std::list<Vector2> mpoints;

    plate(Image localMap, Vector2 globalPos, Vector2 direction, float speed);
    ~plate();
    Vector2 getPos();
    Vector2 getSize();
    Vector2 getDirection();
    std::list<Vector2>& getHull();

    void movePlate();
    void movePlateWrapped(int wrap_x, int wrap_y);
    void setPos(Vector2 pos);
    void setPixel(int x, int y);
    void render(int pos_x, int pos_y);
};

// initialises the plate with global pos
plate::plate(Image localMap, Vector2 globalPos, Vector2 direction, float speed){
    this->localMap = localMap;
    this->globalPos = globalPos;
    this->color = (Color){
        .r = (unsigned char)(rand()%256),
        .g = (unsigned char)(rand()%256),
        .b = (unsigned char)(rand()%256),
        .a = 255
    };
    this->direction = direction;
    this->speed = speed;
    
}

plate::~plate(){
    printf("closing plate\n");
    UnloadImage(this->localMap);
    UnloadTexture(mapTexture);
}


Vector2 plate::getPos(){
    return (Vector2){
        globalPos.x,
        globalPos.y
    };
}

std::list<Vector2>& plate::getHull(){
    return this->hull;
}

void plate::movePlate(){
    this->globalPos.x += this->direction.x * this->speed;
    this->globalPos.y += this->direction.y * this->speed;
    // printf("M %f:%f\n",this->pos.x,this->pos.y);
}

void plate::movePlateWrapped(int wrap_x, int wrap_y){
    this->globalPos.x += this->direction.x * this->speed;
    this->globalPos.y += this->direction.y * this->speed;


    this->globalPos.x = fmod((wrap_x + this->globalPos.x), wrap_x);
    this->globalPos.y = fmod((wrap_y + this->globalPos.y), wrap_y);

    // printf("M %f:%f\n",this->pos.x,this->pos.y);
}

void plate::setPos(Vector2 pos){
    this->globalPos = pos;
}

void plate::setPixel(int x, int y){
    int pixel_x = (x - (int)this->globalPos.x + (this->localMap.width/2)  + this->localMap.width) % (int)(this->localMap.width);
    int pixel_y = (y - (int)this->globalPos.y + (this->localMap.height/2) + this->localMap.height) % (int)(this->localMap.height);
    ImageDrawPixel(&this->localMap,
    pixel_x, 
    pixel_y,
    this->color);

    

    // ImageDrawPixel(&this->localMap,x,y,this->color);
}

// renders the plate
void plate::render(int pos_x, int pos_y){
    UnloadTexture(mapTexture);
    ImageDrawCircle(&this->localMap, 5,5,10,RED);
    ImageDrawCircle(&this->localMap, this->localMap.width/2,this->localMap.height/2,10,BLUE);


    for(Vector2 v : this->hull){
        // printf("%d,%d\n",v.x,v.y);
        ImageDrawCircle(&this->localMap, v.x + this->localMap.width/2,v.y + this->localMap.height/2,5,GREEN);
    }

    for(Vector2 v : this->mpoints){
        // printf("%d,%d\n",v.x,v.y);
        ImageDrawCircle(&this->localMap, v.x + this->localMap.width/2,v.y + this->localMap.height/2,5,RED);
    }


    mapTexture = LoadTextureFromImage(this->localMap);
    // printf("REN %f:%d,, %f:%d\n",pos.x,pos_x,pos.y,pos_y);
    DrawTexture(mapTexture,
    globalPos.x + pos_x - this->localMap.width/2,
    globalPos.y + pos_y - this->localMap.height/2, 
    WHITE);    
}
#endif