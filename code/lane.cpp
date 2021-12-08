/**
 * Lane
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Function implementation for lane class
 * 
 * Revision History:
 *      13NOV2021  R-11-13: Document Created, initial coding
 *      30NOV2021  R-11-30: Added allowed lanes functionality
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "lane.h"

// Constructor - Default Version
Lane::Lane(Node* src, Node* dest)
    :source(src), destination(dest), laneLength(DEFAULT_LANE_LENGTH), beginIntersection(DEFAULT_INTERSECTION_START), endIntersection(DEFAULT_INTERSECTION_END)
    {
        laneID = src->nodeID + "-" + dest->nodeID;
    }

// Constructor - Customizable Version
Lane::Lane(Node* src, Node* dest, unsigned int length, unsigned int beginInt, unsigned int endInt)
    :source(src), destination(dest), laneLength(length), beginIntersection(beginInt), endIntersection(endInt)
    {
        laneID = src->nodeID + "-" + dest->nodeID;
    }

/**
 * isInLane
 * Inputs:
 *      double - Linear position of vehicle
 * Outputs:
 *      bool - True if vehicle is still in this lane
 * Description:
 *          Checks if vehicle has reached the end of the lane,
 *          returns false if vehicle is beyond the end of the lane
 **/
bool Lane::isInLane(double pos)
{
    if (pos >= 0 && pos <= laneLength){
        return true;
    }
    return false;
}

/**
 * isInIntersection
 * Inputs:
 *      double - Linear position of vehicle
 * Outputs:
 *      bool - True if vehicle is in intersection square
 * Description:
 *          Checks if vehicle is in intersection square
 **/
bool Lane::isInIntersection(double pos)
{
    if (pos > beginIntersection && pos < endIntersection){
        return true;
    }
    return false;
}

/**
 * isThisLane
 * Inputs:
 *      Node* - Pointer to node that is the source
 *      Node* - Pointer to node that is the destination
 * Outputs:
 *      bool - True if this is the matching lane
 * Description:
 *          Checks if source and destination corresponds
 *          to this lane
 **/
bool Lane::isThisLane(Node* src, Node* dest)
{
    if (src == source && dest == destination){
        return true;
    }
    return false;
}

/**
 * isThisLane
 * Inputs:
 *      std::string - Lane identifier
 * Outputs:
 *      bool - True if this is the matching lane
 * Description:
 *          Checks if lane identifier corresponds
 *          to this lane
 **/
bool Lane::isThisLane(std::string lane_id)
{
    if (laneID == lane_id)
    {
        return true;
    }
    return false;
}

/**
 * isAllowedLane
 * Inputs:
 *      std::string - Lane identifier
 * Outputs:
 *      bool - True if inputted lane is allowed
 *              in the intersection simultaneously
 * Description:
 *          Checks if lane identifier is in vector of allowed lanes
 **/
bool Lane::isAllowedLane(std::string lane_id)
{
    for (int i=0; i<allowedLanes.size(); ++i)
    {
        if (allowedLanes[i] == lane_id)
        {
            return true;
        }
    }
    return false;
}

