#ifndef COLLID_C
#define COLLID_C
#include "TED.hpp"


// ax + by + c = 0
Vector3 getPerpindicularBisector(Vector2 p1, Vector2 p2){
    // getting midpoint
    float mx = (p1.x + p2.x) / 2.0;
    float my = (p1.y + p2.y) / 2.0;

    // slopes
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    
    if (dy == 0) { // Vertical line segment
        return (Vector3){1,0,mx};
    }
    if (dx == 0) { // Horizontal line segment
        return (Vector3){0,1,my};
    }

    float slope = -dx/dy;
    
    return (Vector3){-slope,1,my - slope * mx};
}

Vector3 getPerpLineThroughPoint(Vector3 abcline , Vector2 point){
    // ax + by + c => bx - ay + c, get perpindicular
    Vector3 temp = {
        abcline.y,
        abcline.x,
        abcline.z
    };

    // b(x) + a(y) = -c
    temp.z  = -(temp.x*point.x + temp.y*point.y);


    return temp;
}

Vector3 getLineEquation(Vector2 a, Vector2 b){
    // (b.y-a.y)x - (b.x-a.x)y + ((b.x * a.y)-(a.x*b.y)) = 0
    return (Vector3){
        (b.y-a.y),
        -(b.x-a.x),
        ((b.x * a.y)-(a.x*b.y))
    };
}

Vector2 getIntersector(Vector3 p1, Vector3 p2){

    
    double det = p1.x * p2.y - p2.x * p1.y;

    if (std::abs(det) <= 1e-9) {
        // Lines are parallel
        printf("FAILED INTERSECT\n");
        return (Vector2){NAN, NAN};
    }
    return (Vector2){
        (p2.y * p1.z - p1.y * p2.z) / det,
        (p1.x * p2.z - p2.x * p1.z) / det
    };
}

// (b.x-a.x)(c.y-a.y)-(b.y-a.y)(c.x-a.x) cross product
float crossproduct(Vector2 a, Vector2 b, Vector2 c){
    return ((b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x));
}

#endif