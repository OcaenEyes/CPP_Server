#include "util/initialize.h"

namespace oceanim
{
    Initialize::Initialize(int &argc, char **argv[])
    {
        google::InitGoogleLogging((*argv)[0]);
        google::SetStderrLogging(google::GLOG_INFO);
        FLAGS_colorlogtostderr = true;

        gflags::ParseCommandLineFlags(&argc, argv, true);

        LOG(INFO) << "Initialize";
        std::srand(std::time(nullptr));
    }

    Initialize::~Initialize()
    {
        LOG(INFO) << "~Initialize";
        google::ShutdownGoogleLogging();
    }
} // namespace oceanim
