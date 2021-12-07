// Gives offset from center of intersection
Coord getPos(std::string lane_id, double t)
{
    double scale = 6;

    Coord position;

    // Check Right Turn
    if (lane_id == "0-1" || lane_id == "1-2" || lane_id == "2-3" || lane_id == "3-0")
    {
        unsigned int src = lane_id[0]-'0';
        if (t < 40)
        {
            position.x = -1 * (40-t);
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
            position.y = (t-60);
        }

        double tmp;
        switch(src)
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
        position = getPosRight(src, t);
    }
    else if (lane_id == "0-2" || lane_id == "1-3" || lane_id == "2-0" || lane_id == "3-1")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosStraight(src, t);
    }
    else
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosLeft(src, t);
    }

    return position;
}

// Right Turn Position Calculator
Coord getPosRight(unsigned int lane, double t)
{
    Coord position;

    return position;
}

// Straight Position Calculator
Coord getPosStraight(unsigned int lane, double t)
{
    Coord position;

    double a = (t-50);
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
Coord getPosLeft(unsigned int lane, double t)
{
    Coord position;

    if (t < 40)
    {
        position.x = -1 * (40-t);
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
        position.y = (60-t);
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