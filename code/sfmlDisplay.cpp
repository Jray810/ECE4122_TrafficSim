/**
 * Main
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Main program for using SFML to display the project
 * 
 * Revision History:
 *      05DEC2021  R-11-30: Document Created, initial coding
 * 
 **/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "intersect4wsl.h"
#include "autoTrafficController.h"
#include "lightTrafficController.h"
#include "stopTrafficController.h"

// Window Dimension Constants
#define WINDOW_XDIM 1920
#define WINDOW_YDIM 1080

// Controller Type
#define AUTO    0
#define LIGHT   1
#define STOP    2
unsigned int controllerType;

// Default Speed Limit
#define DEFAULT_SPEED_LIMIT 4

// Default Tick Speed
#define DEFAULT_TICK_MICROS 100000

// Spawn Chance
#define PROBABILITY 25

// Fonts
sf::Font thinFont;
sf::Font regFont;
sf::Font boldFont;

// Global Variables
Intersect4WSL* theIntersection;
TrafficController* theTrafficController;
sf::Texture textureCollection[8];
std::vector<Vehicle*> vehicleCollection;
std::vector<sf::Sprite*> spriteCollection;
unsigned int tickSpeed;

struct Coord
{
    double x;
    double y;
    double r;
};

void cleanup();
void vehicleSpawner();
Coord scaleCorrection(Coord pos, double s);
Coord getPos(std::string lane_id, double t);
Coord getPosRight(unsigned int lane, double t, double s);
Coord getPosStraight(unsigned int lane, double t, double s);
Coord getPosLeft(unsigned int lane, double t, double s);

using namespace sf;

/**
 * Main
 **/
