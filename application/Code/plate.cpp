#ifndef PLATE_C
#define PLATE_C
#include "TED.hpp"
#define MIN_ANG 10.0
#define MAX_ANG 177.0
#define MAX_DISTANCE 1
#define ERROR_MARG 1
#define FORCE_DEFAULT 3.0

class plate{
private:
    Vector2 globalPos;
    Vector2 direction;
    Rectangle boundingBox;
    std::list<Vector2> hull;
    heightMesh * mesh;
    heightMesh * tempMesh;

    void internalVertTest(plate * primary, plate * secondary, Vector2 Offset, std::vector<Vector2> ignore);
    void VertRayTest(Vector2 Vertex, plate * primary, plate * secondary,  Vector2 Offset);
    void VertAngleFilter(plate * primary, double min_angle, double max_angle);
    void VertDistFilter(plate * primary, double min_distance);

public:
    Color DebugRect = GREEN;
    float speed;

    plate(Vector2 globalPos, Vector2 direction, float speed);
    ~plate();
    Vector2 getPos();
    Vector2 getDirection();
    std::list<Vector2>& getHull();
    Rectangle getBoundingBox();
    heightMesh * getMesh();

    Vector2 regenBoundingBox(int maxWidth, int maxHeight);
    void initHeightMesh(int depth);
    void rebuildHeightMesh();

    // collision checkers
    bool selfSATCollisionCheck(plate * Collision);
    bool selfAABBCollisionCheck(plate * Collision, Vector2 Offset);
    bool selfCollisionDeformation(plate * Collision, Vector2 Offset);
    bool internalTest(Vector2 Offset);

    void movePlate();
    void movePlateWrapped(int wrap_x, int wrap_y);


    void AngleFilter();

    void DeformBackfill();

    void setPos(Vector2 pos);
    void setPixel(int x, int y);

    void RebuildPlate();

    void render(int pos_x, int pos_y);
};

// initialises the plate with global pos
plate::plate(Vector2 globalPos, Vector2 direction, float speed){
    this->globalPos = globalPos;
    this->direction = direction;
    this->speed = speed;

    tempMesh = new heightMesh();
    mesh = new heightMesh();
}

plate::~plate(){
    printf("closing plate\n");
}


Vector2 plate::getPos(){
    return (Vector2){
        globalPos.x,
        globalPos.y
    };
}

Vector2 plate::getDirection(){
    return this->direction;
}

std::list<Vector2>& plate::getHull(){
    return this->hull;
}

Rectangle plate::getBoundingBox(){
    return this->boundingBox;
}

heightMesh * plate::getMesh(){
    return this->mesh;
}

// Generate bounding box
Vector2 plate::regenBoundingBox(int maxWidth, int maxHeight){
    Vector2 TempOffset;
    Rectangle tempBox = this->boundingBox;

    this->boundingBox = {
        this->hull.front().x,
        this->hull.front().y,
        this->hull.front().x,
        this->hull.front().y
    };

    //gets the minimum and maximum vertices for the bounding box
    for(Vector2 v1: this->hull){
        this->boundingBox.x = std::min(this->boundingBox.x, v1.x);
        this->boundingBox.y = std::min(this->boundingBox.y, v1.y);
        this->boundingBox.width = std::max(this->boundingBox.width, v1.x);
        this->boundingBox.height = std::max(this->boundingBox.height, v1.y);
    }

    // recentering
    TempOffset.x = (this->boundingBox.width + this->boundingBox.x)/2.0;
    TempOffset.y = (this->boundingBox.height + this->boundingBox.y)/2.0;

    // moves the global position 
    this->globalPos.x += TempOffset.x;
    this->globalPos.y += TempOffset.y;
    
    this->globalPos.x = fmod(maxWidth + this->globalPos.x,maxWidth);
    this->globalPos.y = fmod(maxHeight + this->globalPos.y,maxHeight);

    for(Vector2 &v1: this->hull){
        v1.x -= TempOffset.x;
        v1.y -= TempOffset.y;
    }

    return {boundingBox.x/tempBox.x,boundingBox.y/tempBox.y};
}


