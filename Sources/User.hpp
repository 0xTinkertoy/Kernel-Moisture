//
//  User.hpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/23/21.
//

#ifndef User_hpp
#define User_hpp

//
// Deployment: Event Handlers
//
// Event Identifiers:
// Event 0: Idle (Reserved)
// Event 1: Periodic Sensor Reading
// Event 2: Dry Soil (Notify the actuator to start watering the plant)
// Event 3: Wet Soil (Notify the actuator to stop watering the plant)
//

enum UserEvent
{
    kIdleEvent = 0,
    kSensorEvent = 1,
    kDrySoilEvent = 2,
    kWetSoilEvent = 3
};

__attribute__((noreturn))
void idleHandler();

void readSensor();

void drySoilHandler();

void wetSoilHandler();

#endif /* User_hpp */
