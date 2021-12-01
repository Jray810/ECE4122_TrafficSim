#include <iostream>
#include "intersect4wsl.h"

#define DEFAULT_SPEED_LIMIT 40

using namespace std;

int main(int argc, char *argv[])
{
    // Intersection Setup Testing
    Intersect4WSL* theIntersection = new Intersect4WSL(DEFAULT_SPEED_LIMIT);

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


    // Cleanup
    delete theIntersection;
    return 0;
}
