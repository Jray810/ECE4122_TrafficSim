/**
 * Intersection
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Defines base class intersection object that intersection controllers will control.
 *      Contain lane and node objects.
 * 
 * Revision History:
 *      14NOV2021  R-11-14: Document Created, initial coding
 *      01DEC2021  R-12-01: Added getters for vector sizes
 * 
 **/

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "lane.h"
#include <vector>

class Intersection
{
public:
    // Constructors
    Intersection(std::string id):intersectionID(id){}

    // Member Functions
    bool isInIntersection(unsigned int pos, std::string laneID)
    {
        Lane* theLane = getLane(laneID);
        return theLane->isInIntersection(pos);
    }

    bool isThisIntersection(std::string id)
    {
        if (id == intersectionID)
        {
            return true;
        }
        return false;
    }

    // Getters
    std::string getIntersectionID(){return intersectionID;}

    Node* getNode(std::string nodeID)
    {
        for (int i=0; i<intersectionNodes.size(); ++i)
        {
            if (intersectionNodes[i]->nodeID == nodeID)
            {
                return intersectionNodes[i];
            }
        }
        return NULL;
    }

    Lane* getLane(std::string laneID)
    {
        for (int i=0; i<intersectionLanes.size(); ++i)
        {
            if (intersectionLanes[i]->isThisLane(laneID))
            {
                return intersectionLanes[i];
            }
        }
        return NULL;
    }

    unsigned int getNumNodes()
    {
        return intersectionNodes.size();
    }

    unsigned int getNumLanes()
    {
        return intersectionLanes.size();
    }

protected:
    std::string intersectionID;
    std::vector<Node*> intersectionNodes;
    std::vector<Lane*> intersectionLanes;
};

#endif