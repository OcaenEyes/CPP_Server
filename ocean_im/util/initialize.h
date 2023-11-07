#ifndef __OCEANIM_UTIL_INITIALIZE_H__
#define __OCEANIM_UTIL_INITIALIZE_H__

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstdlib>
#include <ctime>

namespace oceanim
{
    class Initialize final
    {
    public:
        Initialize(int &argc, char **argv[]);
        ~Initialize();
    };

} // namespace oceanim

#endif