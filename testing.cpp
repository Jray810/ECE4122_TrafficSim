#include <iostream>
#include <thread>
#include <chrono>
#include "intersect4wsl.h"
#include "autoTrafficController.h"
#include "lightTrafficController.h"

#define DEFAULT_SPEED_LIMIT 1

#define TEST_INTERSECTION 0
#define TEST_ADDVEHICLES 1

#define AUTO    0
#define LIGHT   1
#define STOP    2

unsigned int controllerType;

using namespace std;

int main(int argc, char *argv[])
{
    // Command Line Argument Detection
    if (argc == 1)
    {
        controllerType = 0;
    }
    if (argc == 2)
    {
        string typeArg = argv[1];
        char c = typeArg[1];
        int i_c = c - 'A';
        switch (i_c)
        {
            case 'A'-'A':
                controllerType = AUTO;
                break;
            case 'L'-'A':
                controllerType = LIGHT;
                break;
            case 'S'-'A':
                controllerType = STOP;
                break;
            default:
                cerr << "Something went wrong\n";
                return 1;
                break;
        }
    }

    // Setup testing environment
    Intersect4WSL* theIntersection = new Intersect4WSL(DEFAULT_SPEED_LIMIT);
    TrafficController* theTrafficController;
    switch (controllerType)
    {
        case 0:
            theTrafficController = new AutoTrafficController(theIntersection);
            break;
        case 1:
            theTrafficController = new LightTrafficController(theIntersection);
            break;
        case 2:
        default:
            theTrafficController = new AutoTrafficController(theIntersection);
            break;
    }
    theTrafficController->startController();
    if (controllerType == LIGHT)
    {
        LightTrafficController* control = dynamic_cast<LightTrafficController*>(theTrafficController);
        control->startLightCycle();
    }

    // Intersection Setup Testing
    if (TEST_INTERSECTION)
    {
        cout << "Testing Intersection Setup\n";
        cout << theIntersection->getIntersectionID() << endl;
        string lane_id;
        for (int i=0; i<4; ++i)
        {
            for (int j=0; j<4; ++j)
            {
                if (i==j) {continue;}

                lane_id = to_string(i) + "-" + to_string(j);
                cout << "Lane: " << lane_id << endl;

                Lane* thisLane = theIntersection->getLane(lane_id);

                string testLane;
                for (int k=0; k<4; ++k)
                {
                    for (int l=0; l<4; ++l)
                    {
                        testLane = to_string(k) + "-" + to_string(l);
                        if (thisLane->isAllowedLane(testLane))
                        {
                            cout << testLane << endl;
                        }
                    }
                }
            }
        }
    }

    // Test adding vehicles
    if (TEST_ADDVEHICLES)
    {
        cout << "Testing Adding Vehicles\n";
        Node* srcNode = theIntersection->getNode("0");
        Node* destNode = theIntersection->getNode("3");
        Vehicle* testVehicle = new Vehicle("bobby", 10, 10, 1, srcNode, destNode);
        theTrafficController->entryQueue.push(testVehicle);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        Vehicle* testVehicleB = new Vehicle("fred", 10, 10, 1, srcNode, destNode);
        theTrafficController->entryQueue.push(testVehicleB);
        std::this_thread::sleep_for(std::chrono::seconds(50));
    }

    // Cleanup
    theTrafficController->stopController();
    delete theTrafficController;
    delete theIntersection;
    return 0;
}
