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

    void stretchDeform(Vector2 Direction, Vector2 Boundary);
    void propogateDeform(Vector2 Origin, Vector2 Direction, float force);
    void render(Vector2 offset);
};

heightMesh::heightMesh(){

}

void heightMesh::initMesh(int depth, std::list<Vector2> hull){
    centerPoint = {0,0,DEFAULT_HEIGHT};

    meshPoints = new Vector3 * [depth];
    this->width = hull.size() * 2;
    this->depth = depth;

    float x = 0, y = 0;


    for(int i = 0; i < depth; i++){
        meshPoints[i] = new Vector3[width];

        float meshMultiplier = (depth - i) / (float)depth;
        // printf("MESH M %f",meshMultiplier);

        auto h = hull.begin();
        
        for(int j = 0; j < width; j++, ++h){
            auto hn = (std::next(h) == hull.end()) ? hull.begin() : std::next(h);
            x = h->x*(meshMultiplier);
            y = h->y*(meshMultiplier);

            meshPoints[i][j] = {x,y, DEFAULT_HEIGHT};

            x = (h->x*(meshMultiplier) + hn->x*(meshMultiplier))/2.0f;
            y = (h->y*(meshMultiplier) + hn->y*(meshMultiplier))/2.0f;

            j++;
            meshPoints[i][j] = {x,y, DEFAULT_HEIGHT};
        }
    }

}

heightMesh::~heightMesh(){
}


void heightMesh::propogateDeform(Vector2 Origin, Vector2 Direction, float force){
    //get distance to origin 

    // Vector3 Line = getLineEquation(Direction,Origin);

    int originIndex;
    float closest_dist = __FLT_MAX__;
    float temp_dist = 0;    
    float max = 0;


    // printf("ORIGIN %f,%f\n", Origin.x, Origin.y);
    // Get Origin index
    for(int w  = 0; w < this->width; w++){
        temp_dist = Vector2Distance({meshPoints[0][w].x,meshPoints[0][w].y},Origin);

        // printf("%f,%f :: %f\n", meshPoints[0][w].x,meshPoints[0][w].y, temp_dist);
        if(max < temp_dist){
            max = temp_dist;
        }
        if(temp_dist < closest_dist){
            closest_dist = temp_dist;
            originIndex = w;
        }
        
    }


    
    // printf("Move %f,%f\n",Direction.x,Direction.y);
    // printf("Move %f,%f\n",Movement.x,Movement.y);

    for(int i = 0; i < depth; i++){

        for(int j = 0; j < width; j++){

            // float side = crossproduct(
            //     Origin,
            //     {Origin.x + Origin.y,Origin.y - Origin.x},
            //     {meshPoints[i][j].x,meshPoints[i][j].y}
            // );

            float side = Vector2Distance(Origin, {meshPoints[i][j].x,meshPoints[i][j].y});
            Vector2 Movement = Vector2Normalize({meshPoints[i][j].x - Origin.x,meshPoints[i][j].y - Origin.y});

            // printf("Side %f/ %f = %f -> %f\n", side, max, (side/max), (side/max) * Movement.x);
            // printf("%f,%f\n", Movement.x, Movement.y);
            if(side < 0){
                meshPoints[i][j].x -= (side/max) * -Movement.x * 0.5;
                meshPoints[i][j].y -= (side/max) * -Movement.y * 0.5;
            }
            if(side > 0){
                meshPoints[i][j].x -= (side/max) * -Movement.x  * 0.5;
                meshPoints[i][j].y -= (side/max) * -Movement.y  * 0.5;
            }
        }

        
    }

}

void heightMesh::render(Vector2 offset){
    for(int d = 0; d < this->depth; d++){
        for(int w  = 0; w < this->width; w++){

            DrawCircle(meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, 5, ORANGE);
            DrawText(std::to_string(w).c_str(),meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y + 10, 5, GREEN);
            DrawLine(offset.x, offset.y, meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, ORANGE);

            DrawLine(meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, meshPoints[d][(w + 1)%width].x + offset.x, meshPoints[d][(w + 1)%width].y + offset.y, ORANGE);
        }
    }
}

#endif