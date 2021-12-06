/**
 * 4-Way Single Lane Intersection
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      4-Way Single Lane Intersection
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 * 
 **/

#ifndef INTERSECT4WSL_H
#define INTERSECT4WSL_H

#define DEFAULT_LANE_LENGTH 100
#define DEFAULT_INTERSECTION_START 40
#define DEFAULT_INTERSECTION_END 60

#include "intersection.h"
#include <iostream>

class Intersect4WSL: public Intersection
{
public:
    // Constructors
    Intersect4WSL(unsigned int speedLimit):Intersection("4W-SL")
    {
        for (int i=0; i<4; ++i)
        {
            Node* nodePtr = new Node;
            intersectionNodes.push_back(nodePtr);
            intersectionNodes[i]->intersectionID = "4W-SL";
            intersectionNodes[i]->nodeID = std::to_string(i);
            intersectionNodes[i]->speedLimit = speedLimit;
        }

        for (int i=0; i<intersectionNodes.size(); ++i)
        {
            for (int j=0; j<intersectionNodes.size(); ++j)
            {
                if (i==j)
                {
                    continue;
                }
                Lane* thisLane = new Lane(intersectionNodes[i], intersectionNodes[j], DEFAULT_LANE_LENGTH, DEFAULT_INTERSECTION_START, DEFAULT_INTERSECTION_END);
                std::string lane_id = std::to_string(i) + "-" + std::to_string(j);
                thisLane->setLaneID(lane_id);
                intersectionLanes.push_back(thisLane);

                // Set allowed concurrent lanes
                switch (j-i)
                {
                    // Right Turn
                    case 1:
                    case -3:
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
                        lane_id = std::to_string((i+1)%4) + "-" + std::to_string((j+3)%4);
                        thisLane->addAllowedLane(lane_id);
                        lane_id = std::to_string((i+2)%4) + "-" + std::to_string((j+2)%4);
                        thisLane->addAllowedLane(lane_id);
                        lane_id = std::to_string((i+3)%4) + "-" + std::to_string((j+1)%4);
                        thisLane->addAllowedLane(lane_id);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    // Destructors
    ~Intersect4WSL()
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

    // Member Functions

    // Getters

    // Setters
};

#endif