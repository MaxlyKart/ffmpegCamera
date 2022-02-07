#ifndef PROCESS_STATUS
#define PROCESS_STATUS

#include "windows.h"
#include "psapi.h"
#include "typeAInclude.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef long long int64_t;
typedef unsigned long long uint64_t;

class processStatus {
private:
    int ProcessorCount;
    int64_t lastTime;
    int64_t lastSysTime;

    int64_t fileTime2UTC(const FILETIME *fileTime);

public:
    processStatus();
    int getCPUUsage();
    /**
     * @brief 获取内存使用情况
     * 
     * @param mem 内存
     * @param vmem 虚拟内存
     * @return int 
     */
    int getMemUsage(_OUT uint64_t *mem, _OUT uint64_t *vmem);
    /**
     * @brief IO读写情况
     * 
     * @param readBytes 
     * @param writeBytes 
     * @return int 
     */
    int getIOBytes(_OUT uint64_t *readBytes, _OUT uint64_t *writeBytes);

};

#ifdef __cplusplus
}
#endif

#endif



