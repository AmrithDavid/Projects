#include "ackerman.h"
#include "audi.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath> 
#include <thread>

Ackerman::Ackerman() : Controller(), i(0) {
    // Set the tolerance using the existing function from the Controller class
    setTolerance(0.5);  // Initialize tolerance to 0.5 meters

    // Set the specific type for Ackerman
    setType(pfms::PlatformType::ACKERMAN);
}
Ackerman::~Ackerman()
{
    // delete pipes here which forces them to close before the object is terminated
    PfmsConnector_.reset();
};

Audi::Audi() : max_break_torque_(8000.0),
               steering_ratio_(17.3),
               lock_to_lock_revs_(3.2),
               max_steer_angle_(M_PI * lock_to_lock_revs_ / steering_ratio_),
               wheelbase_(2.65),
               steadyStateV_(2.91),
               deltaD_(1.638){};

void Ackerman::run()
{
    Audi audi_;
    for (auto goal : goals_)
    {

        pfms::nav_msgs::Odometry currentOdo = getOdometry();


// Start tracking motion for this goal
        startMotion();

        // Set status to running
       setStatus(pfms::PlatformStatus::RUNNING);

        // Calculate the distance to the goal
        double distance = distanceToGoal();

        // While distance from goal is still more than 1.5m we accelerate
        while (distance > 1.5)
        {
            // Compute steering
            audi_.computeSteering(currentOdo, goal, steering, distance);

            // Accelerate
            throttle = 0.2;

            
            brake = 0.0;

            // Create and send command
            pfms::commands::Ackerman cmd{
                i, // Sequence number
                brake,
                steering,
                throttle};

            PfmsConnector_->send(cmd);

            // Update odometry 
            currentOdo = getOdometry();

            double distance = distanceToGoal();
            // Sleep to control loop rate
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            i++;
        }

        // While distance to goal is less than 1m and greater than 0m, decelerate
        while (distance < 1.5)
        {
            // Compute steering 
            audi_.computeSteering(currentOdo, goal, steering, distance);

            // Decelerate
            throttle = 0.0;

           
            brake = 300.0;

            // Create and send command
            pfms::commands::Ackerman cmd{
                i, // Sequence number
                brake,
                steering,
                throttle};
            PfmsConnector_->send(cmd);

            // Update odometry 
            currentOdo = getOdometry();
            distance = distanceToGoal();

            // Sleep to control loop rate
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            i++;
        }

        // Stop when distance to goal is 0m
        throttle = 0.0;
        
       
        brake = 8000.0; 

        // Keep steering straight
        steering = 0.0; 

        // Create and send command
        pfms::commands::Ackerman cmd{
            i, // Sequence number
            brake,
            steering,
            throttle};
        PfmsConnector_->send(cmd);
    }
      // At the end of reaching the goal, mark the end of motion
        stopMotion();

    setStatus(pfms::PlatformStatus::IDLE);
}

bool Ackerman::setGoals(std::vector<pfms::geometry_msgs::Point> goals)
{
    // get Odo
     pfms::nav_msgs::Odometry odo = getOdometry();

     pfms::nav_msgs::Odometry estimatedGoalPose;
// put goals into the goals_ vector
    goals_ = goals;

    bool allReachable = true;
    for (auto &goal : goals) {
        if (!checkOriginToDestination(odo, goal, distance_, time_, estimatedGoalPose)) {
            allReachable = false; // mark as false if any goal is not reachable
            break; // 
        }
    }
    return allReachable; // return true only if all goals are reachable
}

bool Ackerman::checkOriginToDestination(pfms::nav_msgs::Odometry origin,
                                        pfms::geometry_msgs::Point goal,
                                        double &distance,
                                        double &time,
                                        pfms::nav_msgs::Odometry &estimatedGoalPose)
{
    Audi audi_;

pfms::nav_msgs::Odometry odo = getOdometry();

    
    if (audi_.checkOriginToDestination(odo, goal, distance_, time_, estimatedGoalPose))
    {
        return true;
    }
    else
        return false;
}

double Ackerman::timeToGoal()
{
    // Get the distance to the goal and set maxSpeed
    // double distance_ = distanceToGoal();
    double maxSpeed = 2.91;
    // time = distance/speed
    time_ = distance_ / maxSpeed;

    return time_;
}

double Ackerman::distanceTravelled()
{
    double totalDistance = 0.0;

    // Get the initial odometry
    pfms::nav_msgs::Odometry lastOdometry = getOdometry();

    // Iterate over recorded motion intervals
    for (const auto& interval : motionIntervals)
    {
        
        pfms::nav_msgs::Odometry currentOdometry = getOdometry();

        
        double dx = currentOdometry.position.x - lastOdometry.position.x;
        double dy = currentOdometry.position.y - lastOdometry.position.y;
        
        double intervalDistance = std::sqrt(dx * dx + dy * dy);

        // Update last odometry for the next iteration
        lastOdometry = currentOdometry;

        // Add the interval distance to the total distance travelled
        totalDistance += intervalDistance;
    }

    return totalDistance;
}
    

