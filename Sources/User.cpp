//
//  User.cpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/23/21.
//

#include "User.hpp"
#include "Syscall.hpp"
#include "Message.hpp"

// Remove all printing if we are running experiments to measure the stack usage
#ifdef RUN_STACK_EXP
    #define sysprintf (void)
#endif

__attribute__((noreturn))
void idleHandler()
{
    while (true)
    {
        asm("wfi");
    }
}

void readSensor()
{
    // Read the moisture level (in percentage)
    sysprintf("=================================================\n");

    sysprintf("RSH: Prepare to read the moisture sensor.\n");

    int moisture = sysReadSensor(0);

    sysprintf("RSH: The current moisture level is %d%%.\n", moisture);

    if (moisture < 30)
    {
        sysprintf("RSH: The moisture level has fallen below the threshold.\n");

        sysprintf("RSH: Will notify the dry soil handler.\n");

        sysSendEvent(kDrySoilEvent);
    }
    else
    {
        sysprintf("RSH: No need to water the plant.\n");
    }

    sysprintf("=================================================\n");
}

void readSensorWet()
{
    // Read the moisture level (in percentage)
    sysprintf("=================================================\n");

    sysprintf("RSH: Prepare to read the moisture sensor.\n");

    int moisture = sysReadSensor(0);

    sysprintf("RSH: The current moisture level is %d%%.\n", moisture);

    if (moisture > 50)
    {
        sysprintf("RSH: The moisture level has reached the upper threshold.\n");

        sysprintf("RSH: Will notify the wet soil handler.\n");

        sysSendEvent(kWetSoilEvent);
    }
    else
    {
        sysprintf("RSH: The moisture level has not yet reached the upper threshold.\n");
    }

    sysprintf("=================================================\n");
}

void drySoilHandler()
{
    sysprintf("=================================================\n");

    sysprintf("DSH: Prepare to send a Dry Soil Alert to the actuator.\n");

    Message alert = Message::soilDryAlert();

    if (sysSendData(&alert, sizeof(Message)) == sizeof(Message))
    {
        sysprintf("DSH: Alert has been sent.\n");

        sysSetEventHandler(kSensorEvent, readSensorWet);
    }
    else
    {
        sysprintf("DSH: Failed to send the alert.\n");
    }

    sysprintf("=================================================\n");
}

void wetSoilHandler()
{
    sysprintf("=================================================\n");

    sysprintf("WSH: Prepare to send a Wet Soil Alert to the actuator.\n");

    Message alert = Message::soilWetAlert();

    if (sysSendData(&alert, sizeof(Message)) == sizeof(Message))
    {
        sysprintf("WSH: Alert has been sent.\n");

        sysSetEventHandler(kSensorEvent, readSensor);
    }
    else
    {
        sysprintf("WSH: Failed to send the alert.\n");
    }

    sysprintf("=================================================\n");
}