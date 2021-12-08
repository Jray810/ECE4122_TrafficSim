/**
 * 4-Way Single Lane Intersection
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Function implementation for Intersect4WSL class
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "intersect4wsl.h"

// Constructor
Intersect4WSL::Intersect4WSL(unsigned int speedLimit)
    :Intersection("4W-SL")
    {
        // Create and initialize nodes
        for (int i=0; i<4; ++i)
        {
            Node* nodePtr = new Node;
            // Add node to node vector
            intersectionNodes.push_back(nodePtr);
            intersectionNodes[i]->intersectionID = "4W-SL";
            intersectionNodes[i]->nodeID = std::to_string(i);
            intersectionNodes[i]->speedLimit = speedLimit;
        }

        // Iterate among nodes to create lanes
        for (int i=0; i<intersectionNodes.size(); ++i)
        {
            for (int j=0; j<intersectionNodes.size(); ++j)
            {
                // Can't make a lane from a node to itself
                if (i==j)
                {
                    continue;
                }

                // Create and initialize new lane
                Lane* thisLane = new Lane(intersectionNodes[i], intersectionNodes[j], DEFAULT_LANE_LENGTH, DEFAULT_INTERSECTION_START, DEFAULT_INTERSECTION_END);
                std::string lane_id = std::to_string(i) + "-" + std::to_string(j);
                // Add lane to lane vector
                intersectionLanes.push_back(thisLane);

                // Set allowed concurrent lanes and lane type
                switch (j-i)
                {
                    // Right Turn
                    case 1:
                    case -3:
                        thisLane->setLaneType(RIGHT);
                        for (int k=0; k<4; ++k)
                        {
                            for (int l=0; l<4; ++l)
                            {
                                if (k != l && k != i && l != j)
                                {
                                    lane_id = std::to_string(k) + "-" + std::to_string(l);
                                    thisLane->addAllowedLane(lane_id);
                                }
                            }
                        }
                        break;
                    // Straight
                    case 2:
                    case -2:
                        thisLane->setLaneType(STRAIGHT);
                        lane_id = std::to_string((i+2)%4) + "-" + std::to_string((j+2)%4);
                        thisLane->addAllowedLane(lane_id);
                        lane_id = std::to_string((i+2)%4) + "-" + std::to_string((j+1)%4);
                        thisLane->addAllowedLane(lane_id);
                        lane_id = std::to_string((i+3)%4) + "-" + std::to_string((j+2)%4);
                        thisLane->addAllowedLane(lane_id);
                        break;
                    // Left Turn
                    case 3:
                    case -1:
                        thisLane->setLaneType(LEFT);
                        lane_id = std::to_string((i+1)%4) + "-" + std::to_string((j+3)%4);
                        thisLane->addAllowedLane(lane_id);
                        // Only add this one if there is sufficient space for simultaneous left turns from facing lanes
                        // lane_id = std::to_string((i+2)%4) + "-" + std::to_string((j+2)%4);
                        // thisLane->addAllowedLane(lane_id);
                        lane_id = std::to_string((i+3)%4) + "-" + std::to_string((j+1)%4);
                        thisLane->addAllowedLane(lane_id);
                        break;
                    default:
                        std::cerr << "Error in intersect4wsl constructor in intersect4wsl.cpp\n";
                        break;
                }
            }
        }
    }