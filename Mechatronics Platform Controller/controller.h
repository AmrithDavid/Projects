#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "controllerinterface.h"
#include <cmath>
#include <pfmsconnector.h>
#include <thread>
#include <memory>

class Controller: public ControllerInterface {
public:
    Controller();
    virtual ~Controller();  // Destructor declaration

    virtual void run() = 0;
    pfms::PlatformStatus status() override;
    virtual bool setGoals(std::vector<pfms::geometry_msgs::Point> goals) = 0;
    virtual bool checkOriginToDestination(pfms::nav_msgs::Odometry origin, pfms::geometry_msgs::Point goal, double& distance, double& time, pfms::nav_msgs::Odometry& estimatedGoalPose) = 0;
    pfms::PlatformType getPlatformType() override;
     double distanceToGoal() override;
    virtual double timeToGoal() = 0;
     bool setTolerance(double tolerance) override;
    virtual double distanceTravelled() = 0;
     double timeTravelled() override;
    pfms::nav_msgs::Odometry getOdometry() override;
    void setType(pfms::PlatformType type); 
     void setStatus(pfms::PlatformStatus newStatus);
     static constexpr double TARGET_SPEED = 1.0; // for quad
     std::vector<pfms::geometry_msgs::Point> getObstacles() override;

protected:

    std::vector<pfms::geometry_msgs::Point> goals_;
    std::shared_ptr<PfmsConnector> PfmsConnector_; // Smart pointer to manage the PfmsConnector
    std::atomic<pfms::PlatformStatus> executionStatus_;  // Status of the controller's execution
    std::vector<std::pair<std::chrono::high_resolution_clock::time_point, std::chrono::high_resolution_clock::time_point>> motionIntervals;
    pfms::PlatformType platformType_;  // Member to store the platform type
    double tolerance_;  // Tolerance for reaching the goal
    double distance_;
    double time_;
    double currentPosX_, currentPosY_; 

    void startMotion();
    void stopMotion();
    

};

#endif // CONTROLLER_H
