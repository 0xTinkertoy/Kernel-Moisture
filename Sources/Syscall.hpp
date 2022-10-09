//
//  Syscall.hpp
//  Kernel-ARM
//
//  Created by FireWolf on 2/5/21.
//

#ifndef Syscall_hpp
#define Syscall_hpp

#include <Execution/SimpleEventDriven/Syscall.hpp>

namespace SyscallIdentifiers
{
    static constexpr int SetEventHandler = 0;
    static constexpr int SendEvent = 1;
    static constexpr int EventHandlerReturn = 2;
    static constexpr int ReadSensor = 3;
    static constexpr int SendData = 4;
    static constexpr int Print = 5;
}

int sysReadSensor(int id);

size_t sysSendData(const void* bytes, size_t count);

void sysprintf(const char* format, ...);

#endif /* Syscall_hpp */
