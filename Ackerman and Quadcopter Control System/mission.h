#ifndef MISSION_H
#define MISSION_H

#include <vector>
#include "missioninterface.h"
#include "pfmsconnector.h"
#include <map>
#include <mutex>
#include <atomic>

class Mission: public MissionInterface
{
public:
    /**
    The Default constructor
    @sa ControllerInterface and @sa MissionInterface for more information
    */
  Mission(std::vector<ControllerInterface*> controllers);

  // Destructor
    virtual ~Mission();

   
    void setGoals(std::vector<pfms::geometry_msgs::Point> goals, pfms::PlatformType platform) override; // Set the goals for the mission based on the platform type

    // Runs the mission, starting each controller in a separate thread
    bool run() override;

    // Returns the mission completion status as a percentage for each platform
    std::vector<unsigned int> status() override;

    // Set the mission objective 
    void setMissionObjective(mission::Objective objective) override;

    // Returns the total distance travelled by each platform since the program started
    std::vector<double> getDistanceTravelled() override;

    // Returns the total time each platform has been moving since the program started
    std::vector<double> getTimeMoving() override;

    // Provides the association of which platform is completing which goal
    std::vector<std::pair<int, int>> getPlatformGoalAssociation() override;





private:
  std::vector<ControllerInterface*> controllers_; //!< A private copy of ControllerInterfaces @sa ControllerInterface

   // A map to store goals per platform type
    std::map<pfms::PlatformType, std::vector<pfms::geometry_msgs::Point>> platformGoals_;
    
  // Current mission objective, defaults to BASIC
    mission::Objective currentObjective_{mission::BASIC};

    // Helper method to initiate threads for each controller
    void startControllers();

    // Helper method to collect and aggregate status from controllers
    void updateMissionStatus();

    // Atomic flag to track if the mission can be completed
    std::atomic<bool> missionPossible_{true};

    // Mutex for synchronizing access to shared resources
    std::mutex mutex_;

};

#endif // RANGERFUSION_H
