#ifndef ACKERMAN_H
#define ACKERMAN_H

#include "controller.h"
#include <atomic>
#include <vector>
#include <chrono>

class Ackerman : public Controller {
public:
    Ackerman();
    virtual ~Ackerman();  // Declare the destructor

    void run() override;
    bool setGoals(std::vector<pfms::geometry_msgs::Point> goals) override;
     bool checkOriginToDestination(pfms::nav_msgs::Odometry origin,
                                  pfms::geometry_msgs::Point goal,
                                  double& distance,
                                  double& time,
                                  pfms::nav_msgs::Odometry& estimatedGoalPose) override;
    double timeToGoal() override;
    double distanceTravelled() override;
    

    

private:

    
    
    long unsigned int i = 0;          // Sequence number for commands
    double throttle;   // Throttle value
    double brake;      // Brake value
    double steering;   // Steering value
    

};


#endif // ACKERMAN_H