void plate::initHeightMesh(int depth){
    this->mesh->initMesh(depth, hull);
}

// rebuilds the heightmesh 
void plate::rebuildHeightMesh(){
    mesh->processForceQueue();  //processes all of the queued forces
    // tempMesh->freeMeshPoints(); //frees the mesh (disabled as compiler seems to clean it up automatically)
    tempMesh->initMesh(mesh->getDepth(), this->hull); //initialises the mesh
    tempMesh->Reshape(*mesh);   //reshapes the heightmesh based on nearest points of the old mesh
    
    // used to swap the height meshes
    heightMesh * a, * b;

    a = tempMesh;
    b = mesh;

    mesh = a;
    tempMesh = b;
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
    }

    // run collision callback
    return true; // collision
}

// Axis aligned collision check
bool plate::selfAABBCollisionCheck(plate * Collision, Vector2 Offset){
    
    // minimum of other shape is outside the checking shape // maximum of other shape is outside the checking shape // checking shape minimum is outside the other shape
    if( this->boundingBox.width + this->getPos().x < Collision->boundingBox.x + Collision->getPos().x + Offset.x|| 
        Collision->boundingBox.width + Collision->getPos().x + Offset.x< this->boundingBox.x + this->getPos().x){
        return false; // no collision
    }
    
    // minimum of other shape is outside the checking shape // maximum of other shape is outside the checking shape // checking shape minimum is outside the other shape
    if( this->boundingBox.height + this->getPos().y < Collision->boundingBox.y  + Collision->getPos().y + Offset.y || 
        Collision->boundingBox.height + Collision->getPos().y + Offset.y < this->boundingBox.y + this->getPos().y){
        return false; // no collision
    }

    return true;
}

