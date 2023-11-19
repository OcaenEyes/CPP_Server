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
        Initialize(int &argc, char **argv[])
        {
            google::InitGoogleLogging((*argv)[0]);
            google::SetStderrLogging(google::GLOG_INFO);
            FLAGS_colorlogtostderr = true;
            gflags::ParseCommandLineFlags(&argc, argv, true);

            LOG(INFO) << "Initialize";
            std::srand(std::time(nullptr));
        }
        ~Initialize()
        {
            LOG(INFO) << "~Initialize";
            google::ShutdownGoogleLogging();
        }
    };

} // namespace oceanim

#endif