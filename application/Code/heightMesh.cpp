#ifndef HEIGHT_MESH_C
#define HEIGHT_MESH_C

#include "TED.hpp"



class heightMesh{
private:
    int depth;
    int width;
    Vector3 ** meshPoints;
    Vector3 centerPoint;
    //owner_id, platePosition, source, force
    std::list<std::tuple<void *,Vector2 ,Vector2, float>> ForceQueue;
    // {key, {plate position, minimum vector, maximum vector, force}}
    std::unordered_map<void *, std::tuple<Vector2, Vector2, Vector2, float>> epicenter;
    //position force
    std::list<std::pair<Vector2, float>> sourceList;

    std::list<Vector2> test;
public:
    int ran = 0;
    heightMesh();
    ~heightMesh();

    int getDepth();
    int getWidth();

    void rebuildHullfromMesh(std::list<Vector2> & hull);

    void initMesh(int depth, std::list<Vector2>);
    void queueForces(void * owner_id, Vector2 platePosition, Vector2 source, float force);
    void processForceQueue();
    void Reshape(heightMesh);
    void freeMeshPoints();
    void render(Vector2 offset);
};

heightMesh::heightMesh(){
}

int heightMesh::getDepth(){
    return this->depth;
}

int heightMesh::getWidth(){
    return this->width;
}

void heightMesh::rebuildHullfromMesh(std::list<Vector2> & hull){

    for(int w = 0; w < this->width -1; w+=2){

        hull.push_back({meshPoints[0][w].x,meshPoints[0][w].y});
    }

}

void heightMesh::initMesh(int depth, std::list<Vector2> hull){
    centerPoint = {0,0,DEFAULT_HEIGHT};

    meshPoints = new Vector3 * [depth];
    this->width = hull.size() * 2;
    this->depth = depth;

    // printf("D %d, W %d\n",this->depth, this->width);

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

            // creates a point between the two vertices
            x = (h->x*(meshMultiplier) + hn->x*(meshMultiplier))/2.0f;
            y = (h->y*(meshMultiplier) + hn->y*(meshMultiplier))/2.0f;

            j++;
            if(j == width){ //safeguard
                break;
            }
            meshPoints[i][j] = {x,y, DEFAULT_HEIGHT};
        }
    }
}

void heightMesh::queueForces(void * owner_id, Vector2 platePosition, Vector2 source, float force){
    ForceQueue.push_back({owner_id, platePosition, source, force});
}

