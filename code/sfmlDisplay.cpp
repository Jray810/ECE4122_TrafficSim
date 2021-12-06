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

// Spawn Chance
#define PROBABILITY 5

// Fonts
sf::Font thinFont;
sf::Font regFont;
sf::Font boldFont;

// Global Variables
Intersect4WSL* theIntersection;
TrafficController* theTrafficController;
sf::Texture textureCollection[7];
std::map<std::string, Vehicle*> vehicleCollection;
std::map<std::string, sf::Sprite*> spriteCollection;

struct Coord
{
    double x;
    double y;
    double r;
};

void cleanup();
void vehicleSpawner();
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
    if (argc == 1)
    {
        controllerType = 0;
    }
    if (argc == 2)
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
    FloatRect titleTextRect = titleText.getLocalBounds();
    titleText.setOrigin(titleTextRect.left +
        titleTextRect.width / 2.0f,
        titleTextRect.top +
        titleTextRect.height / 2.0f);
    titleText.setPosition(WINDOW_XDIM / 4.0f, WINDOW_YDIM / 6.0f);

    // Create and Initialize Background Sprite
    Texture textureBackground;
    textureBackground.loadFromFile("graphics/background.png");
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

    // Setup Environment
    double speedLimit = controllerType == STOP ? 1 : DEFAULT_SPEED_LIMIT;
    theIntersection = new Intersect4WSL(speedLimit);
    switch (controllerType)
    {
        case AUTO:
            theTrafficController = new AutoTrafficController(theIntersection);
            break;
        case LIGHT:
            theTrafficController = new LightTrafficController(theIntersection);
            break;
        case STOP:
            theTrafficController = new StopTrafficController(theIntersection);
            break;
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

    /**
     * Draw Start Screen
     **/
    window.clear();
    window.draw(spriteBackground);
    window.draw(titleText);
    window.display();

    double s = 6;

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
        std::map<std::string, Vehicle*>::iterator next_it = vehicleCollection.begin();
        for (std::map<std::string, Vehicle*>::iterator it = vehicleCollection.begin(); it != vehicleCollection.end(); it = next_it)
        {
            ++next_it;
            std::string vehicle_id = it->first;
            Pod* podPtr = (Pod*)(it->second->getPod());
            if (podPtr == NULL)
            {
                continue;
            }
            Coord pos = getPos(podPtr->getLane()->getLaneID(), podPtr->getPosition());
            Sprite* vehicleSprite = spriteCollection.find(vehicle_id)->second;
            vehicleSprite->setPosition(WINDOW_XDIM/2 + pos.x*s, WINDOW_YDIM/2 + pos.y*s);
            vehicleSprite->setRotation(pos.r);
            window.draw(*vehicleSprite);
        }

        // Draw Text
        window.draw(titleText);

        // Display Window
        window.display();
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
    unsigned int carType = rand() % 7;
    Vehicle* newVehicle = new Vehicle(randomizedID, 10, 10, 1, theIntersection->getNode(std::to_string(src)), theIntersection->getNode(std::to_string(dest)));
    vehicleCollection.insert({newVehicle->getVehicleID(), newVehicle});
    Sprite* spriteVehicle = new Sprite;
    spriteVehicle->setTexture(textureCollection[carType]);
    spriteVehicle->setScale(Vector2f(0.3f, 0.3f));
    spriteVehicle->setOrigin(30, 59);
    spriteCollection.insert({newVehicle->getVehicleID(), spriteVehicle});
    theTrafficController->entryQueue.push(newVehicle);
}

// Gives offset from center of intersection
Coord getPos(std::string lane_id, double t)
{
    double scale = 6;

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
    else
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosLeft(src, t, scale);
    }

    return position;
}

// Right Turn Position Calculator
Coord getPosRight(unsigned int lane, double t, double s)
{
    Coord position;

    if (t < 40)
    {
        position.x = -1 * (40-t) * s;
        position.y = -1 * 7;
    }
    else if (t <= 60)
    {
        position.x = 7 * sin((t-40)*M_PI/40);
        position.y = -1 * 7 * cos((t-40)*M_PI/40);
    }
    else
    {
        position.x = 7;
        position.y = (t-60) * s;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 10;
            position.y += 10;
            position.r = 0;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 10;
            position.y += 10;
            position.r = -90;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 10;
            position.y -= 10;
            position.r = 180;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 10;
            position.y -= 10;
            position.r = 90;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }
    
    position.r += 90;

    if (t > 60)
    {
        position.r += 90;
        if (position.r > 180)
        {
            position.r -= 360;
        }
    }
    else if (t >= 40)
    {
        // std::cout << "shiet\n";
    }

    return position;
}

// Straight Position Calculator
Coord getPosStraight(unsigned int lane, double t, double s)
{
    Coord position;

    double a = (t-50) * s;
    double b = 3;

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

    if (t < 40)
    {
        position.x = -1 * (40-t) * s;
        position.y = 13;
    }
    else if (t <= 60)
    {
        position.x = 13 * sin((t-40)*M_PI/40);
        position.y = 13 * cos((t-40)*M_PI/40);
    }
    else
    {
        position.x = 13;
        position.y = (60-t) * s;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 10;
            position.y -= 10;
            position.r = 90;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 10;
            position.y += 10;
            position.r = 0;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 10;
            position.y += 10;
            position.r = -90;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 10;
            position.y -= 10;
            position.r = 180;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }

    if (t > 60)
    {
        position.r -= 90;
        if (position.r < -180)
        {
            position.r += 360;
        }
    }
    else if (t >= 40)
    {
        // std::cout << "shiet\n";
    }

    return position;
}