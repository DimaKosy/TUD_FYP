#ifndef TED_H
#define TED_H

int GLOBAL_FLAG = 0;
int GLOBAL_MAX_WIDTH_SPREAD = 3;
int GLOBAL_MAX_DEPTH_SPREAD = 2;

#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <cmath>
#include <string>
#include <unordered_map>
#include <list>
#include <tuple>
#include <vector>
#include <sstream>
#include <algorithm>
#include <time.h>

#include "collisionFunctions.cpp"
#include "heightMesh.cpp"
#include "plate.cpp"
#include "gridCell.cpp"
// #include "infiniteWorld.cpp"
#include "fixedWorld.cpp"



#endif