// deformation of plates
bool plate::selfCollisionDeformation(plate * Collision, Vector2 Offset){
    Vector2 s_v1, s_v2, o_v1, o_v2;
    Vector3 s_lv, o_lv;


    std::vector<Vector2> selfNewPoint;  //new point that is to be added
    std::vector<std::list<Vector2>::iterator> selfPrePointPtr;
    std::vector<std::list<Vector2>::iterator> selfNextPointPtr;

    std::vector<Vector2> otherNewPoint; //new point that is to be added
    std::vector<std::list<Vector2>::iterator> otherPrePointPtr;
    std::vector<std::list<Vector2>::iterator> otherNextPointPtr;
    

    // check for edge intersections
    for(auto v1 = this->hull.begin(); v1 != this->hull.end(); ++v1){

        std::list<Vector2>::iterator v2 = v1;
        ++v2;
        if(v2 == this->hull.end()){     // Wrap around to the first element
            v2 = this->hull.begin(); 
        }

        s_v1 = (Vector2){v1->x + this->getPos().x, v1->y + this->getPos().y};
        s_v2 = (Vector2){v2->x + this->getPos().x, v2->y + this->getPos().y};

        //gets line equation that goes through the two points
        s_lv = getLineEquation(s_v1,s_v2);

        // loop through the opposing plates hull
        for(auto v3 = Collision->hull.begin(); v3 != Collision->hull.end(); ++v3){
            std::list<Vector2>::iterator v4 = v3;
            ++v4;
            if(v4 == Collision->hull.end()){    // Wrap around to the first element
                v4 = Collision->hull.begin(); 
            }


            o_v1 = (Vector2){v3->x + Collision->getPos().x +Offset.x, v3->y + Collision->getPos().y + Offset.y};
            o_v2 = (Vector2){v4->x + Collision->getPos().x +Offset.x, v4->y + Collision->getPos().y + Offset.y};

            //gets line equation that goes through the two points
            o_lv = getLineEquation(o_v1,o_v2);

        
            //find point of intersection
            Vector2 intersection = getLineIntersector(s_lv,o_lv);

            if(std::isnan(intersection.x)){ //skips it if lines are parrel
                continue;
            }

            if( //checks if the intersection is within the line segment
                intersection.x >= std::min(s_v1.x, s_v2.x) -0.1f && intersection.x <= std::max(s_v1.x, s_v2.x) + 0.1f &&
                intersection.y >= std::min(s_v1.y, s_v2.y) -0.1f && intersection.y <= std::max(s_v1.y, s_v2.y) + 0.1f &&
                intersection.x >= std::min(o_v1.x, o_v2.x) -0.1f && intersection.x <= std::max(o_v1.x, o_v2.x) + 0.1f &&
                intersection.y >= std::min(o_v1.y, o_v2.y) -0.1f && intersection.y <= std::max(o_v1.y, o_v2.y) + 0.1f &&

                // Get distance from both lines, helps offset any floating point errors
                abs(crossproduct(s_v1, s_v2, intersection)) <= 0.1f &&
                abs(crossproduct(o_v1, o_v2, intersection)) <= 0.1f
            ){
                //adds it to a queue to be added

                Vector2 momentum = { //disabled due to it not working
                    0,// ((this->getDirection().x * this->speed) - (Collision->getDirection().x * Collision->speed))/4,
                    0// ((this->getDirection().y * this->speed) - (Collision->getDirection().y * Collision->speed))/4
                };
                
                Vector2 temp = {
                    intersection.x - this->getPos().x - momentum.x,
                    intersection.y - this->getPos().y - momentum.y
                };

                //queues the points to be added
                selfNewPoint.push_back(temp);
                selfPrePointPtr.push_back(v1);
                selfNextPointPtr.push_back(v2);

                temp = {
                    intersection.x - Collision->getPos().x - Offset.x - momentum.x,
                    intersection.y - Collision->getPos().y - Offset.y - momentum.y
                };

                //queues the points to be added
                otherNewPoint.push_back(temp);
                otherPrePointPtr.push_back(v3);
                otherNextPointPtr.push_back(v4);                
                continue;
            }
        }        
    }

    // inseting the vertices
    for(int i = 0; i < selfNewPoint.size(); i++){
        // adds the new vertice to the hull and queues a force to be enacted onto the height mesh
        if(this->hull.size() > 30){//emergancy exit
            break;
        }
        this->hull.insert(selfNextPointPtr[i],selfNewPoint[i]);
        this->getMesh()->queueForces((void *)Collision, Vector2Subtract(Collision->getPos(), this->getPos()), selfNewPoint[i], FORCE_DEFAULT);
    }

    // inseting the vertices
    for(int i = 0; i < otherNewPoint.size(); i++){
        // adds the new vertice to the hull and queues a force to be enacted onto the height mesh
        if(Collision->hull.size() > 30){ //emergancy exit
            break;
        }
        Collision->hull.insert(otherNextPointPtr[i],otherNewPoint[i]);
        Collision->getMesh()->queueForces((void *)this, Vector2Subtract(this->getPos(), Collision->getPos()), otherNewPoint[i], FORCE_DEFAULT);
    }

    //removing vertices that are inside other plates
    internalVertTest(this, Collision, Offset, otherNewPoint);
    internalVertTest(Collision, this, {-Offset.x,-Offset.y}, selfNewPoint);

    //removing vertices that are form too sharp bends or too shallow bends
    VertAngleFilter(this, MIN_ANG*(PI/180.0), MAX_ANG*(PI/180.0));
    VertAngleFilter(Collision, MIN_ANG*(PI/180.0), MAX_ANG*(PI/180.0));
    
    //removing vertices that are too close to others
    VertDistFilter(this,1);
    VertDistFilter(Collision,1);    
    
    
    
    // safety clear
    selfNewPoint.clear();
    selfNextPointPtr.clear();
    selfPrePointPtr.clear();
    otherNewPoint.clear();
    otherPrePointPtr.clear();
    otherNextPointPtr.clear();
}

