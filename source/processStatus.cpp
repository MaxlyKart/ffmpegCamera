#include "processStatus.h"

int processStatus::getCPUUsage() {
    FILETIME now;
    // process
    FILETIME pCreationTime;
    FILETIME pExitTime;
    FILETIME pKernelTime;
    FILETIME pUserTime;
    int64_t systemTime;
    int64_t nowTime;
    int64_t systemTimeDelta;
    int64_t timeDelta;

    GetSystemTimeAsFileTime(&now);
    // 进程创建时间，终止时间，内核模式上时间，用户模式时间
    if (!GetProcessTimes(GetCurrentProcess(), &pCreationTime, &pExitTime,
        &pKernelTime, &pUserTime))
    {  
        return -1;
    }
    systemTime = (fileTime2UTC(&pKernelTime) + fileTime2UTC(&pUserTime)) / ProcessorCount;
    nowTime = fileTime2UTC(&now);

    if (lastSysTime == -1) {
        lastSysTime = systemTime;
    }
    if (lastTime == -1) {
        lastTime = nowTime;
        return -1;
    }

    systemTimeDelta = systemTime - lastSysTime;
    timeDelta = nowTime - lastTime;
    lastTime = nowTime;
    lastSysTime = systemTime;

    return systemTimeDelta * 100 / timeDelta + 0.5;
}

int processStatus::getMemUsage(_OUT uint64_t *mem = NULL, _OUT uint64_t *vmem = NULL) {
    PROCESS_MEMORY_COUNTERS pMem;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pMem, sizeof(pMem))) {
        if (mem) {
            *mem = pMem.WorkingSetSize;
        }
        if (vmem) {
            *vmem = pMem.PagefileUsage;
        }
        return 0;
    }
    return -1;
}

int processStatus::getIOBytes(_OUT uint64_t *readBytes = NULL, _OUT uint64_t *writeBytes = NULL) {
    IO_COUNTERS ioCounter;
    if (GetProcessIoCounters(GetCurrentProcess(), &ioCounter)) {
        if (readBytes) {
            *readBytes = ioCounter.ReadTransferCount;
        }
        if (writeBytes) {
            *writeBytes = ioCounter.WriteTransferCount;
        }
        return 0;
    }
    return -1;
}

processStatus::processStatus() {
    lastSysTime = -1;
    lastTime = -1;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    ProcessorCount = sysInfo.dwNumberOfProcessors;
}

int64_t processStatus::fileTime2UTC(_IN const FILETIME *fileTime){
    LARGE_INTEGER li;  
  
    li.LowPart = fileTime->dwLowDateTime;
    li.HighPart = fileTime->dwHighDateTime;
    return li.QuadPart;
}