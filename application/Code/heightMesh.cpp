#ifndef HEIGHT_MESH_C
#define HEIGHT_MESH_C

#include "TED.hpp"

#define DEFAULT_HEIGHT 100

class heightMesh{
private:
    int depth;
    int width;
    Vector3 ** meshPoints;
    Vector3 centerPoint;
public:
    heightMesh();
    ~heightMesh();

    void initMesh(int depth, std::list<Vector2>);

    void stretchDeform(Vector2 Direction);
    void propogateDeform(Vector2 Origin, Vector2 Direction, float force);
    void render(Vector2 offset);
};

heightMesh::heightMesh(){

}

void heightMesh::initMesh(int depth, std::list<Vector2> hull){
    centerPoint = {0,0,DEFAULT_HEIGHT};

    meshPoints = new Vector3 * [depth];
    this->width = hull.size();
    this->depth = depth;

    float x = 0, y = 0;


    for(int i = 0; i < depth; i++){
        meshPoints[i] = new Vector3[width];

        float meshMultiplier = (depth - i) / (float)depth;
        printf("MESH M %f",meshMultiplier);

        auto h = hull.begin();
        for(int j = 0; j < width; j++, ++h){

            x = h->x*(meshMultiplier);
            y = h->y*(meshMultiplier);

            meshPoints[i][j] = {x,y, DEFAULT_HEIGHT};
        }
    }

}

heightMesh::~heightMesh(){
}

void heightMesh::stretchDeform(Vector2 Direction){
    for(int i = 0; i < depth; i++){

        for(int j = 0; j < width; j++){
            meshPoints[i][j].x *= abs(Direction.x);
            meshPoints[i][j].y *= abs(Direction.y);
        }
    }
}

void heightMesh::propogateDeform(Vector2 Origin, Vector2 Direction, float force){
    //get distance to origin 
    int originIndex;
    float closest_dist = __FLT_MAX__;
    float temp_dist = 0;

    for(int w  = 0; w < this->width; w++){
        temp_dist = Vector2Distance({meshPoints[0][w].x,meshPoints[0][w].y},Origin);

        if(temp_dist < closest_dist){
            closest_dist = temp_dist;
            originIndex = w;
        }
    }

    //use closest points to propogsate deform
    for(int i = 0; i < depth; i++){//deform originator line
        meshPoints[i][originIndex].x += Direction.x * force/((depth-i)/depth);
        meshPoints[i][originIndex].y += Direction.y * force/((depth-i)/depth);
        meshPoints[i][originIndex].z += force/((depth-i)/depth);
    }

    for(int d = 1; d < depth && d < width/2; d++){
        
        // left propogration
        // meshPoints[0][(width + originIndex - d)%width];
        // right propogration
        // meshPoints[0][(originIndex + d)%width];

        for(int w = 0; w + d < depth; w++){
            // left propogration
            
            meshPoints[w][(width + originIndex - d)%width].x += Direction.x * force/((depth-(w+d))/depth);
            meshPoints[w][(width + originIndex - d)%width].y += Direction.y * force/((depth-(w+d))/depth);
            meshPoints[w][(width + originIndex - d)%width].z += force/((depth-(w+d))/depth);
            // right propogration

            meshPoints[w][(originIndex + d)%width].x += Direction.x * force/((depth-(w+d))/depth);
            meshPoints[w][(originIndex + d)%width].y += Direction.y * force/((depth-(w+d))/depth);
            meshPoints[w][(originIndex + d)%width].z += force/((depth-(w+d))/depth);
        }
        // propbably easier to do recursively.....

    }

    // // propogations slows to 0%~ at max depth
}

void heightMesh::render(Vector2 offset){
    for(int d = 0; d < this->depth; d++){
        for(int w  = 0; w < this->width; w++){
            DrawCircle(meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, 5, ORANGE);
        }
    }
}

#endif