void plate::internalVertTest(plate * primary, plate * secondary, Vector2 Offset, std::vector<Vector2> ignore){
    std::list<std::list<Vector2>::iterator> removeList;


    // adds all the vertices to the remove list
    for (auto h = secondary->hull.begin(); h != secondary->hull.end(); ++h) {

        auto h_prev = (h == secondary->hull.begin()) ? std::prev(secondary->hull.end()) : std::prev(h);
        auto h_next = (std::next(h) == secondary->hull.end()) ? secondary->hull.begin() : std::next(h);

        bool found = false;
        for(auto v : ignore){ //skips the vertice if its in the ignore list
            if(h->x == v.x && h->y == v.y){
                found = true;
                break;
            }
        }


        //doesnt add vertices to the remove list if theyre too far
        if(Vector2Distance(*h, *h_next) > 50 && Vector2Distance(*h, *h_prev) > 50){
            continue;;
        }

        

        Vector2 global_h = Vector2Add(*h, secondary->getPos()); // changes the local vector to a global vector
        Vector2 global_offset_h = Vector2Add(global_h, Offset); // adjusts the global vector by the offset
        Vector2 relative_h = Vector2Subtract(global_offset_h, primary->getPos()); // changes the global vector to a vector relative to this plate

        // doesnt add this vertice to the remove list if its outside the bounding box;
        if(relative_h.x < primary->boundingBox.x - 10 || relative_h.x > primary->boundingBox.width + 10){
            continue;
        }
        if(relative_h.y < primary->boundingBox.y - 10 || relative_h.y > primary->boundingBox.height + 10){
            continue;
        }

        if(!found){
            removeList.push_back(h);
        }
    }

    // loops through remove list
    for(auto r = removeList.begin(); r != removeList.end();){
        Vector2 hp, hn;
        int intersections = 0;

        for(auto h = secondary->getHull().begin(); h != secondary->getHull().end(); ++h){
            if(*r == h){
                if (h == secondary->getHull().begin()) {
                    hp = *std::prev(secondary->getHull().end());
                } else {
                    hp = *std::prev(h);
                }


                if (std::next(h) == secondary->getHull().end()) {
                    hn = *secondary->getHull().begin();
                } else {
                    hn = *std::next(h);
                }

                break;
            }
        }

        Vector2 global_secondary_vertice = (Vector2){(*(*r)).x + secondary->getPos().x + Offset.x, (*(*r)).y + secondary->getPos().y + Offset.y};

        // printf("GSV %f,%f\n", global_secondary_vertice.x, global_secondary_vertice.y);

        Vector3 testingLine = getLineEquation(global_secondary_vertice,primary->getPos());

        // loops through all hull vertices
        for(auto p_vc = primary->hull.begin(); p_vc != primary->hull.end(); ++p_vc){
            auto p_vn = std::next(p_vc); //next

            if(p_vn == primary->hull.end()){
                p_vn = primary->hull.begin();
            }

            // gets the global position of vertices
            Vector2 global_primary_vertice1 = (Vector2){(p_vc->x) + primary->getPos().x, (p_vc->y) + primary->getPos().y};
            Vector2 global_primary_vertice2 = (Vector2){(p_vn->x) + primary->getPos().x, (p_vn->y) + primary->getPos().y};


            Vector3 lineEdge = getLineEquation(global_primary_vertice1,global_primary_vertice2);

            Vector2 intersect_point = getLineIntersector(testingLine, lineEdge);


            if( //checks if the intersection is within the line segment
                intersect_point.x >= std::min(global_primary_vertice1.x, global_primary_vertice2.x) && intersect_point.x <= std::max(global_primary_vertice1.x, global_primary_vertice2.x) &&
                intersect_point.y >= std::min(global_primary_vertice1.y, global_primary_vertice2.y) && intersect_point.y <= std::max(global_primary_vertice1.y, global_primary_vertice2.y) &&
                intersect_point.x >= std::min(global_secondary_vertice.x, primary->getPos().x) && intersect_point.x <= std::max(global_secondary_vertice.x, primary->getPos().x) &&
                intersect_point.y >= std::min(global_secondary_vertice.y, primary->getPos().y) && intersect_point.y <= std::max(global_secondary_vertice.y, primary->getPos().y)

            ){
                intersections++;
                
                // /*DEBUG*/printf("Intersection Passed (%f,%f),(%f,%f)\t%d\t%f,%f\n",global_secondary_vertice.x, global_secondary_vertice.y,primary->getPos().x,primary->getPos().y,intersections, intersect_point.x, intersect_point.y);
                // /*DEBUG*/printf("FROM: (%f,%f),(%f,%f)\n",global_primary_vertice1.x,global_primary_vertice1.y, global_primary_vertice2.x, global_primary_vertice2.y);
            }

        }
        if((intersections % 2) != 0){
            // printf("(%f,%f),",(*(*r)).x + secondary->getPos().x, (*(*r)).y + secondary->getPos().y);
            r = removeList.erase(r);    //returns pointer to the next element if erased
        }       
        else{    //increments if nothing was erased
            // /*DEBUG*/printf("(%f,%f),(%f,%f) %d\n",global_secondary_vertice.x, global_secondary_vertice.y, primary->getPos().x,primary->getPos().y, intersections);
            ++r;
        }

    }

    // printf("\b \n");
    for(auto r : removeList){
        // printf("NR (%f,%f)\n",(*r).x + secondary->getPos().x, (*r).y + secondary->getPos().y);
        for (auto h = secondary->hull.begin(); h != secondary->hull.end();) {  
            if (h == r) {  

                // /*DEBUG INTERNAL*/printf("DEBUG RM INTERNAL_VERT %f,%f ::%f\n", h->x + secondary->getPos().x, h->y + secondary->getPos().y);
                h = secondary->hull.erase(h);  // Use returned iterator
            } else {
                ++h;  // Only increment if not erased
            }
        }
    }
}

