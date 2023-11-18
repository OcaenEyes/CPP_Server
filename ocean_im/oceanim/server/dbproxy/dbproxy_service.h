#ifndef __OCEANIM_DBPROXY_FBPROXY_SERVICE_H__
#define __OCEANIM_DBPROXY_FBPROXY_SERVICE_H__

#include "dbproxy.pb.h"
#include "type.h"

#include "brpc/channel.h"
#include "brpc/redis.h"
#include <gflags/gflags.h>
#include <glog/logging.h>
// #include <soci/soci.h>
#include "soci/soci.h"

namespace brpc
{
    class Channel;
    class Controller;
} // namespace brpc

namespace oceanim
{
    class DbproxyServiceImpl : public DbProxyService
    {
    public:
        explicit DbproxyServiceImpl();
        ~DbproxyServiceImpl();

        void Test(google::protobuf::RpcController *controller,
                  const Ping *ping,
                  Pong *pong,
                  google::protobuf::Closure *done) override;

        void AuthAndSaveSession(google::protobuf::RpcController *controller,
                                const SigninData *new_msg,
                                Pong *pong,
                                google::protobuf::Closure *done) override;

        void ClearSession(google::protobuf::RpcController *controller,
                          const UserId *user_id,
                          Pong *,
                          google::protobuf::Closure *done) override;

        void SavePrivateMsg(google::protobuf::RpcController *controller,
                            const NewPrivateMsg *new_msg,
                            Reply *reply,
                            google::protobuf::Closure *done) override;

        void SaveGroupMsg(google::protobuf::RpcController *controller,
                          const NewGroupMsg *new_group_msg,
                          Reply *reply,
                          google::protobuf::Closure *done) override;

        void SetUserLastSendData(
            google::protobuf::RpcController *controller,
            const UserLastSendData *user_last_send_data,
            Pong *,
            google::protobuf::Closure *done) override;

        void GetUserLastSendData(
            google::protobuf::RpcController *controller,
            const UserId *userid,
            UserLastSendData *user_last_send_data,
            google::protobuf::Closure *done) override;

        void GetSessions(
            google::protobuf::RpcController *controller,
            const UserIds *user_ids,
            Sessions *sessions,
            google::protobuf::Closure *done) override;

        void GetMsgs(google::protobuf::RpcController *controller,
                     const MsgIdRange *msg_range,
                     Msgs *msgs,
                     google::protobuf::Closure *done) override;

        void GetFriends(
            google::protobuf::RpcController *controller,
            const UserId *userid,
            UserInfos *user_infos,
            google::protobuf::Closure *done) override;

        void GetGroups(
            google::protobuf::RpcController *controller,
            const UserId *userid,
            GroupInfos *group_infos,
            google::protobuf::Closure *done) override;

        void GetGroupMembers(
            google::protobuf::RpcController *controller,
            const GroupId *groupid,
            UserInfos *user_infos,
            google::protobuf::Closure *done) override;

    private:
        void SetUserLastSendData_(brpc::Controller *cntl, const UserLastSendData *user_last_send_data);

        soci::connection_pool *ChooseDatabase(user_id_t user_id)
        {
            return &db_message_connect_pool_;
        }

        soci::connection_pool db_message_connect_pool_;

        soci::connection_pool db_group_members_connect_pool_;
        brpc::Channel redis_channel_;
    };
} // namespace oceanim

#endif