int main(int argc, char *argv[])
{
    // Seed RNG
    srand(time(NULL));

    // Command Line Argument Detection
    tickSpeed = DEFAULT_TICK_MICROS;
    if (argc == 1)
    {
        controllerType = 0;
    }
    if (argc >= 2)
    {
        std::string typeArg = argv[1];
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
                std::cerr << "Something went wrong\n";
                return 1;
                break;
        }
    }
    if (argc >= 3)
    {
        std::string typeArg = argv[2];
        tickSpeed = stoi(typeArg);
    }

    // Create and Open a Window
    VideoMode vm(WINDOW_XDIM, WINDOW_YDIM);
    RenderWindow window(vm, "Traffic Simulator", Style::Default);

    // Load Fonts
    thinFont.loadFromFile("fonts/HalisR-Light.otf");
    regFont.loadFromFile("fonts/HalisR-Medium.otf");
    boldFont.loadFromFile("fonts/HalisR-Regular.otf");

    // Text Objects
    Text titleText("Traffic Simulator", regFont, 100);
    titleText.setFillColor(Color::White);
    titleText.setPosition(50,50);
    Text countText("Vehicles Processed: ", thinFont, 24);
    countText.setFillColor(Color::White);
    countText.setPosition(1600, 40);
    Text averageWaitText("Average Wait: ", thinFont, 24);
    averageWaitText.setFillColor(Color::White);
    averageWaitText.setPosition(1600, 70);

    // Create and Initialize Background Sprite
    Texture textureBackground;
    std::string intersectionType = controllerType == STOP ? "Stop" : "";
    textureBackground.loadFromFile("graphics/intersection" + intersectionType + ".png");
    Sprite spriteBackground;
    spriteBackground.setTexture(textureBackground);
    spriteBackground.setPosition(0,0);

    // Create Vehicle Textures
    for (int i=0; i<7; ++i)
    {
        Texture textureVehicle;
        textureVehicle.loadFromFile("graphics/vehicle" + std::to_string(i) + ".png");
        textureCollection[i] = textureVehicle;
    }
    Texture textureCow;
    textureCow.loadFromFile("graphics/cow.png");
    textureCollection[7] = textureCow;

    // Setup Environment
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

    /**
     * Draw Start Screen
     **/
    window.clear();
    window.draw(spriteBackground);
    window.draw(titleText);
    window.display();

    double s = 36;
    unsigned long int vehiclesProcessed = 0;
    double averageWait = 0;

    // Enter Display Loop
    while (window.isOpen())
    {
        /**
         * Check if user presses escape key
         */
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            // Close window and quit game if user presses escape key
            window.close();
        }

        // Spawn a Vehicle
        vehicleSpawner();

        /**
         * Draw everything
         **/
        // Clear Window
        window.clear();

        // Draw Background
        window.draw(spriteBackground);

        // Draw Vehicles
        theTrafficController->protectControlledPods.lock();
        bool leaveControl = false;
        for (int i=0; i<vehicleCollection.size(); ++i)
        {
            Vehicle* thisVehicle = vehicleCollection[i];
            if (thisVehicle->isExited())
            {
                vehiclesProcessed++;
                double waitTime = controllerType == STOP ? thisVehicle->getWaitTime()-5 : thisVehicle->getWaitTime();
                averageWait = (averageWait * ((double)vehiclesProcessed-1) + waitTime) / (double) vehiclesProcessed;
                delete thisVehicle;
                delete spriteCollection[i];
                vehicleCollection.erase(vehicleCollection.begin() + i);
                spriteCollection.erase(spriteCollection.begin() + i);
                --i;
                continue;
            }
            Pod* podPtr = (Pod*)(thisVehicle->getPod());
            if (podPtr == NULL)
            {
                continue;
            }
            Coord pos = getPos(podPtr->getLane()->getLaneID(), podPtr->getPosition());
            pos.x = WINDOW_XDIM/2 + 0 + pos.x*s;
            pos.y = WINDOW_YDIM/2 + 0 + pos.y*s;
            // pos = scaleCorrection(pos, s);
            if (pos.x < 0 || pos.x > WINDOW_XDIM || pos.y < 0 || pos.y > WINDOW_YDIM)
            {
                continue;
            }
            Sprite* vehicleSprite = spriteCollection[i];
            vehicleSprite->setPosition(pos.x, pos.y);
            vehicleSprite->setRotation(pos.r);
            window.draw(*vehicleSprite);
        }
        theTrafficController->protectControlledPods.unlock();

        // Draw Text
        window.draw(titleText);
        countText.setString("Vehicles Processed: " + std::to_string(vehiclesProcessed));
        window.draw(countText);
        averageWaitText.setString("Average Wait: " + std::to_string(averageWait));
        window.draw(averageWaitText);

        // Display Window
        window.display();

        // Update rate at approx UPDATE_GAP_MS between updates
        std::this_thread::sleep_for(std::chrono::microseconds(tickSpeed));
    }

    // Cleanup
    cleanup();
    return 0;
}

void cleanup()
{
    theTrafficController->stopController();
    delete theTrafficController;
    delete theIntersection;
}

void vehicleSpawner()
{
    // Spawn at spawn chance probability
    if (rand() % 100 > PROBABILITY)
    {
        return;
    }

    // Spawn one vehicle
    std::string randomizedID = "";
    for (int i=0; i<10; ++i)
    {
        randomizedID += std::to_string(rand() % 10);
    }
    unsigned int src = rand() % theIntersection->getNumNodes();
    unsigned int dest = rand() % theIntersection->getNumNodes();
    if (src == dest)
    {
        return;
    }
    // unsigned int carType = rand() % 7;
    Vehicle* newVehicle = new Vehicle(randomizedID, 10, 10, 1, theIntersection->getNode(std::to_string(src)), theIntersection->getNode(std::to_string(dest)));
    vehicleCollection.push_back(newVehicle);
    Sprite* spriteVehicle = new Sprite;
    spriteVehicle->setTexture(textureCollection[7]);
    spriteVehicle->setScale(Vector2f(0.5f, 0.5f));
    spriteVehicle->setOrigin(30, 30);
    spriteCollection.push_back(spriteVehicle);
    theTrafficController->entryQueue.push(newVehicle);
}