void plate::VertRayTest(Vector2 Vertex, plate * primary, plate * secondary,  Vector2 Offset){
    int intersections = 0;

    Vector2 global_primary_vertice = (Vector2){Vertex.x + secondary->getPos().x + Offset.x, Vertex.y + secondary->getPos().y + Offset.y};

    // printf("GSV %f,%f\n", global_secondary_vertice.x, global_secondary_vertice.y);

    Vector3 testingLine = getLineEquation(global_primary_vertice,secondary->getPos());

    // loops through all hull vertices
    for(auto p_vc = secondary->hull.begin(); p_vc != secondary->hull.end(); ++p_vc){
        auto p_vn = std::next(p_vc); //next

        if(p_vn == secondary->hull.end()){
            p_vn = secondary->hull.begin();
        }

        // gets the global position of vertices
        Vector2 global_secondary_vertice1 = (Vector2){(p_vc->x * 0.96f) + secondary->getPos().x, (p_vc->y * 0.96f) + secondary->getPos().y};
        Vector2 global_secondary_vertice2 = (Vector2){(p_vn->x * 0.96f) + secondary->getPos().x, (p_vn->y * 0.96f) + secondary->getPos().y};


        Vector3 lineEdge = getLineEquation(global_secondary_vertice1,global_secondary_vertice2);

        Vector2 intersect_point = getLineIntersector(testingLine, lineEdge);
        // printf("%f:%f   ", intersect_point.x,intersect_point.y);

        // intersect_point.x = -intersect_point.x;
        // intersect_point.y = -intersect_point.y;


        if( //checks if the intersection is within the line segment
            intersect_point.x >= std::min(global_secondary_vertice2.x, global_secondary_vertice2.x) && intersect_point.x <= std::max(global_secondary_vertice1.x, global_secondary_vertice2.x) &&
            intersect_point.y >= std::min(global_secondary_vertice2.y, global_secondary_vertice2.y) && intersect_point.y <= std::max(global_secondary_vertice1.y, global_secondary_vertice2.y) &&
            intersect_point.x >= std::min(global_primary_vertice.x, secondary->getPos().x) && intersect_point.x <= std::max(global_primary_vertice.x, secondary->getPos().x) &&
            intersect_point.y >= std::min(global_primary_vertice.y, secondary->getPos().y) && intersect_point.y <= std::max(global_primary_vertice.y, secondary->getPos().y)

        ){
            // printf("Intersection Passed %f,%f\t%d\t%f,%f\n",global_secondary_vertice.x, global_secondary_vertice.y,intersections, intersect_point.x, intersect_point.y);
            // printf("FROM: (%f,%f),(%f,%f)\n",global_primary_vertice1.x,global_primary_vertice1.y, global_primary_vertice2.x, global_primary_vertice2.y);
            intersections++;
        }

    }
    if((intersections % 2) != 0){
        // printf("(%f,%f),",(*(*r)).x + secondary->getPos().x, (*(*r)).y + secondary->getPos().y);
        for (auto h = secondary->hull.begin(); h != secondary->hull.end();) {  
            if (h->x == Vertex.x && h->y == Vertex.y) {  

                ///*DEBUG*/printf("DEBUG RM INTERNAL_VERT %f,%f \n", h->x + secondary->getPos().x, h->y + secondary->getPos().y);
                h = secondary->hull.erase(h);  // Use returned iterator
            } else {
                ++h;  // Only increment if not erased
            }
        }
    }       


}


