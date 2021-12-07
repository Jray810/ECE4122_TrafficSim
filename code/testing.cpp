/**
 * Main
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Program for testing simulation situations.
 *      No display support.
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      04DEC2021  R-12-04: Added TEST_ADDVEHICLES and TEST_STOPCONTROLLER
 *      06DEC2021  R-12-06: Added TEST_TRAFFICJAM
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "debugSetup.h"

#include "intersect4wsl.h"
#include "autoTrafficController.h"
#include "lightTrafficController.h"
#include "stopTrafficController.h"

#define DEFAULT_SPEED_LIMIT 4

#define TEST_INTERSECTION 0
#define TEST_ADDVEHICLES 0
#define TEST_STOPCONTROLLER 0
#define TEST_TRAFFICJAM 1

#define AUTO    0
#define LIGHT   1
#define STOP    2
unsigned int controllerType;

// Default Speed Limit
#define DEFAULT_SPEED_LIMIT 4

// Default Tick Speed
#define DEFAULT_TICK_MICROS 1000000

using namespace std;

int main(int argc, char *argv[])
{
    // Set debug to true
    DEBUG = true;

    // Seed RNG
    srand(time(NULL));

    // Command Line Argument Detection
    tickSpeed = DEFAULT_TICK_MICROS;
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
    if (argc >= 3)
    {
        std::string typeArg = argv[2];
        tickSpeed = stoi(typeArg);
    }


    // Setup testing environment
    double speedLimit = controllerType == STOP ? 1 : DEFAULT_SPEED_LIMIT;
    theIntersection = new Intersect4WSL(speedLimit);
    switch (controllerType)
    {
        case AUTO:
            theTrafficController = new AutoTrafficController(theIntersection, tickSpeed);
            break;
        case LIGHT:
            theTrafficController = new LightTrafficController(theIntersection, tickSpeed);
            break;
        case STOP:
            theTrafficController = new StopTrafficController(theIntersection, tickSpeed);
            break;
        default:
            theTrafficController = new AutoTrafficController(theIntersection, tickSpeed);
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
        Vehicle* testVehicleA = new Vehicle("Alice", 10, 10, 1, theIntersection->getNode("0"), theIntersection->getNode("2"));
        theTrafficController->entryQueue.push(testVehicleA);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        Vehicle* testVehicleB = new Vehicle("Bob", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("3"));
        theTrafficController->entryQueue.push(testVehicleB);
        Vehicle* testVehicleC = new Vehicle("Carol", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("2"));
        theTrafficController->entryQueue.push(testVehicleC);
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    // Test Stop Sign Controlled Traffic Controller
    if (TEST_STOPCONTROLLER)
    {
        cout << "Testing Stop Sign Controlled Traffic Controller\n";
        Vehicle* testVehicleA = new Vehicle("Alice", 10, 10, 1, theIntersection->getNode("0"), theIntersection->getNode("2"));
        theTrafficController->entryQueue.push(testVehicleA);
        Vehicle* testVehicleB = new Vehicle("Bob", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("3"));
        theTrafficController->entryQueue.push(testVehicleB);
        Vehicle* testVehicleC = new Vehicle("Carol", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("2"));
        theTrafficController->entryQueue.push(testVehicleC);
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    // Test Traffic Jame
    if (TEST_TRAFFICJAM)
    {
        cout << "Testing Traffic Jam\n";
        for (int i=0; i<10; ++i)
        {
            int dest = (rand() % 2) + 2;
            int l = rand() % 2;
            Vehicle* testVehicle = new Vehicle(std::to_string(i) + std::to_string(l), 10, 10, 1, theIntersection->getNode(std::to_string(l)), theIntersection->getNode(std::to_string(dest)));
            theTrafficController->entryQueue.push(testVehicle);
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    // Cleanup
    theTrafficController->stopController();
    delete theTrafficController;
    delete theIntersection;
    return 0;
}
