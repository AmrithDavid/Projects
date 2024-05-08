#include "mission.h"
#include <thread>
#include <mutex>

// Constructor
Mission::Mission(std::vector<ControllerInterface*> controllers)
    : controllers_(controllers) {}

// Destructor
Mission::~Mission() {}
// Set the goals for a specific platform and ensure thread safety using a mutex
void Mission::setGoals(std::vector<pfms::geometry_msgs::Point> goals, pfms::PlatformType platform) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    platformGoals_[platform] = goals;
}
// Starts the mission by launching separate threads for each controller based on their platform type
bool Mission::run() {
    std::vector<std::thread> threads;
    for (auto& controller : controllers_) {
        if (controller->getPlatformType() == pfms::QUADCOPTER) {
            threads.emplace_back([&] {
                controller->setGoals(platformGoals_[pfms::QUADCOPTER]);
                controller->run();
            });
        } else if (controller->getPlatformType() == pfms::ACKERMAN) {
            threads.emplace_back([&] {
                controller->setGoals(platformGoals_[pfms::ACKERMAN]);
                controller->run();
            });
        }
    }
  // Wait for all threads to complete their tasks
     for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
// Returns true if the mission was possible to complete
    return missionPossible_.load();
}
// Calculates and returns the mission completion status as a percentage of total distance travelled
std::vector<unsigned int> Mission::status() {
    std::vector<unsigned int> completionStatus(controllers_.size());
    double totalDistance = 0;
    for (auto& goal : platformGoals_) {
        for (auto& point : goal.second) {
            totalDistance += sqrt(point.x * point.x + point.y * point.y); // Assuming the start is at (0,0)
        }
    }
// Compute the percentage of distance travelled by each controller
    double distanceTravelled = 0;
    for (size_t i = 0; i < controllers_.size(); ++i) {
        distanceTravelled += controllers_[i]->distanceTravelled();
        completionStatus[i] = static_cast<unsigned int>((distanceTravelled / totalDistance) * 100);
    }

    return completionStatus;
}

void Mission::setMissionObjective(mission::Objective objective) {
    currentObjective_ = objective;
}
// Retrieves the distance travelled by each platform since the mission started
std::vector<double> Mission::getDistanceTravelled() {
    std::vector<double> distances(controllers_.size());
    for (size_t i = 0; i < controllers_.size(); ++i) {
        distances[i] = controllers_[i]->distanceTravelled();
    }
    return distances;
}
// Retrieves the total time each platform has been actively moving since the mission started
std::vector<double> Mission::getTimeMoving() {
    std::vector<double> times(controllers_.size());
    for (size_t i = 0; i < controllers_.size(); ++i) {
        times[i] = controllers_[i]->timeTravelled();
    }
    return times;
}
// Returns a list of pairs indicating which platform is completing which goal
std::vector<std::pair<int, int>> Mission::getPlatformGoalAssociation() {
    std::vector<std::pair<int, int>> associations;
    for (auto& pair : platformGoals_) {
        for (size_t i = 0; i < pair.second.size(); i++) {
            associations.emplace_back(static_cast<int>(pair.first), i);
        }
    }
    return associations;
}