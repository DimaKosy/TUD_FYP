#include <iostream>


std::pair<int,int> to_2D_index(int i, int indexCount_x = 1, int indexCount_y = 1){
    int x = i % indexCount_x;
    int y = i / indexCount_x;
    return std::pair(x,y);
}

int to_1D_index(int x, int y, int indexCount_x, int indexCount_y){
    //modulo with - wrap
    x = x % indexCount_x;
    x = (indexCount_x + x) % indexCount_x;
    printf("%d\n",x);
    y = y % indexCount_y;
    y = (indexCount_y + y) % indexCount_y;
    printf("%d\n",y);


    return indexCount_x*y + x;
}

int main(int argc, char ** arg){

    int x =5;
    int y =4;

    for(int i = 0; i < x*y; i++){
        std::pair<int,int> coords = to_2D_index(i,x,y);
        printf("%d: (%d:%d) :%d\n",i,coords.first,coords.second,to_1D_index(coords.first, coords.second,x,y));
    }


}