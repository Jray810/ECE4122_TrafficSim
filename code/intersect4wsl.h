/**
 * 4-Way Single Lane Intersection
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      A 4-Way Single Lane Intersection object derived from Intersection class
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#ifndef INTERSECT4WSL_H
#define INTERSECT4WSL_H

#define DEFAULT_LANE_LENGTH 50
#define DEFAULT_INTERSECTION_START 20
#define DEFAULT_INTERSECTION_END 30

#include "intersection.h"

class Intersect4WSL: public Intersection
{
public:
    // Constructors
    Intersect4WSL(unsigned int speedLimit);
};

#endif