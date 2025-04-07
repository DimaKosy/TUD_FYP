#ifndef TEST_C
#define TEST_C
#include "TED.hpp"

void getLineEquation_test(){
    Vector3 R;

    R = getLineEquation(
        {10,0},
        {0,0}
    );

    if(R.x != 0 && R.y != 10 && R.z != 0){
        printf("Fail getLineEquation_test 1\n");
    }


    R = getLineEquation(
        {10,0},
        {10,0}
    );

    if(R.x != 0 && R.y != 0 && R.z != 0){
        printf("Fail getLineEquation_test 2\n");
    }


    R = getLineEquation(
        {6.85,2.54},
        {8,10}
    );

    if(R.x != 7.46f && R.y != -1.15f && R.z != -48.18f){
        printf("Fail getLineEquation_test 3\n");
    }

    printf("Complete getLineEquation\n");
}

void getLineIntersection_test(){
    Vector2 V;
    
    V = getLineIntersector(
        getLineEquation(
            {10,0},
            {0,0}
        ),
        getLineEquation(
            {5,5},
            {5,-5}
        )
    );

    printf("%f,%f\n",V.x,V.y);

    V = getLineIntersector(
        getLineEquation(
            {254.542328,5},
            {0,0}
        ),
        getLineEquation(
            {5,5},
            {5,-5}
        )
    );

    printf("%f,%f\n",V.x,V.y);
}

#endif