void heightMesh::processForceQueue(){    
    // printf("Process\n");
    //narrows down the epicenter of the influencing force
    for(auto fp : ForceQueue){

        if(epicenter.find(std::get<0>(fp)) != epicenter.end()){ //checks if the key already exists
            Vector2 source = std::get<2>(fp);
            Vector2 min_vector = std::get<1>(epicenter[std::get<0>(fp)]);
            Vector2 max_vector = std::get<2>(epicenter[std::get<0>(fp)]);

            //gets distance from line between the mesh center and the interaction axis
            float distance = crossproduct({centerPoint.x, centerPoint.y}, std::get<1>(fp) , source);

            //get current min and max distances
            float min_distance = crossproduct({centerPoint.x, centerPoint.y}, std::get<1>(fp) , min_vector);
            float max_distance = crossproduct({centerPoint.x, centerPoint.y}, std::get<1>(fp) , max_vector);

            if(distance < min_distance){ //set min distance
                std::get<1>(epicenter[std::get<0>(fp)]) = std::get<2>(fp);
            }

            if(distance > max_distance){ //set max distance
                std::get<2>(epicenter[std::get<0>(fp)]) = std::get<2>(fp);
            }

        }
        else{ //else adds it
            epicenter.insert({
                std::get<0>(fp), // id
                {
                    std::get<1>(fp), // plate position
                    std::get<2>(fp), // minimum vector
                    std::get<2>(fp), // maximum vector
                    std::get<3>(fp)  // force
                }
            });
        }
        
    }

    // turns it from an epicenter list to source list by finding where the perpindicular bisector intersects with the mesh
    for(auto& [key, value] : epicenter){
        Vector2 platePos = std::get<0>(value);
        Vector2 min_vector = std::get<1>(value);
        Vector2 max_vector = std::get<2>(value);

        // get perpindicular bisector between the minimum and maximum vectors
        Vector3 perpBisector = getPerpindicularBisector(min_vector, max_vector);

        //finding where the perpindicular bisector intersects the mesh edge
        for(int this_w = 0; this_w < this->width; this_w++){

            Vector3 meshEdge = getLineEquation(
                {this->meshPoints[0][this_w].x, this->meshPoints[0][this_w].y},
                {this->meshPoints[0][(this_w + 1) % this->width].x, this->meshPoints[0][(this_w + 1) % this->width].y}
            );

            Vector2 intersection = getLineIntersector(perpBisector, meshEdge);

            if(std::isnan(intersection.x)){
                // /*DEBUG*/printf("NAN INTERSECT");
                continue;
            }
            
            // use a scaled min and max vectors to reduce edge cases when intersection is on the line
            float cross_edge = crossproduct(Vector2Multiply(min_vector, {0.9,0.9}), Vector2Multiply(max_vector, {0.9,0.9}), intersection);
            float direction_edge = crossproduct(min_vector, max_vector, platePos);

            if(
                intersection.x >= std::min(this->meshPoints[0][this_w].x, this->meshPoints[0][(this_w + 1) % this->width].x) - 1 && intersection.x <= std::max(this->meshPoints[0][this_w].x, this->meshPoints[0][(this_w + 1) % this->width].x) + 1 &&
                intersection.y >= std::min(this->meshPoints[0][this_w].y, this->meshPoints[0][(this_w + 1) % this->width].y) - 1 && intersection.y <= std::max(this->meshPoints[0][this_w].y, this->meshPoints[0][(this_w + 1) % this->width].y) + 1 &&
                std::signbit(cross_edge) == std::signbit(direction_edge)
            ){
                //add to source list
                
                sourceList.push_back({intersection, std::get<3>(value)});
                break;
            }
        }
    }


    for(auto source : sourceList){
        for(int this_w = 0; this_w < this->width; this_w++){
            // distance based approach
            // float distance = 1.0f *Vector2Distance({centerPoint.x, centerPoint.y},source.first);


            // for(int  this_d = 0; this_d < this->depth; this_d++){
            //     float vectorDistance = Vector2Distance({this->meshPoints[this_d][this_w].x, this->meshPoints[this_d][this_w].y},source.first);

            //     float multiplier = std::max(0.0f,(distance - vectorDistance)/distance);

            //     this->meshPoints[this_d][this_w].z += source.second * pow(multiplier,2);

            // }
            

            int left_index = this_w % this->width;
            int right_index = (this_w + 1) % this->width;

            float left_cross = crossproduct(
                {0,0},
                {this->meshPoints[0][left_index].x, this->meshPoints[0][left_index].y},
                source.first
            );
            float right_cross = crossproduct(
                {0,0},
                {this->meshPoints[0][right_index].x, this->meshPoints[0][right_index].y},
                source.first
            );

            // skips if its not within the current two vertex lines
            if(left_cross < 0){
                continue;
            }
            if(right_cross >= 0){
                continue;
            }
            

            float d1, d2, total_dist;

            d1 = Vector2Distance({this->meshPoints[0][left_index].x, this->meshPoints[0][left_index].y} ,source.first);
            d2 = Vector2Distance({this->meshPoints[0][right_index].x, this->meshPoints[0][right_index].y},source.first);

            total_dist = d1 + d2;

            // adds the changes to the height
            this->meshPoints[0][left_index].z += source.second * ((total_dist - d1 * 0.5f)/total_dist);
            this->meshPoints[0][right_index].z += source.second * ((total_dist - d2 * 0.5f)/total_dist);

            //spreads it past initial points

            

            // using spread values
            for(int w = 0; w <= GLOBAL_MAX_WIDTH_SPREAD; w++){
                for(int d = 0; d <= GLOBAL_MAX_DEPTH_SPREAD - w; d++){

                    // float multiplier = ((GLOBAL_MAX_WIDTH_SPREAD + GLOBAL_MAX_DEPTH_SPREAD) == 0 ? 1 : (GLOBAL_MAX_WIDTH_SPREAD + GLOBAL_MAX_DEPTH_SPREAD - w - d)/(GLOBAL_MAX_WIDTH_SPREAD + GLOBAL_MAX_DEPTH_SPREAD));

                    float multiplier = 1;

                    this->meshPoints[std::min(d, this->depth-1)][mod(left_index - w, this->width)].z += source.second * ((total_dist - d1 * 0.5f)/total_dist) * multiplier;
                    this->meshPoints[std::min(d, this->depth-1)][mod(right_index + w, this->width)].z += source.second * ((total_dist - d2 * 0.5f)/total_dist) * multiplier;
                }
            }

            test.push_back({this->meshPoints[0][left_index].x, this->meshPoints[0][left_index].y});
            test.push_back({this->meshPoints[0][right_index].x, this->meshPoints[0][right_index].y});
        }
    }

    ForceQueue.clear(); //empty forceQueue
}

