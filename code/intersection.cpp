/**
 * Intersection
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Function implementation for Intersection class
 * 
 * Revision History:
 *      14NOV2021  R-11-14: Document Created, initial coding
 *      01DEC2021  R-12-01: Added getters for vector sizes
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "intersection.h"

// Destructor
Intersection::~Intersection()
{
    for (int i=0; i<intersectionLanes.size(); ++i)
    {
        delete intersectionLanes[i];
    }

    for (int i=0; i<intersectionNodes.size(); ++i)
    {
        delete intersectionNodes[i];
    }
}

/**
 * isInIntersection
 * Inputs:
 *      unsigned int - Position of the pod
 *      std::string - Lane identifier
 * Outputs:
 *      bool - True if pod is in the intersection
 * Description:
 *          Checks if the given pod is in the intersection
 *          square
 **/
bool Intersection::isInIntersection(unsigned int pos, std::string laneID)
{
    Lane* theLane = getLane(laneID);
    return theLane->isInIntersection(pos);
}

/**
 * isThisIntersection
 * Inputs:
 *      std::string - Intersection identifier
 * Outputs:
 *      bool - True if identifier matches this intersection
 * Description:
 *          Helps identify if this is the matching intersection
 **/
bool Intersection::isThisIntersection(std::string id)
{
    if (id == intersectionID)
    {
        return true;
    }
    return false;
}

/**
 * getNode
 * Inputs:
 *      std::string - Node identifier
 * Outputs:
 *      Node* - Pointer to a node object
 * Description:
 *          Returns pointer to requested node. Returns NULL if not found.
 **/
Node* Intersection::getNode(std::string node_id)
{
    // Goes through vector of nodes to find the requested node
    for (int i=0; i<intersectionNodes.size(); ++i)
    {
        if (intersectionNodes[i]->nodeID == node_id)
        {
            return intersectionNodes[i];
        }
    }
    return NULL;
}

/**
 * getLane
 * Inputs:
 *      std::string - Lane identifier
 * Outputs:
 *      Lane* - Pointer to a lane object
 * Description:
 *          Returns pointer to requested lane. Returns NULL if not found.
 **/
Lane* Intersection::getLane(std::string lane_id)
{
    // Goes through vector of lanes to find the requested lane
    for (int i=0; i<intersectionLanes.size(); ++i)
    {
        if (intersectionLanes[i]->isThisLane(lane_id))
        {
            return intersectionLanes[i];
        }
    }
    return NULL;
}