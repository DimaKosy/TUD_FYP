#include <iostream>
#include <vector>
#include <list>
#include <raylib.h>
#include <rlgl.h>

class plate{
private:
    int x, y;
    std::vector<Vector2> vertexs;
    

public:
    plate(int x, int y);
    ~plate();
    void render();
    void render_verts();
    int get_x(){return x;};
    int get_y(){return y;};
    void add_vertex(int x, int y, int position);
};


plate::plate(int x = 0, int y = 0){
    this->x = x;
    this->y = y;
}

plate::~plate(){

}

void plate::render(){
    DrawCircle(this->x, this->y, 5, (Color){255,0,0,255});
}

void plate::render_verts(){
    int v_size =  vertexs.size();

    
    for(int v = 0; v < v_size; v++){
        DrawLine(vertexs.at(v).x, vertexs.at(v).y, vertexs.at((v+1)%v_size).x, vertexs.at((v+1)%v_size).y,(Color){0,0,255,255});
        DrawCircle(vertexs.at(v).x, vertexs.at(v).y, 3, (Color){255,255,255,150});
    }
    
}

void plate::add_vertex(int x, int y, int position){
    if(position == -1){
        this->vertexs.push_back((Vector2){x,y});
        return;
    }
    
    
    this->vertexs.insert(vertexs.begin(),position,(Vector2){x,y});  
}


class world{
private:
    std::vector<plate> plates;
    int grid_count_x;
    int grid_count_y;

    int square_size_x;
    int square_size_y;

public:
    world(int world_x, int world_y, int grid_count_x, int grid_count_y);
    ~world();

    std::vector<plate> get_plates();
    std::pair<int,int> to_2D_index(int i);
    int to_1D_index(int x, int y);
    void render_plates();
};

world::world(int world_x, int world_y, int grid_count_x, int grid_count_y){
    this->grid_count_x = grid_count_x;
    this->grid_count_y = grid_count_y;
    int grid_size;
    this->square_size_x;
    this->square_size_y;


    //grid count check to avoid 0 or less
    if(grid_count_y <= 0){
        grid_count_y = 1;
    }
    if(grid_count_y <= 0){
        grid_count_y = 1;
    }

    //grid size setting
    grid_size = grid_count_x*grid_count_y;
    square_size_x = world_x / grid_count_x;
    square_size_y = world_y / grid_count_y; 
    
    // creates the plates centers
    for(int i = 0; i < grid_size; i++ ){
        plates.push_back(plate(
            (square_size_x * (i%grid_count_x)) + rand()%square_size_x, 
            (square_size_y * (i/grid_count_x)) + rand()%square_size_y
            ));
    }
}

world::~world(){

}

std::vector<plate> world::get_plates(){
    return this->plates;
}

//Convert to 1D index to 2D
std::pair<int,int> world::to_2D_index(int i){
    int x = i % grid_count_x;
    int y = i / grid_count_x;
    return std::pair(x,y);
}

//Converts 2D index to 1D
int world::to_1D_index(int x, int y){
    //modulo with - wrap
    x = x % grid_count_x;
    x = (grid_count_x + x) % grid_count_x;
    y = y % grid_count_y;
    y = (grid_count_y + y) % grid_count_y;


    return grid_count_x*x + y;
}

void world::render_plates(){
    int plate_size = plates.size();

    int grid_size = grid_count_x*grid_count_y;

    for(int x = 0; x < this->grid_count_x; x++){
        for(int y = 0; y < this->grid_count_y; y++){
            DrawRectangle(x*);
        }
    }

    for(int i = 0; i < plate_size; i++){
        std::pair<int, int> xy = to_2D_index(i);

        plate current = plates.at(i);

        current.render();
        current.render_verts();
    }
}