// Corrects scaling
Coord scaleCorrection(Coord pos, double s)
{
    double correctionFactor = (s-1)*4;
    pos.x = pos.x > 0 ? pos.x - correctionFactor : pos.x + correctionFactor;
    pos.y = pos.y > 0 ? pos.y - correctionFactor : pos.y + correctionFactor;
    return pos;
}

// Gives offset from center of intersection
Coord getPos(std::string lane_id, double t)
{
    double scale = 1;

    Coord position;

    if (lane_id == "0-1" || lane_id == "1-2" || lane_id == "2-3" || lane_id == "3-0")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosRight(src, t, scale);
    }
    else if (lane_id == "0-2" || lane_id == "1-3" || lane_id == "2-0" || lane_id == "3-1")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosStraight(src, t, scale);
    }
    else if (lane_id == "0-3" || lane_id == "1-0" || lane_id == "2-1" || lane_id == "3-2")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosLeft(src, t, scale);
    }
    else
    {
        std::cerr << "something went wrong!\n";
    }

    return position;
}

// Right Turn Position Calculator
Coord getPosRight(unsigned int lane, double t, double s)
{
    Coord position;

    if (t < 20)
    {
        position.x = -1 * (20-t) * s;
        position.y = -1 * 2;
    }
    else if (t < 30)
    {
        position.x = 2 * sin((t-20)*M_PI/20);
        position.y = -1 * 2 * cos((t-20)*M_PI/20);
    }
    else
    {
        position.x = 2;
        position.y = (t-30) * s;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 3;
            position.y += 3;
            position.r = 0;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 3;
            position.y += 3;
            position.r = -90;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 3;
            position.y -= 3;
            position.r = 180;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 3;
            position.y -= 3;
            position.r = 90;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }
    
    position.r += 90;

    if (t > 30)
    {
        position.r += 90;
        if (position.r > 180)
        {
            position.r -= 360;
        }
    }
    else if (t > 20)
    {
        // std::cout << "tangent\n";
    }

    return position;
}

// Straight Position Calculator
Coord getPosStraight(unsigned int lane, double t, double s)
{
    Coord position;

    double a;
    if (t < 20)
    {
        a = (t-23)*s;
    }
    else if (t < 30)
    {
        a = (t-20)*3/5 - 3;
    }
    else
    {
        a = (t-27)*s;
    }

    double b = 1;

    switch (lane)
    {
        case 0:
            position.x = a;
            position.y = b;
            position.r = 90;
            break;
        case 2:
            position.x = -1*a;
            position.y = -1*b;
            position.r = -90;
            break;
        case 1:
            position.x = b;
            position.y = -1*a;
            position.r = 0;
            break;
        case 3:
            position.x = -1*b;
            position.y = a;
            position.r = 180;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }

    return position;
}

// Left Position Calculator
Coord getPosLeft(unsigned int lane, double t, double s)
{
    Coord position;

    if (t < 20)
    {
        position.x = -1 * (20-t) * s;
        position.y = 4;
    }
    else if (t < 30)
    {
        position.x = 4 * sin((t-20)*M_PI/20);
        position.y = 4 * cos((t-20)*M_PI/20);
    }
    else
    {
        position.x = 4;
        position.y = (30-t) * s;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 3;
            position.y -= 3;
            position.r = 90;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 3;
            position.y += 3;
            position.r = 0;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 3;
            position.y += 3;
            position.r = -90;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 3;
            position.y -= 3;
            position.r = 180;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }

    if (t > 30)
    {
        position.r -= 90;
        if (position.r < -180)
        {
            position.r += 360;
        }
    }
    else if (t > 20)
    {
        // std::cout << "tangent\n";
    }

    return position;
}