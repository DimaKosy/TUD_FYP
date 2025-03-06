#ifndef PLATE_C
#define PLATE_C
#include "TED.hpp"
#define MIN_ANG 3.0
#define MAX_ANG 173.0
#define ERROR_MARG 0.005

class plate{
private:
    Vector2 globalPos;
    Vector2 direction;
    Rectangle boundingBox;
    std::list<Vector2> hull;
    
    float speed;
    Texture2D mapTexture;

    void internalVertTest(plate * primary, plate * secondary, std::vector<std::list<Vector2>::iterator> * PrePointPtr, std::vector<std::list<Vector2>::iterator> * nextPointPtr);
    void VertAngleFilter(plate * primary, double min_angle, double max_angle);

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
    void regenBoundingBox();

    // collision checkers
    bool selfSATCollisionCheck(plate * Collision);
    bool selfAABBCollisionCheck(plate * Collision);
    bool selfCollisionDeformation(plate * Collision);

    void movePlate();
    void movePlateWrapped(int wrap_x, int wrap_y);
    void setPos(Vector2 pos);
    void setPixel(int x, int y);
    void render(int pos_x, int pos_y);


    // static functions

    static void deform(plate * self, plate * other);
};

// initialises the plate with global pos
plate::plate(Image localMap, Vector2 globalPos, Vector2 direction, float speed){
    this->localMap = localMap;
    this->globalPos = globalPos;
    this->color = (Color){
        .r = (unsigned char)(rand()%256),
        .g = (unsigned char)(rand()%256),
        0,
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

// Generate bounding box
void plate::regenBoundingBox(){
    this->boundingBox = {
        this->hull.front().x,
        this->hull.front().y,
        this->hull.front().x,
        this->hull.front().y
    };

    for(Vector2 v1: this->hull){
        this->boundingBox.x = std::min(this->boundingBox.x, v1.x);
        this->boundingBox.y = std::min(this->boundingBox.y, v1.y);
        this->boundingBox.width = std::max(this->boundingBox.width, v1.x);
        this->boundingBox.height = std::max(this->boundingBox.height, v1.y);
    }
}


// Separating Axis Theorem check
bool plate::selfSATCollisionCheck(plate * Collision){
    for(std::list<Vector2>::iterator v1 = this->hull.begin(); v1 != this->hull.end(); ++v1){
        Vector3 temp;
        Vector2 s_v1, s_v2, o_v1;


        int temp_i = 0;

        // stores the Min and Max projections of its own shape
        float s_min = 0, s_max = 0;

        // stores the Min and Max projections of the checked shape
        float o_min = 0, o_max = 0;


        std::list<Vector2>::iterator v2 = v1;
        ++v2;
        if(v2 == this->hull.end()){    // Wrap around to the first element
            v2 = this->hull.begin(); 
        }


        s_v1 = (Vector2){v1->x + this->getPos().x, v1->y + this->getPos().y};
        s_v2 = (Vector2){v2->x + this->getPos().x, v2->y + this->getPos().y};


        Vector3 projectionLine = getPerpindicularBisector(s_v1, s_v2);
        

        // run through self vertices and project onto line
        temp_i = 0;
        for(std::list<Vector2>::iterator v3 = Collision->hull.begin(); v3 != Collision->hull.end(); ++v3){
            o_v1 = (Vector2){v3->x + this->getPos().x, v3->y + this->getPos().y};
            
            temp = getPerpLineThroughPoint(projectionLine,o_v1);



            if(temp_i == 0){
                s_min = temp.z;
                s_max = temp.z;
                temp_i++;
                continue;
            }

            // sets min and max sizes for overlap check
            s_min = std::min(s_min, temp.z);
            s_max = std::max(s_max, temp.z);

        }

        // run through other Collision plate vertices and project onto line
        temp_i = 0;
        for(std::list<Vector2>::iterator v3 = Collision->hull.begin(); v3 != Collision->hull.end(); ++v3){
            o_v1 = (Vector2){v3->x + Collision->getPos().x, v3->y + Collision->getPos().y};
            temp = getPerpLineThroughPoint(projectionLine,o_v1);
            
            if(temp_i == 0){
                o_min = temp.z;
                o_max = temp.z;
                temp_i++;
                continue;
            }
            
            // sets min and max sizes for overlap check
            o_min = std::min(o_min, temp.z);
            o_max = std::max(o_max, temp.z);
        }

        // minimum of other shape is outside the checking shape // maximum of other shape is outside the checking shape // checking shape minimum is outside the other shape
        if( o_max < s_min || s_max < o_min){
            return false; // no collision
        }
        // printf("passed with  %f:%f, %f:%f \n",o_min, o_max, s_min, s_max);
    }

    // run collision callback
    return true; // collision
}

// Axis aligned collision check
bool plate::selfAABBCollisionCheck(plate * Collision){
    
    // minimum of other shape is outside the checking shape // maximum of other shape is outside the checking shape // checking shape minimum is outside the other shape
    if( this->boundingBox.width + this->getPos().x < Collision->boundingBox.x + Collision->getPos().x || 
        Collision->boundingBox.width + Collision->getPos().x < this->boundingBox.x + this->getPos().x){
        // printf("NON X\n");
        return false; // no collision
    }
    
    // minimum of other shape is outside the checking shape // maximum of other shape is outside the checking shape // checking shape minimum is outside the other shape
    if( this->boundingBox.height + this->getPos().y < Collision->boundingBox.y  + Collision->getPos().y || 
        Collision->boundingBox.height + Collision->getPos().y  < this->boundingBox.y + this->getPos().y){
        // printf("NON Y\n");
        return false; // no collision
    }
    // printf("X %f:%f, %f:%f\n",self.x, self.width, other.x, other.width);
    // printf("Y %f:%f, %f:%f\n",self.y, self.height, other.y, other.height);
    return true;
}

// deformation of plates
bool plate::selfCollisionDeformation(plate * Collision){
    Vector2 s_v1, s_v2, o_v1, o_v2;
    Vector3 s_lv, o_lv;


    std::vector<Vector2> selfNewPoint;
    std::vector<std::list<Vector2>::iterator> selfPrePointPtr;
    std::vector<std::list<Vector2>::iterator> selfNextPointPtr;

    std::vector<Vector2> otherNewPoint;
    std::vector<std::list<Vector2>::iterator> otherPrePointPtr;
    std::vector<std::list<Vector2>::iterator> otherNextPointPtr;
    

    // check for edge intersections
    for(auto v1 = this->hull.begin(); v1 != this->hull.end(); ++v1){

        std::list<Vector2>::iterator v2 = v1;
        ++v2;
        if(v2 == this->hull.end()){    // Wrap around to the first element
            v2 = this->hull.begin(); 
        }

        s_v1 = (Vector2){v1->x + this->getPos().x, v1->y + this->getPos().y};
        s_v2 = (Vector2){v2->x + this->getPos().x, v2->y + this->getPos().y};

        s_lv = getLineEquation(s_v1,s_v2);

        // printf("NEW LINE\n");
        for(auto v3 = Collision->hull.begin(); v3 != Collision->hull.end(); ++v3){
            std::list<Vector2>::iterator v4 = v3;
            ++v4;
            if(v4 == Collision->hull.end()){    // Wrap around to the first element
                v4 = Collision->hull.begin(); 
            }


            o_v1 = (Vector2){v3->x + Collision->getPos().x, v3->y + Collision->getPos().y};
            o_v2 = (Vector2){v4->x + Collision->getPos().x, v4->y + Collision->getPos().y};

            o_lv = getLineEquation(o_v1,o_v2);

            // printf("Failing %f,%f,%f => %f,%f,%f \n",s_lv.x,s_lv.y,s_lv.z, o_lv.x, o_lv.y, o_lv.z);

            Vector2 intersection = getIntersector(s_lv,o_lv);

            

            if(std::isnan(intersection.x)){
                continue;
            }

            intersection.x = -intersection.x;
            intersection.y = -intersection.y;

            // printf("insert attempt between \n(%f,%f)=>(%f,%f)\n(%f,%f)=>(%f,%f)\n(%f:%f)\n\n",
            // s_v1.x,s_v1.y,
            // s_v2.x,s_v2.y,
            // o_v1.x,o_v1.y,
            // o_v2.x,o_v2.y,
            // intersection.x, intersection.y            
            // );

            if( //checks if the intersection is within the line segment
                intersection.x >= std::min(s_v1.x, s_v2.x) - ERROR_MARG && intersection.x <= std::max(s_v1.x, s_v2.x) + ERROR_MARG &&
                intersection.y >= std::min(s_v1.y, s_v2.y) - ERROR_MARG && intersection.y <= std::max(s_v1.y, s_v2.y) + ERROR_MARG &&
                intersection.x >= std::min(o_v1.x, o_v2.x) - ERROR_MARG && intersection.x <= std::max(o_v1.x, o_v2.x) + ERROR_MARG &&
                intersection.y >= std::min(o_v1.y, o_v2.y) - ERROR_MARG && intersection.y <= std::max(o_v1.y, o_v2.y) + ERROR_MARG
            ){
                //adds it to a queue to be added

                selfNewPoint.push_back((Vector2){
                        intersection.x - this->getPos().x,
                        intersection.y - this->getPos().y
                    });
                selfPrePointPtr.push_back(v1);
                selfNextPointPtr.push_back(v2);

                otherNewPoint.push_back((Vector2){
                    intersection.x - Collision->getPos().x,
                    intersection.y - Collision->getPos().y
                });
                
                otherPrePointPtr.push_back(v3);
                otherNextPointPtr.push_back(v4);

                // printf("Inserted\n\n");
                
                
                continue;
            }
            
            // printf("fail insert not between\n");
        }        
    }

    // inseting the vertices
    for(int i = 0; i < selfNewPoint.size(); i++){
        // adds it to both hulls
        this->hull.insert(selfNextPointPtr[i],selfNewPoint[i]);
        Collision->hull.insert(otherNextPointPtr[i],otherNewPoint[i]);
    }

    // // check if either of the parent vertexs are inside the other shape
    internalVertTest(this, Collision, &otherPrePointPtr, &otherNextPointPtr);
    internalVertTest(Collision, this, &selfPrePointPtr, &selfNextPointPtr);

    VertAngleFilter(this, MIN_ANG*(PI/180.0), MAX_ANG*(PI/180.0));
    VertAngleFilter(Collision, MIN_ANG*(PI/180.0), MAX_ANG*(PI/180.0));

    // (b.x-a.x)(c.y-a.y)-(b.y-a.y)(c.x-a.x) cross product
    // remove if they are inside
    // 
    // for(auto v1 = this->hull.begin(); v1 != this->hull.end(); ++v1){
    //     std::vector<std::list<Vector2>::iterator> keep; // vector for vertices that are to be kept
    // 
    //     std::list<Vector2>::iterator v2 = v1;
    //     ++v2;
    //     if(v2 == this->hull.end()){    // Wrap around to the first element
    //         v2 = this->hull.begin(); 
    //     }
    // 
    //     s_v1 = (Vector2){v1->x + this->getPos().x, v1->y + this->getPos().y};
    //     s_v2 = (Vector2){v2->x + this->getPos().x, v2->y + this->getPos().y};
    // 
    //     for(auto v3 : otherPrePointPtr){
    //         Vector2 s_v3 = (Vector2){v3->x + Collision->getPos().x, v3->y + Collision->getPos().y};
    // 
    //         float crossvalue = crossproduct(s_v1,s_v2,s_v3);
    //         if(crossvalue < 0){
    //             keep.push_back(v3);
    //         }
    //     }
    //     printf("next vert pair\n");
    // }
    // printf("\n");
    // for(auto v1 = Collision->hull.begin(); v1 != Collision->hull.end(); ++v1){
    //     std::vector<std::list<Vector2>::iterator> keep; // vector for vertices that are to be kept
    // 
    //     std::list<Vector2>::iterator v2 = v1;
    //     ++v2;
    //     if(v2 == Collision->hull.end()){    // Wrap around to the first element
    //         v2 = Collision->hull.begin(); 
    //     }
    // 
    //     s_v1 = (Vector2){v1->x + Collision->getPos().x, v1->y + Collision->getPos().y};
    //     s_v2 = (Vector2){v2->x + Collision->getPos().x, v2->y + Collision->getPos().y};
    // 
    //     // printf("(%f,%f),", s_v1.x, s_v1.y);
    // 
    //     for(auto v3 : selfPrePointPtr){
    //         Vector2 s_v3 = (Vector2){v3->x + this->getPos().x, v3->y + this->getPos().y};
    // 
    //         float crossvalue = crossproduct(s_v1,s_v2,s_v3);
    // 
    //         if(crossvalue < 0){
    //             keep.push_back(v3);
    //         }
    //     }
    // 
    //     // remove vertices that are being kept
    //     for(auto k: keep){
    //         auto purge_node = std::find(selfPrePointPtr.begin(), selfPrePointPtr.end(), k);
    //         if (purge_node != selfPrePointPtr.end()) {
    //             printf("removed\n");
    //             selfPrePointPtr.erase(purge_node);
    //         }
    //     }
    // }
    
    // safety clear
    selfNewPoint.clear();
    selfNextPointPtr.clear();
    selfPrePointPtr.clear();
    otherNewPoint.clear();
    otherPrePointPtr.clear();
    otherNextPointPtr.clear();

}

void plate::internalVertTest(plate * primary, plate * secondary, std::vector<std::list<Vector2>::iterator> * PrePointPtr, std::vector<std::list<Vector2>::iterator> * nextPointPtr){
    for(auto v1 = primary->hull.begin(); v1 != primary->hull.end(); ++v1){
        std::vector<std::list<Vector2>::iterator> keep_pre; // vector for vertices that are to be kept
        std::vector<std::list<Vector2>::iterator> keep_next; // vector for vertices that are to be kept
        
        std::list<Vector2>::iterator v2 = v1;
        ++v2;
        if(v2 == primary->hull.end()){    // Wrap around to the first element
            v2 = primary->hull.begin(); 
        }

        Vector2 s_v1 = (Vector2){v1->x + primary->getPos().x, v1->y + primary->getPos().y};
        Vector2 s_v2 = (Vector2){v2->x + primary->getPos().x, v2->y + primary->getPos().y};

        // printf("(%f,%f),", s_v1.x, s_v1.y);

        for(auto v3 : *PrePointPtr){
            Vector2 s_v3 = (Vector2){v3->x + secondary->getPos().x, v3->y + secondary->getPos().y};

            float crossvalue = crossproduct(s_v1,s_v2,s_v3);

            if(crossvalue < 0){
                keep_pre.push_back(v3);
            }
        }

        // remove vertices that are being kept
        for(auto k: keep_next){
            auto purge_node = std::find(nextPointPtr->begin(), nextPointPtr->end(), k);
            if (purge_node != nextPointPtr->end()) {
                
                nextPointPtr->erase(purge_node);
            }
        }

        for(auto v3 : *nextPointPtr){
            Vector2 s_v3 = (Vector2){v3->x + secondary->getPos().x, v3->y + secondary->getPos().y};

            float crossvalue = crossproduct(s_v1,s_v2,s_v3);

            if(crossvalue < 0){
                keep_next.push_back(v3);
            }
        }

        // remove vertices that are being kept
        for(auto k: keep_pre){
            auto purge_node = std::find(PrePointPtr->begin(), PrePointPtr->end(), k);
            if (purge_node != PrePointPtr->end()) {
                
                PrePointPtr->erase(purge_node);
            }
        }

        for(auto k: keep_next){
            auto purge_node = std::find(nextPointPtr->begin(), nextPointPtr->end(), k);
            if (purge_node != nextPointPtr->end()) {
                
                nextPointPtr->erase(purge_node);
            }
        }
    }


    for(auto ptr : *PrePointPtr){
        for (auto h = secondary->hull.begin(); h != secondary->hull.end();) {  
            if (h->x == ptr->x && h->y == ptr->y) {  
                // printf("erasing pre %f,%f ", h->x + secondary->getPos().x, h->y + secondary->getPos().y);
                h = secondary->hull.erase(h);  // Use returned iterator
            } else {
                ++h;  // Only increment if not erased
            }
        }
    }
    for(auto ptr : *nextPointPtr){
        for (auto h = secondary->hull.begin(); h != secondary->hull.end();) {  
            if (h->x == ptr->x && h->y == ptr->y) {  
                // printf("erasing next %f,%f ", h->x + secondary->getPos().x, h->y + secondary->getPos().y);
                h = secondary->hull.erase(h);  // Use returned iterator
            } else {
                ++h;  // Only increment if not erased
            }
        }
    }
    // printf("\n");

    // for(auto v:secondary->hull){
    //     printf("(%f,%f),",v.x + secondary->getPos().x,v.y + secondary->getPos().y);
    // }
    // printf("\n");

    // for(auto ptr: *nextPointPtr){
    //     auto purge_node = std::find(secondary->hull.begin(), secondary->hull.end(), ptr);
    //     if (purge_node != secondary->hull.end()) {
    //         printf("removed\n");
    //         secondary->hull.erase(purge_node);
    //     }
    // }
}

void plate::VertAngleFilter(plate * primary, double min_angle, double max_angle){
    std::vector<std::list<Vector2>::iterator> removeList;

    for(auto vc = primary->hull.begin(); vc != primary->hull.end(); ++vc){
        auto vp = (vc == primary->hull.begin()) ? std::prev(primary->hull.end()) : std::prev(vc); // previous ptr
        auto vn = std::next(vc); //next
        
        // wrap
        if(vn == primary->hull.end()){
            vn == primary->hull.begin();
        }

        float angle = angleBetween(*vp, *vc, *vn);

        if(angle < min_angle || angle > max_angle){
            removeList.push_back(vc);
        }
        
        // printf("Angle %f\n",angle);
    }

    for(auto ptr : removeList){
        for (auto h = primary->hull.begin(); h != primary->hull.end();) {  
            if (h->x == ptr->x && h->y == ptr->y) {  
                // printf("erasing pre %f,%f ", h->x + secondary->getPos().x, h->y + secondary->getPos().y);
                h = primary->hull.erase(h);  // Use returned iterator
            } else {
                ++h;  // Only increment if not erased
            }
        }
    }
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
    ImageClearBackground(&this->localMap, BLACK);
    UnloadTexture(mapTexture);
    // ImageDrawCircle(&this->localMap, 5,5,10,RED);
    ImageDrawCircle(&this->localMap, this->localMap.width/2,this->localMap.height/2,3,BLUE);


    int i = 0;
    Vector2 vp = this->hull.back();
    for(Vector2 v : this->hull){
        i++;
        // printf("%d,%d\n",v.x,v.y);
        ImageDrawLine(&this->localMap, vp.x + this->localMap.width/2,vp.y + this->localMap.height/2,v.x + this->localMap.width/2,v.y + this->localMap.height/2,PINK);
        ImageDrawCircle(&this->localMap, v.x + this->localMap.width/2,v.y + this->localMap.height/2,5,GREEN);
        ImageDrawText(&this->localMap,std::to_string(i).c_str(), v.x + 10 + this->localMap.width/2,v.y + 10 + this->localMap.height/2,15,WHITE);
        vp = v;
    }

    int lx[9] = {-1, 0, 1, 1, 1, 0,-1,-1,-1};
    int ly[9] = {-1,-1,-1, 0, 1, 1, 1, 0,-1};

    i = 0;
    // for(Vector2 v : this->mpoints){
    //     i++;
    //     // printf("%d,%d\n",v.x,v.y);
    //     std::ostringstream temp;
    //     temp << lx[i] << ":" << ly[i];
    //     ImageDrawCircle(&this->localMap, v.x + this->localMap.width/2,v.y + this->localMap.height/2,5,RED);
    //     ImageDrawText(&this->localMap, temp.str().c_str(), v.x + 10 + this->localMap.width/2,v.y + 10 + this->localMap.height/2,15,WHITE);
    // }


    mapTexture = LoadTextureFromImage(this->localMap);
    // printf("REN %f:%d,, %f:%d\n",pos.x,pos_x,pos.y,pos_y);
    DrawTexture(mapTexture,
    globalPos.x + pos_x - this->localMap.width/2,
    globalPos.y + pos_y - this->localMap.height/2, 
    WHITE);    
}

void plate::deform(plate * self, plate * other){
    
}
#endif