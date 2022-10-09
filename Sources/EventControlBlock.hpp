//
//  EventControlBlock.hpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/5/21.
//

#ifndef EventControlBlock_hpp
#define EventControlBlock_hpp

#include <Types.hpp>
#include <ARM/Context.hpp>
#include <Scheduler/Scheduler.hpp>
#include <Execution/Common/TaskControlBlockComponents.hpp>

using EventHandler = void(*)();
using Event = unsigned int;

struct EventControlBlock: Scheduler::Schedulable, Listable<EventControlBlock>,
        TaskControlBlockComponents::SharedStackSupport<EventControlBlock>,
        TaskControlBlockComponents::SystemCallSupport<EventControlBlock, Context>,
        TaskControlBlockComponents::EventHandlerSupport<EventControlBlock, EventHandler>
{
    friend std::strong_ordering operator <=>(const EventControlBlock& lhs, const EventControlBlock& rhs)
    {
        return std::addressof(lhs) <=> std::addressof(rhs);
    }
};

#endif /* EventControlBlock_hpp */