bool plate::internalTest(Vector2 testPos){    

    for(auto p_vc = this->hull.begin(); p_vc != this->hull.end(); ++p_vc){
        // auto p_vp = (p_vc == primary->hull.begin()) ? std::prev(primary->hull.end()) : std::prev(p_vc); // previous ptr with wrap around
        auto p_vn = std::next(p_vc); //next

        if(p_vn == this->hull.end()){
            p_vn = this->hull.begin();
        }
        
        Vector2 global_this_vertice1 = (Vector2){p_vc->x + this->getPos().x, p_vc->y + this->getPos().y};
        Vector2 global_this_vertice2 = (Vector2){p_vn->x + this->getPos().x, p_vn->y + this->getPos().y};

        float crossvalue = crossproduct(global_this_vertice1, global_this_vertice2, testPos);

        if(crossvalue < 0){
            return true;
        }

    }

    return false;

}

void plate::VertAngleFilter(plate * primary, double min_angle, double max_angle){
    std::vector<std::list<Vector2>::iterator> removeList;

    for(auto vc = primary->hull.begin(); vc != primary->hull.end(); ++vc){
        auto vp = (vc == primary->hull.begin()) ? std::prev(primary->hull.end()) : std::prev(vc); // previous ptr
        auto vn = std::next(vc); //next
        
        // wrap
        if(vn == primary->hull.end()){
            vn = primary->hull.begin();
        }

        float angle = angleBetween(*vp, *vc, *vn);
        if(
            (angle < min_angle || angle > max_angle)
            // && (sqrt(pow(vn->x - vc->x,2) + pow(vn->y - vc->y,2) < this->speed*3))
            // && (sqrt(pow(vc->x - vp->x,2) + pow(vc->y - vp->y,2) < this->speed*3))
            
        ){
            removeList.push_back(vc);
        }
        
        // printf("Angle %f\n",angle);
    }

    for(auto ptr : removeList){
        for (auto h = primary->hull.begin(); h != primary->hull.end();) {  
            if (h == ptr) {  
                // /*DEBUG ANG*/printf("DEBUG RM ANGLE %f,%f \n", h->x + primary->getPos().x, h->y + primary->getPos().y);
                h = primary->hull.erase(h);  // Use returned iterator
            } else {
                ++h;  // Only increment if not erased
            }
        }
    }
}

