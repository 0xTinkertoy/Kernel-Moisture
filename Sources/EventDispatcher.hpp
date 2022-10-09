//
//  EventDispatcher.hpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/5/21.
//

#ifndef EventDispatcher_hpp
#define EventDispatcher_hpp

#include <Execution/Common/Dispatcher.hpp>
#include <Execution/Common/KernelServiceRoutines.hpp>
#include <Execution/SimpleEventDriven/KernelServiceRoutines.hpp>
#include <Execution/SimpleEventDriven/EventHandlerTrampoline.hpp>
#include "EventHandlerTrampolineContextBuilder.hpp"
#include "EventHandlerSwitcher.hpp"
#include "EventScheduler.hpp"
#include "UART/PL011.hpp"
#include "Message.hpp"
#include "EventController.hpp"
#include "CMSIS/ARMCM3.h"

extern EventControlBlock gEventTable[4];

struct EventControlBlockMapper
{
    EventControlBlock* operator()(int event)
    {
        return KernelServiceRoutines::GetTaskController<EventController>().getRegisteredEvent(event);
    }
};

//
// MARK: - Define kernel service routine functions and the mapper for the dispatcher
//
namespace KernelServiceRoutines
{
    using SyscallSendEventRoutine = KernelServiceRoutines::SyscallSendEvent<EventControlBlock, EventScheduler, EventControlBlockMapper>;
    OSDefineAndRouteKernelRoutine(kSyscallSendEventRoutine, EventControlBlock, SyscallSendEventRoutine)

    using SyscallEventHandlerReturnRoutine = KernelServiceRoutines::SyscallEventHandlerReturn<EventControlBlock, EventScheduler>;
    OSDefineAndRouteKernelRoutine(kSyscallEventHandlerReturnRoutine, EventControlBlock, SyscallEventHandlerReturnRoutine)

    using SyscallUnknownIdentifierRoutine = KernelServiceRoutines::UnknownServiceIdentifier<EventControlBlock>;
    OSDefineAndRouteKernelRoutine(kSyscallUnknownIdentifier, EventControlBlock, SyscallUnknownIdentifierRoutine)

    static EventControlBlock* kSysTickInterruptHandler(EventControlBlock* current)
    {
        /// Every 10 seconds
        static UInt32 timeout = 5000;

        pinfo("SysTick Interrupt.");

        timeout -= 1;

        if (timeout == 0)
        {
            pinfo("Periodic Event Triggered.");

            timeout = 5000;

            current = GetTaskScheduler<EventScheduler>().onTaskCreated(current, GetTaskController<EventController>().getRegisteredEvent(1));
        }
        else
        {
            pinfo("Periodic Event Not Triggered. Timeout = %d.", timeout);
        }

        return current;
    }

    static UInt32 kMoistureLevel = 0;

    static EventControlBlock* kUART1ReceiveInterruptHandler(EventControlBlock* current)
    {
        pmesg("UART1 RX Interrupt.");

        Message message = {};

        PL011::receive(PL011::kUART1, message);

        if (message.magic == 0x4657 && message.type == Message::Type::kChangeSoilMoisture)
        {
            kMoistureLevel = message.data;

            pmesg("Environment: Moisture level has been changed to %d.", kMoistureLevel);
        }

        PL011::clearRxInterrupt(PL011::kUART1);

        return current;
    }

    static EventControlBlock* kReadSensorRoutine(EventControlBlock* current)
    {
        current->setSyscallKernelReturnValue(kMoistureLevel);

        return current;
    }

    static EventControlBlock* kSendDataRoutine(EventControlBlock* current)
    {
        const void* data = current->getSyscallArgument<const void*>();

        auto count = current->getSyscallArgument<size_t>();

        PL011::send(PL011::kUART1, data, count);

        current->setSyscallKernelReturnValue(count);

        return current;
    }

#ifndef RUN_STACK_EXP
    static EventControlBlock* kPrintRoutine(EventControlBlock* current)
    {
        const char* format = current->getSyscallArgument<const char*>();

        auto args = current->getSyscallArgument<va_list*>();

        kvprintf(format, *args);

        return current;
    }
#endif

    static EventControlBlock* kSetEventHandler(EventControlBlock* current)
    {
        auto event = current->getSyscallArgument<Event>();

        auto handler = current->getSyscallArgument<EventHandler>();

        GetTaskController<EventController>().registerEvent(event, handler);

        return current;
    }
}

struct EventDispatcherRoutineMapper
{
    using Task = EventControlBlock;

    using Routine = Task* (*)(Task*);

    using ServiceIdentifier = int;

    Routine operator()(const ServiceIdentifier& identifier)
    {
        using namespace KernelServiceRoutines;

        switch (identifier)
        {
            case 0:
                return kSetEventHandler;

            case 1:
                return kSyscallSendEventRoutine;

            case 2:
                return kSyscallEventHandlerReturnRoutine;

            case 3:
                return kReadSensorRoutine;

            case 4:
                return kSendDataRoutine;

#ifndef RUN_STACK_EXP
            case 5:
                return kPrintRoutine;
#endif

            case 15:
                return &kSysTickInterruptHandler;

            case 22:
                return &kUART1ReceiveInterruptHandler;

            default:
                return kSyscallUnknownIdentifier;
        }
    }
};

//
// MARK: - Define additional code injector for the dispatcher
//

using Injector = PreemptiveEventHandlerTrampolineContextInjector<EventControlBlock, EventHandlerTrampolineContextBuilder_ARM>;

//
// MARK: - Assemble a custom dispatcher for a simple event-driven system
//

using EventDispatcher = Dispatcher<EventControlBlock, int, EventDispatcherRoutineMapper, EventHandlerSwitcher, Injector>;

#endif /* EventDispatcher_hpp */
