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
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#ifndef LANE_H
#define LANE_H

#include "debugSetup.h"

#define RIGHT    0
#define STRAIGHT 1
#define LEFT     2

#define DEFAULT_LANE_LENGTH 50
#define DEFAULT_INTERSECTION_START 20
#define DEFAULT_INTERSECTION_END 30

/**
 * Node Struct
 * Description:
 *          Data structure that serves as the starting
 *          and end points of vehicles and lanes
 * Contains:
 *      std::string intersectionID - Unique identifier of intersection it belongs to
 *      std::string nodeID - Unique identifier of the node itself
 *      unsigned int speedLimit - The speed limit of the road this node signifies
 **/
struct Node
{
    std::string intersectionID;
    std::string nodeID;
    unsigned int speedLimit;
};

/**
 * Lane Class
 * Description:
 *          Class that simulates a path between two nodes
 **/
class Lane
{
public:
    // Constructors
    Lane(Node* src, Node* dest);
    Lane(Node* src, Node* dest, unsigned int length, unsigned int beginInt, unsigned int endInt);

    // Member Functions
    bool isInLane(double pos);
    bool isInIntersection(double pos);
    bool isThisLane(Node* src, Node* dest);
    bool isThisLane(std::string lane_id);
    bool isAllowedLane(std::string lane_id);
    void addAllowedLane(std::string lane_id){allowedLanes.push_back(lane_id);}

    // Getters
    std::string getLaneID(){return laneID;}
    Node* getSource(){return source;}
    Node* getDestination(){return destination;}
    unsigned int getLaneLength(){return laneLength;}
    unsigned int getBeginIntersection(){return beginIntersection;}
    unsigned int getEndIntersection(){return endIntersection;}
    int getLaneType(){return laneType;}

    // Setters
    void setLaneType(int type){laneType = type;}

private:
    std::string laneID;                     // Unique lane identifier
    Node* source;                           // Pointer to source node
    Node* destination;                      // Pointer to destination node
    unsigned int laneLength;                // Full length of lane
    unsigned int beginIntersection;         // Length from start of lane to intersection
    unsigned int endIntersection;           // Length from start of lane to end of intersection
    std::vector<std::string> allowedLanes;  // Vector of lane_id's that are allowed in intersection simultaneously
    int laneType;                           // Lane type identifier
};

#endif