void plate::AngleFilter(){
    VertAngleFilter(this, MIN_ANG*(PI/180.0), MAX_ANG*(PI/180.0));
}


void plate::VertDistFilter(plate * primary, double min_distance){
    std::vector<std::list<Vector2>::iterator> removeList;

    for(auto vc = primary->hull.begin(); vc != primary->hull.end(); ++vc){
        auto vp = (vc == primary->hull.begin()) ? std::prev(primary->hull.end()) : std::prev(vc);
        auto vn = std::next(vc); //next
        
        if(vc == primary->hull.end()){
            vn = primary->hull.begin();
        }


        // gets the distance between points and removes the point if its too close to others
        if(Vector2Distance(*vc, *vn) + Vector2Distance(*vp, *vc) <= min_distance){
            removeList.push_back(vc);
        }
    }
    for(auto ptr : removeList){
        for (auto h = primary->hull.begin(); h != primary->hull.end();) {  
            if (h->x == ptr->x && h->y == ptr->y) {  
                // /*DEBUG DIST*/printf("DEBUG RM DISTANCE %f,%f \n", h->x + primary->getPos().x, h->y + primary->getPos().y);
                h = primary->hull.erase(h);  // Use returned iterator
                break;
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

void plate::DeformBackfill(){
    std::vector<Vector2> selfNewPoint;
    std::vector<std::list<Vector2>::iterator> selfNextPointPtr;
    std::vector<std::list<Vector2>::iterator> removeList;

    for(auto p_vc = this->hull.begin(); p_vc != this->hull.end(); ++p_vc) {
        auto p_vn = std::next(p_vc); // next
        auto p_vp = (p_vc == this->hull.begin()) ? std::prev(this->hull.end()) : std::prev(p_vc); // previous ptr
        
        if(p_vn == this->hull.end()) {
            p_vn = this->hull.begin(); 
        }


        // normal of edge
        Vector2 normal_p = Vector2Normalize({-(p_vp->y - p_vc->y),(p_vp->x - p_vc->x)});
        Vector2 normal_n = Vector2Normalize({-(p_vc->y - p_vn->y),(p_vc->x - p_vn->x)});
        // Vector2 normal_n = Vector2Normalize({-(p_vn->y - p_vc->y),(p_vn->x - p_vc->x)});
        
        // dot product
        if((normal_p.x*direction.x + normal_p.y*direction.y) >= 0){
            continue;
        }

        // send point back

        selfNewPoint.push_back((Vector2){
            p_vp->x - (direction.x * speed),
            p_vp->y - (direction.y * speed)
        });
        selfNextPointPtr.push_back(p_vc);

        // check for next edge to see if its facing
        // dot product
        // queue center vertex to be removed if the next edge is facing away from direction
        if((normal_n.x*direction.x + normal_n.y*direction.y) < 0){
            removeList.push_back(p_vc);
            continue;
        }

        // // send point back
        selfNewPoint.push_back((Vector2){
            p_vc->x - direction.x * speed,
            p_vc->y - direction.y * speed
        });
        selfNextPointPtr.push_back(p_vc);
        
    }

    // add points
    for(int i = 0; i < selfNewPoint.size(); i++){
        // adds it to both hulls
        this->hull.insert(selfNextPointPtr[i],selfNewPoint[i]);

        // we use the diagonal distance of the bounding box to ensure that the direction line can intersect with the mesh edge
        float max_dist = Vector2Distance({this->boundingBox.x,this->boundingBox.y}, {this->boundingBox.width, this->boundingBox.height});

        this->mesh->queueForces((void *)this, Vector2Multiply(this->direction,{-max_dist,-max_dist}), selfNewPoint[i], -FORCE_DEFAULT * 0.5);
    }

    // remove points
    for(auto ptr : removeList){
        for (auto h = this->hull.begin(); h != this->hull.end();) {  
            if (h->x == ptr->x && h->y == ptr->y) {  
                // printf("erasing pre %f,%f ", h->x + secondary->getPos().x, h->y + secondary->getPos().y);
                h = this->hull.erase(h);  // Use returned iterator
            } else {
                ++h;  // Only increment if not erased
            }
        }
    }

    selfNewPoint.clear();
    selfNextPointPtr.clear();
    removeList.clear();
}

void plate::RebuildPlate(){
    std::list<Vector2> rebuiltHull;

    //gets longest diagonal of the bounding box
    float diagonalLength = Vector2Distance({this->boundingBox.x, this->boundingBox.y}, {this->boundingBox.width, this->boundingBox.height});

    for(auto vc = this->hull.begin(); vc != hull.end(); ++vc){
        auto vp = (vc == this->hull.begin()) ? std::prev(this->hull.end()) : std::prev(vc); // previous ptr
        auto vn = std::next(vc); //next

        // we get the point from the current hull and check its distance compared to the next point

        if(Vector2Distance(*vc, *vn) + Vector2Distance(*vp, *vc) < 10){
            continue; //we skip it if its within the distance, this helps reduce mesh clutter
        }


        rebuiltHull.push_back(*vc);
    }

    this->hull.clear();
    this->hull = rebuiltHull;
    rebuiltHull.clear();

    if(this->hull.size() > 30){
        printf("S: %d\n",this->hull.size());
        this->hull.clear();
        this->mesh->rebuildHullfromMesh(hull);        
    }
    

    rebuildHeightMesh();

}

void plate::setPos(Vector2 pos){
    this->globalPos = pos;
}

// renders the plate (only renders mesh for final output)
void plate::render(int pos_x, int pos_y){
    int offset_x = globalPos.x + pos_x;
    int offset_y = globalPos.y + pos_y;


    // DrawCircle(offset_x,offset_y,3,BLUE);
    // DrawLine(offset_x,offset_y,direction.x*50 + offset_x,direction.y*50 + offset_y,PINK);
    // DrawText(std::to_string(this->hull.size()).c_str(), offset_x,offset_y,15,WHITE);
    // DrawRectangleLines(this->boundingBox.x + offset_x,this->boundingBox.y + offset_y, this->boundingBox.width - this->boundingBox.x, this->boundingBox.height - this->boundingBox.y,DebugRect);


    // int i = 0;
    // Vector2 vp = this->hull.back();
    // for(Vector2 v : this->hull){
    //     i++;
    //     // printf("%d,%d\n",v.x,v.y);
    //     // Vector2 normal_p = Vector2Normalize({-(vp.y - v.y),(vp.x - v.x)});
    //     // float dir = (normal_p.x*direction.x + normal_p.y*direction.y);

    //     // Color edgeColor = dir > 0? GREEN:RED;

    //     // DrawLineEx({vp.x + offset_x,vp.y + offset_y},{v.x + offset_x,v.y + offset_y},5,edgeColor);

    //     // DrawCircle(vp.x + offset_x, vp.y + offset_y,3,PINK);
    //     // DrawLine(offset_x,offset_y,v.x + offset_x,v.y + offset_y,PURPLE);

        
    //     // DrawCircle(v.x + offset_x,v.y + offset_y,5,GREEN);
    //     // DrawText(std::to_string(i).c_str(), v.x + 10 + offset_x,v.y + 10 + offset_y,15,color);
    //     // DrawText(std::to_string((int)round(v.x)).c_str(), v.x + 10 + offset_x,v.y + 20 + offset_y,15,WHITE);
        
    //     vp = v;
    // }
    
    mesh->render({
        (float)offset_x,
        (float)offset_y
    }); 
}

#endif