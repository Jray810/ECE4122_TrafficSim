/**
 * Intersection
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Defines base class intersection object that intersection controllers will control.
 *      Contains lane and node objects.
 * 
 * Revision History:
 *      14NOV2021  R-11-14: Document Created, initial coding
 *      01DEC2021  R-12-01: Added getters for vector sizes
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "lane.h"

/**
 * Intersection Class
 * Description:
 *          Class that simulates a basic intersection
 **/
class Intersection
{
public:
    // Constructors
    Intersection(std::string id):intersectionID(id){}

    // Destructors
    ~Intersection();

    // Member Functions
    bool isInIntersection(unsigned int pos, std::string laneID);
    bool isThisIntersection(std::string id);

    // Getters
    std::string getIntersectionID(){return intersectionID;}
    unsigned int getNumNodes(){return intersectionNodes.size();}
    unsigned int getNumLanes(){return intersectionLanes.size();}
    Node* getNode(std::string node_id);
    Lane* getLane(std::string lane_id);

protected:
    std::string intersectionID;             // Unique intersection identifier
    std::vector<Node*> intersectionNodes;   // Vector of all nodes belonging to this intersection
    std::vector<Lane*> intersectionLanes;   // Vector of all lanes belonging to this intersection
};

#endif