/**
 * Lane
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Defines node and lane object that vehicles will select and follow in transit.
 *      Intersections will create and store lane objects.
 * 
 * Revision History:
 *      13NOV2021  R-11-13: Document Created, initial coding
 *      30NOV2021  R-11-30: Added allowed lanes functionality
 * 
 **/

#ifndef LANE_H
#define LANE_H

#include <string>
#include <vector>

struct Node
{
    std::string intersectionID;
    std::string nodeID;
    unsigned int speedLimit;
};

class Lane
{
public:
    // Constructors
    Lane(Node* src, Node* dest):
        source(src),destination(dest),laneLength(100),beginIntersection(40),endIntersection(60)
        {
            laneID = src->nodeID + "_" + dest->nodeID;
        }

    Lane(Node* src, Node* dest, unsigned int length, unsigned int beginInt, unsigned int endInt):
        source(src),destination(dest),laneLength(length),beginIntersection(beginInt),endIntersection(endInt)
        {
            laneID = src->nodeID + "_" + dest->nodeID;
        }

    // Member Functions
    bool isInLane(double pos)
    {
        if (pos > 0 && pos <= laneLength){
            return true;
        }
        return false;
    }

    bool isInIntersection(double pos)
    {
        if (pos >= beginIntersection && pos <= endIntersection){
            return true;
        }
        return false;
    }

    bool isThisLane(Node* src, Node* dest)
    {
        if (src == source && dest == destination){
            return true;
        }
        return false;
    }

    bool isThisLane(std::string id)
    {
        if (laneID == id)
        {
            return true;
        }
        return false;
    }

    bool isAllowedLane(std::string id)
    {
        for (int i=0; i<allowedLanes.size(); ++i)
        {
            if (allowedLanes[i] == id)
            {
                return true;
            }
        }
        return false;
    }

    // Getters
    std::string getLaneID(){return laneID;}
    Node* getSource(){return source;}
    Node* getDestination(){return destination;}
    unsigned int getLaneLength(){return laneLength;}
    unsigned int getBeginIntersection(){return beginIntersection;}
    unsigned int getEndIntersection(){return endIntersection;}

    // Setters
    void setLaneID(std::string lane_id){laneID = lane_id;}
    void addAllowedLane(std::string lane_id){allowedLanes.push_back(lane_id);}

private:
    std::string laneID;
    Node* source;
    Node* destination;
    unsigned int laneLength;
    unsigned int beginIntersection;
    unsigned int endIntersection;
    std::vector<std::string> allowedLanes;
};

#endif