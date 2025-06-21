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

Vector3 getLineEquation(Vector2 A, Vector2 B){
    // (b.y-a.y)x - (b.x-a.x)y + ((b.x * a.y)-(a.x*b.y)) = 0
    // printf("LE (%f,%f),(%f,%f)\n",A.x,A.y,B.x,B.y);
    
    float a = (B.y-A.y);
    float b = -(B.x-A.x);
    float c = ((B.x * A.y)-(A.x*B.y));
    // printf("\n%f,%f,%f\n",a,b,c);
    return (Vector3){
        a,
        b,
        c
    };
}

// used for generating Voronois
Vector2 getIntersector(Vector3 p1, Vector3 p2){

    
    float det = p1.x * p2.y - p2.x * p1.y;

    if (std::abs(det) <= 1e-9) {
        // Lines are parallel
        // printf("FAILED INTERSECT\n");
        return (Vector2){NAN, NAN};
    }
    return (Vector2){
        ((p2.y * p1.z - p1.y * p2.z) / det),
        ((p1.x * p2.z - p2.x * p1.z) / det)
    };
}

// for general line intersections
Vector2 getLineIntersector(Vector3 A, Vector3 B){
    
    float det = A.x * B.y - B.x * A.y;

    if (std::abs(det) <= 1e-9) {
        // Lines are parallel
        // printf("FAILED INTERSECT\n");
        return (Vector2){NAN, NAN};
    }
    return (Vector2){
        (A.y * B.z - B.y * A.z) / det,
        (B.x * A.z - A.x * B.z) / det
    };
}

// get the angle between 3 points
float angleBetween(Vector2 A, Vector2 B, Vector2 C){
    Vector2 u = {A.x - B.x, A.y - B.y};
    Vector2 v = {C.x - B.x, C.y - B.y};

    float numerator = u.x * v.x + u.y * v.y;
    float denU = std::sqrt(u.x * u.x + u.y * u.y);
    float denV = std::sqrt(v.x * v.x + v.y * v.y);

    return std::acos(numerator / (denU * denV));//returns angle
}

// (b.x-a.x)(c.y-a.y)-(b.y-a.y)(c.x-a.x) cross product
float crossproduct(Vector2 a, Vector2 b, Vector2 c){
    return ((b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x));
}

// get the weights of a point inside a triangle
Vector2 pointWeightInTriangle(Vector2 A,Vector2 B, Vector2 C, Vector2 P){
        double s1 = C.y - A.y;
        double s2 = C.x - A.x;
        double s3 = B.y - A.y;
        double s4 = P.y - A.y;

        double w1 = (A.x * s1 + s4 * s2 - P.x * s1) / (s3 * s2 - (B.x-A.x) * s1);
        double w2 = (s4- w1 * s3) / s1;
        return {w1,w2};
}

int mod(int value, int modulo){
    return (modulo + (value % modulo)) % modulo;
}

// float Vector2Distance(Vector2 A, Vector2 B){
//     return sqrt(pow(B.x - A.x,2) + pow(B.y - A.y,2));
// }
#endif