void heightMesh::freeMeshPoints() {
    //for some reason freeing the points breaks it? seems to be safe to just drop the points
    
    // for(int i = 0; i < depth; i++){
    //     delete[] meshPoints[i];
    // }

    // delete[] meshPoints;
}

//sets the height of the mesh by comparing the points position with the previous iterations mesh and getting a gradient value
void heightMesh::Reshape(heightMesh other){
    // printf("Reshape\n");
    float height = 0;
    

    for(int this_w = 0, reshape_start = 0; this_w < this->width; this_w++){
        // loop through the points of the other mesh, 
        //adjusting the reshape_start so as to reduce the number of checks required 
        //as we can assume the next vertice will be either be between the same points as the current on or the next set of points
        for(int other_w = reshape_start; other_w < other.width + reshape_start; other_w++, reshape_start++){
            Vector2 current_vector  = {this->meshPoints[0][this_w].x, this->meshPoints[0][this_w].y};

            //indexs for determining the current vectors
            int left_index = other_w % other.width;
            int right_index = (other_w + 1) % other.width;

            //checks if its left or right of the vertex line
            float left_cross = crossproduct(
                {0,0},
                {other.meshPoints[0][left_index].x, other.meshPoints[0][left_index].y},
                current_vector
            );
            float right_cross = crossproduct(
                {0,0},
                {other.meshPoints[0][right_index].x, other.meshPoints[0][right_index].y},
                current_vector
            );
            
            //skips if its not within the current two vertex lines
            if(left_cross < 0){
                continue;
            }
            if(right_cross >= 0){
                continue;
            }
            
            
            //loops through all the vectors on the line and sets their height based on closest vertices
            for(int this_d = 0; this_d < this->depth; this_d++){
                current_vector  = {this->meshPoints[this_d][this_w].x, this->meshPoints[this_d][this_w].y};
                // /* DEBUG */printf("\n(%f,%f)\n",current_vector.x, current_vector.y);

                for(int other_d = 0; other_d < other.depth; other_d++){
                    float d1, d2, d3, d4 = 0;
                    // get the four vertices its inside, or two points if its outside the mesh
                    float outer_cross = crossproduct(
                        {other.meshPoints[other_d][left_index].x, other.meshPoints[other_d][left_index].y},
                        {other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y},
                        current_vector
                    );

                    d1 = Vector2Distance(current_vector, {other.meshPoints[other_d][left_index].x, other.meshPoints[other_d][left_index].y});
                    d2 = Vector2Distance(current_vector, {other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y});

                    // if its outside the shape it assigns using only the outermost vertices and breaks
                    if(outer_cross < 0){
                        //sets the height to be in between the vertices scaled by distance
                        height =(other.meshPoints[other_d][left_index].z *  (((d1 + d2) - d1) / (d1 + d2)) +
                                 other.meshPoints[other_d][right_index].z * (((d1 + d2) - d2) / (d1 + d2))
                                );
                        
                                // printf("H: %f\n",height);

                        this->meshPoints[this_d][this_w].z = height;
                        // /* DEBUG */printf("height [0] %f\n",height);
                        // /* DEBUG */printf("A: %f, B: %f\n",other.meshPoints[other_d][left_index].z, other.meshPoints[other_d][right_index].z);
                        // /* DEBUG */printf("%f, %f\n",(((d1 + d2) - d1) / (d1 + d2)),(((d1 + d2) - d2) / (d1 + d2)));
                        // /* DEBUG */printf("%f, %f\n",other.meshPoints[other_d][left_index].z, other.meshPoints[other_d][right_index].z);
                        // /* DEBUG */printf("(%f,%f),(%f,%f)\n",
                        // /* DEBUG */    other.meshPoints[other_d][left_index].x, other.meshPoints[other_d][left_index].y,
                        // /* DEBUG */    other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y
                        // /* DEBUG */);

                        break; // exit and move onto the next vertex
                    }

                    // if it reaches the end of the depth it uses the center point
                    if(other_d + 1 == other.depth){
                        d3 = Vector2Distance(current_vector, {other.centerPoint.x, other.centerPoint.y});

                        Vector2 weight = pointWeightInTriangle(
                            {other.meshPoints[other_d][left_index].x, other.meshPoints[other_d][left_index].y},
                            {other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y},
                            {other.centerPoint.x, other.centerPoint.y},
                            current_vector
                        );

                        height = (
                        other.meshPoints[other_d][left_index].z * (1 - (weight.x + weight.y)) +
                        other.meshPoints[other_d][right_index].z * weight.x +
                        other.centerPoint.z * weight.y
                        );
                        
                        this->meshPoints[this_d][this_w].z = height ;
                        // /* DEBUG */printf("height [1] %f\n",height);
                        // /* DEBUG */printf("A: %f, B: %f, C: %f\n",other.meshPoints[other_d][left_index].z, other.meshPoints[other_d][right_index].z,centerPoint.z);
                        // /* DEBUG */printf("(%f,%f),(%f,%f),(%f,%f)\n",
                        // /* DEBUG */    other.meshPoints[other_d][left_index].x, other.meshPoints[other_d][left_index].y,
                        // /* DEBUG */    other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y,
                        // /* DEBUG */    other.centerPoint.x, other.centerPoint.y
                        // /* DEBUG */);
                        
                        break;
                    }


                    //First triangle
                    Vector2 weight_1 = pointWeightInTriangle(
                        {other.meshPoints[other_d][left_index].x, other.meshPoints[other_d][left_index].y},
                        {other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y},
                        {other.meshPoints[other_d + 1][left_index].x,  other.meshPoints[other_d + 1][left_index].y},
                        current_vector
                    );

                    Vector2 weight_2 = pointWeightInTriangle(
                        {other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y},
                        {other.meshPoints[other_d + 1][left_index].x,  other.meshPoints[other_d + 1][left_index].y},
                        {other.meshPoints[other_d + 1][right_index].x,  other.meshPoints[other_d + 1][right_index].y},
                        current_vector
                    );

                    
                    
                    if(weight_1.x + weight_1.y <= 1 && weight_1.x >= 0 && weight_1.y >= 0){
                        height = other.meshPoints[other_d][left_index].z * (1 - (weight_1.x + weight_1.y)) +
                        other.meshPoints[other_d][right_index].z * weight_1.x +
                        other.meshPoints[other_d + 1][left_index].z * weight_1.y;

                        this->meshPoints[this_d][this_w].z = height;
                        // /* DEBUG */printf("height [2] %f\n",height);

                        // /* DEBUG */printf("(%f,%f),(%f,%f),(%f,%f)\n",
                        // /* DEBUG */    other.meshPoints[other_d][left_index].x, other.meshPoints[other_d][left_index].y,
                        // /* DEBUG */    other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y,
                        // /* DEBUG */    other.meshPoints[other_d + 1][left_index].x,other.meshPoints[other_d + 1][left_index].y
                        // /* DEBUG */);

                        break;
                    }
                    if(weight_2.x + weight_2.y <= 1 && weight_2.x >= 0 && weight_2.y >= 0){
                        height = other.meshPoints[other_d][right_index].z * (1 - (weight_2.x + weight_2.y)) +
                        other.meshPoints[other_d + 1][left_index].z * weight_2.x +
                        other.meshPoints[other_d + 1][right_index].z * weight_2.y;

                        // /* DEBUG */printf("height [3] %f\n",height);
                        this->meshPoints[this_d][this_w].z = height;

                        // /* DEBUG */printf("(%f,%f),(%f,%f),(%f,%f)\n",
                        // /* DEBUG */    other.meshPoints[other_d][right_index].x, other.meshPoints[other_d][right_index].y,
                        // /* DEBUG */    other.meshPoints[other_d + 1][left_index].x,other.meshPoints[other_d + 1][left_index].y,
                        // /* DEBUG */    other.meshPoints[other_d + 1][right_index].x,other.meshPoints[other_d + 1][right_index].y
                        // /* DEBUG */);
                        break;
                    }

                    
                    
                    // printf("%f, %f\t%f, %f\n",weight_1.x, weight_1.y, weight_2.x, weight_2.y);
                    // /* DEBUG */printf("Outside of test area\n");
                }

                
                this->meshPoints[this_d][this_w].z *= 0.95f; //lowers overall height
                this->meshPoints[this_d][this_w].z = Clamp(this->meshPoints[this_d][this_w].z,1,254);
            }
            
            break;
        }
        // printf("\n");
        
    }
    // printf("END\n");

    height = 0;
    for(int other_w = 0; other_w < other.width; other_w++){
        height += other.meshPoints[other.depth-1][other_w].z;
    }

    height /= other.width;
    this->centerPoint.z = height;
}


