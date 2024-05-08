#include "quadcopter.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

Quadcopter::Quadcopter() : Controller(), ii(0)

{

    // Set the tolerance using the existing function from the Controller class
    setTolerance(0.5); // Initialize tolerance to 0.5 meters

    // Set the specific type for Quadcopter
    setType(pfms::PlatformType::QUADCOPTER);
}

Quadcopter::~Quadcopter() {
    // deletes pipes  which forces them to close before the object is terminated
    PfmsConnector_.reset();
}

bool Quadcopter::checkOriginToDestination(pfms::nav_msgs::Odometry origin,
                                          pfms::geometry_msgs::Point goal,
                                          double& distance,
                                          double& time,
                                          pfms::nav_msgs::Odometry& estimatedGoalPose) {
    double dx = goal.x - origin.position.x;
    double dy = goal.y - origin.position.y;

    distance = std::hypot(dx, dy);
    time = distance / TARGET_SPEED; // TARGET_SPEED of 1m/s

    // Set the estimated goal pose
    estimatedGoalPose.position.x = goal.x;
    estimatedGoalPose.position.y = goal.y;
    estimatedGoalPose.yaw = origin.yaw;
    estimatedGoalPose.linear.x = 0;
    estimatedGoalPose.linear.y = 0;

    return true;  
}

void Quadcopter::run() {
    
    setStatus(pfms::PlatformStatus::TAKEOFF);  // Initial state to take off

    setStatus(pfms::PlatformStatus::RUNNING);

    for (auto& goal : goals_) {
        pfms::nav_msgs::Odometry currentOdo = getOdometry();
        startMotion();

        while (distanceToGoal() > tolerance_) {
            // Recalculate direction towards the goal to account for any drift
            double dx = goal.x - currentOdo.position.x;
            double dy = goal.y - currentOdo.position.y;
            double dz = 2.0 - currentOdo.position.z; // Target height is 2 meters
            double target_angle = std::atan2(dy, dx);

            // Normalize target_angle to keep it between -PI and PI
            if (target_angle > M_PI) {
                target_angle -= 2 * M_PI;
            } else if (target_angle < -M_PI) {
                target_angle += 2 * M_PI;
            }

            // Adjust speed based on proximity to the goal to handle inertia
            double distance = std::hypot(dx, dy);
            double speed = std::min(TARGET_SPEED, distance / 2.0); // Reduce speed as we approach the goal

            double move_f_b = speed * std::cos(target_angle);
            double move_l_r = speed * std::sin(target_angle);
            double move_u_d = dz; // Adjust altitude based on current z position

            // Apply conditional altitude adjustments
            if (currentOdo.position.z > 2.1) { // If too high
                move_u_d = -0.1; // Move down
            } else if (currentOdo.position.z < 1.9) { // If too low
                move_u_d = 0.1; // Move up
            }

            // Send the command to the platform
            pfms::commands::Quadcopter cmd = {ii ++, 0, move_l_r, move_u_d, move_f_b};
            PfmsConnector_->send(cmd);

            // Update odometry for next loop iteration
            currentOdo = getOdometry();
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Adjust timing based on command response time
        }
        stopMotion();
    }

    setStatus(pfms::PlatformStatus::IDLE);
}

bool Quadcopter::setGoals(std::vector<pfms::geometry_msgs::Point> goals) {
    pfms::nav_msgs::Odometry odo = getOdometry();
    pfms::nav_msgs::Odometry estimatedGoalPose;
    goals_ = goals;

    bool allReachable = true;
    for (auto &goal : goals) {
        if (!checkOriginToDestination(odo, goal, distance_, time_, estimatedGoalPose)) {
            allReachable = false;  // If any goal is unreachable, set to false
            break;
        }
    }
    return allReachable;  // Return true only if all goals are reachable
}

double Quadcopter::timeToGoal() {
    double distance = distanceToGoal();
    return distance / TARGET_SPEED;  // Time estimation based on constant speed
}

double Quadcopter::distanceTravelled()
{
    double totalDistance = 0.0;

    // Get the initial odometry
    pfms::nav_msgs::Odometry lastOdometry = getOdometry();

    // Iterate over recorded motion intervals
    for (const auto& interval : motionIntervals)
    {
        // Get the current odometry
        pfms::nav_msgs::Odometry currentOdometry = getOdometry();

        // Calculate the Euclidean distance between two odometry readings
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

