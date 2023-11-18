#include "server/logic/logic_service.h"
#include "server/idgen/idgen.pb.h"
#include "server/common/messages.pb.h"
#include "type.h"

#include <vector>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include "bthread/bthread.h"
#include "brpc/closure_guard.h"
#include "brpc/controller.h"
#include "brpc/options.pb.h"

DEFINE_int32(access_max_retry, 3, "max retried(not including thi first rpc)");
DEFINE_string(access_connection_type, "single", "Connection type. Available values: single, pooled, short");
DEFINE_int32(access_timeout_ms, 100, "rpc timeout in millseconds");

namespace
{
    struct SendtoPeersArgs
    {
        oceanim::MsgIdRequest id_request;
        oceanim::MsgIdReply id_reply;
        oceanim::NewMsg new_msg;
        oceanim::user_id_t sender;
        oceanim::LogicServiceImpl *this_;
    };
} // namespace name

namespace oceanim
{
    class SendToAccessClosure : public ::google::protobuf::Closure
    {
    public:
        SendToAccessClosure(brpc::Controller *cntl, Pong *pong) : cntl_(cntl), pong_(pong) {}

        void Run() override
        {
            DLOG(INFO) << "running SendToAccessClosure";
            if (cntl_->Failed())
            {
                DLOG(ERROR) << "fail to call SendToAccess." << cntl_->ErrorText();
            }
            delete this;
        }

        ~SendToAccessClosure()
        {
            delete cntl_;
            delete pong_;
        }

    private:
        brpc::Controller *cntl_;
        Pong *pong_;
    };

    LogicServiceImpl::LogicServiceImpl(brpc::Channel *id_channel, brpc::Channel *db_channel)
        : id_channel_(id_channel), db_channel_(db_channel) {}

    LogicServiceImpl::~LogicServiceImpl()
    {
        DLOG(INFO) << "calling AccessServiceImpl dtor";
    }

    void LogicServiceImpl::Test(google::protobuf::RpcController *controller, const Ping *ping, Pong *pong, google::protobuf::Closure *done)
    {
        brpc::ClosureGuard done_guard(done);
        DLOG(INFO) << "running test";
    }

    void LogicServiceImpl::SendMsg(google::protobuf::RpcController *controller, const NewMsg *new_msg, MsgReply *reply, google::protobuf::Closure *done)
    {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller *cntl = static_cast<brpc::Controller *>(controller);

        const oceanim::user_id_t user_id = new_msg->user_id();
        DLOG(INFO) << "received request[log_id=" << cntl->log_id()
                   << "] from" << cntl->remote_side()
                   << "to" << cntl->local_side()
                   << "user_id=" << user_id
                   << "peer_id=" << new_msg->peer_id()
                   << "client_time=" << new_msg->client_time()
                   << "msg_type=" << new_msg->msg_type()
                   << "message=" << new_msg->message()
                   << "(attached=" << cntl->request_attachment() << ")";
    }

    void LogicServiceImpl::PullData(google::protobuf::RpcController *controller, const Ping *ping, Msgs *msgs, google::protobuf::Closure *done)
    {
    }

    void LogicServiceImpl::GetMsgs(google::protobuf::RpcController *controller, const MsgIdRange *msg_range, Msgs *msgs, google::protobuf::Closure *done)
    {
    }

    void LogicServiceImpl::GetFriends(google::protobuf::RpcController *controller, const UserId *user_id, UserInfos *user_infos, google::protobuf::Closure *done)
    {
    }

    void LogicServiceImpl::GetGroups(google::protobuf::RpcController *controller, const UserId *user_id, GroupInfos *group_infos, google::protobuf::Closure *done)
    {
    }

    void LogicServiceImpl::GetGroupMembers(google::protobuf::RpcController *controller, const GroupId *group_id, UserInfos *user_infos, google::protobuf::Closure *done)
    {
    }

    void *LogicServiceImpl::SendtoPeers(void *args)
    {
    }

} // namespace oceanim
