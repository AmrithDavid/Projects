#include "controller.h"

Controller::Controller()
    : PfmsConnector_(std::make_shared<PfmsConnector>()), executionStatus_(pfms::PlatformStatus::IDLE) {
    // Constructor code
}

Controller::~Controller() {
    // Destructor code to handle cleanup if necessary
}

void Controller::setStatus(pfms::PlatformStatus newStatus) {
    executionStatus_.store(newStatus);  // Thread-safe update
    PfmsConnector_->send(newStatus);    // Notify external systems
}

pfms::PlatformStatus Controller::status() {
    return executionStatus_.load();  // Return the current status
}


double Controller::timeTravelled() {
    using namespace std::chrono;
    double totalTime = 0.0;
    for (const auto& interval : motionIntervals) {
        
        auto intervalDuration = interval.second - interval.first;

        // Cast the duration to a duration type with double seconds precision
        auto durationInSeconds = duration_cast<duration<double>>(intervalDuration);

        // Add the duration in seconds to the total time
        totalTime += durationInSeconds.count();
    }
    return totalTime;
}

void Controller::startMotion() {
    motionIntervals.push_back({std::chrono::high_resolution_clock::now(), {}});
}

void Controller::stopMotion() {
    if (!motionIntervals.empty() && motionIntervals.back().second == std::chrono::high_resolution_clock::time_point{}) {
        motionIntervals.back().second = std::chrono::high_resolution_clock::now();
    }
}

void Controller::setType(pfms::PlatformType type) {
    platformType_ = type; // Set the platform type
}

pfms::PlatformType Controller::getPlatformType() {
    return platformType_; // Return the stored platform type
}

bool Controller::setTolerance(double newTolerance) {
    if (newTolerance >= 0) {
        tolerance_ = newTolerance; // Directly set the member variable
        return true; // Tolerance accepted
    }
    return false; // Tolerance not accepted due to invalid value
}

pfms::nav_msgs::Odometry Controller::getOdometry() {
    pfms::nav_msgs::Odometry odo;
    if (PfmsConnector_) {
        PfmsConnector_->read(odo, getPlatformType());
    }
    return odo;
}

double Controller::distanceToGoal()
{
    // Get the odometry
    pfms::nav_msgs::Odometry odo = getOdometry();

    // get current x and y coordinate
    currentPosX_ = odo.position.x;
    currentPosY_ = odo.position.y;

    if (goals_.empty()) {

        return 0.0;
    }

    double currentGoalx = goals_.front().x;  // Assuming the next goal is at the front of the list
    double currentGoaly = goals_.front().y;

    // calculate distance by using the Pythagoras' theorem
    double squaredDistance = pow((currentGoaly - currentPosY_), 2) + pow((currentGoalx - currentPosX_), 2);

    distance_ = sqrt(squaredDistance);

    pfms::nav_msgs::Odometry estimatedGoalPose;

    checkOriginToDestination(odo, goals_.front(), distance_, time_, estimatedGoalPose);

    // sqrt to find the distance
    return distance_;
}

 std::vector<pfms::geometry_msgs::Point> Controller::getObstacles()
{
    // Create an empty vector
    std::vector<pfms::geometry_msgs::Point> obstacles;

    // Return the empty vector
    return obstacles;
}