heightMesh::~heightMesh(){
    freeMeshPoints();
}

// void heightMesh::

void heightMesh::render(Vector2 offset){
    // printf("Render\n");
    for(int d = 0; d < this->depth; d++){
        for(int w  = 0; w < this->width; w++){

            float next_x = (d + 1 == this->depth?0: meshPoints[d + 1][w].x) + offset.x;
            float next_y = (d + 1 == this->depth?0: meshPoints[d + 1][w].y) + offset.y;
            float next_x_angled = (d + 1 == this->depth?0: meshPoints[d + 1][(w + 1) % this->width].x) + offset.x;
            float next_y_angled = (d + 1 == this->depth?0: meshPoints[d + 1][(w + 1) % this->width].y) + offset.y;


            rlDrawRenderBatchActive(); // Force draw previous batch
            
            rlBegin(RL_QUADS);
            // rlBegin(RL_TRIANGLES);

            // Vertex 1
            int col1 = meshPoints[d][w].z;
            rlColor4ub(col1, col1, col1, 255);   // Red
            rlVertex2f(meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y);

            // Vertex 2

            int col2 = (d + 1 == this->depth?centerPoint.z:meshPoints[d + 1][w].z);
            rlColor4ub(col2, col2, col2, 255);   // Green
            rlVertex2f(next_x, next_y);

            // Vertex 3
            int col3 = (d + 1 == this->depth?centerPoint.z:meshPoints[d + 1][(w + 1) % this->width].z);
            rlColor4ub(col3, col3, col3, 255);   // Blue
            rlVertex2f(next_x_angled, next_y_angled);

            //Vertex 4
            int col4 = meshPoints[d][(w + 1)%this->width].z;
            rlColor4ub(col4, col4, col4, 255);             
            // rlColor4ub(255, 255, 0, 255);             
            rlVertex2f(meshPoints[d][(w + 1)%this->width].x + offset.x, meshPoints[d][(w + 1)%this->width].y + offset.y);

            rlEnd();
            
            // DrawCircle(meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, 0, {0,0,0,0});

            //towards center
            // DrawLine(next_x, next_y, meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, ORANGE);

            //around
            // DrawLine(meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, meshPoints[d][(w + 1)%width].x + offset.x, meshPoints[d][(w + 1)%width].y + offset.y, ORANGE);

            //angled next
            // DrawLine(next_x_angled, next_y_angled, meshPoints[d][w].x + offset.x, meshPoints[d][w].y + offset.y, ORANGE);
        }
    }

    // for(auto fp: ForceQueue){
    //     DrawLine(std::get<1>(fp).x + offset.x, std::get<1>(fp).y + offset.y, centerPoint.x + offset.x, centerPoint.y + offset.y, RED);
    // }

    // for(auto fp: epicenter){
    //     DrawCircle(std::get<1>(fp.second).x + offset.x, std::get<1>(fp.second).y + offset.y, 10, GREEN);
    //     DrawCircle(std::get<2>(fp.second).x + offset.x, std::get<2>(fp.second).y + offset.y, 10, RED);
    // }
    epicenter.clear();

    // for(auto fp: sourceList){
    //     DrawCircle(fp.first.x + offset.x, fp.first.y + offset.y, 10, ORANGE);
    // }
    sourceList.clear(); //empty sourceList

    // for(auto fp: test){
    //     DrawCircle(fp.x + offset.x, fp.y + offset.y, 5, PURPLE);
    // }
    test.clear();
}


#endif