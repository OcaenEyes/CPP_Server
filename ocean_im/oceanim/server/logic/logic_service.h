/*
 * @Author: OCEAN.GZY
 * @Date: 2023-11-19 14:38:22
 * @LastEditors: OCEAN.GZY
 * @LastEditTime: 2023-11-19 20:17:39
 * @FilePath: /ocean_im/oceanim/server/logic/logic_service.h
 * @Description: 注释信息
 */
#ifndef __OCEANIM_LOGIC_LOGIC_SERVICE_H__
#define __OCEANIM_LOGIC_LOGIC_SERVICE_H__
#include "logic/logic.pb.h"
#include "type.h"

#include <mutex>
#include <unordered_map>

#include "brpc/channel.h"
#include "bthread/unstable.h"

namespace brpc
{
    class Channel;
    class Controller;
} // namespace brpc

namespace butil
{
    class Status;
} // namespace butil

namespace oceanim
{
    class LogicServiceImpl : public oceanim::LogicService
    {
    public:
        LogicServiceImpl(brpc::Channel *id_channel, brpc::Channel *db_channel);
        virtual ~LogicServiceImpl();

        void Test(google::protobuf::RpcController *controller, const Ping *ping, Pong *pong, google::protobuf::Closure *done) override;

        void SendMsg(google::protobuf::RpcController *controller, const NewMsg *new_msg, MsgReply *reply, google::protobuf::Closure *done) override;

        void PullData(google::protobuf::RpcController *controller, const Ping *ping, Msgs *msgs, google::protobuf::Closure *done) override;

        void GetMsgs(google::protobuf::RpcController *controller, const MsgIdRange *msg_range, Msgs *msgs, google::protobuf::Closure *done) override;

        void GetFriends(google::protobuf::RpcController *controller, const UserId *user_id, UserInfos *user_infos, google::protobuf::Closure *done) override;

        void GetGroups(google::protobuf::RpcController *controller, const UserId *user_id, GroupInfos *group_infos, google::protobuf::Closure *done) override;

        void GetGroupMembers(google::protobuf::RpcController *controller, const GroupId *group_id, UserInfos *user_infos, google::protobuf::Closure *done) override;

    private:
        static void *SendtoPeers(void *args);

        brpc::Channel *id_channel_;
        brpc::Channel *db_channel_;

        std::mutex access_map_mtx_;
        std::unordered_map<std::string, brpc::Channel> access_map_;
    };
} // namespace oceanim

#endif