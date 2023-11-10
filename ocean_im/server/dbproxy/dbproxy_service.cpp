#include "server/dbproxy/dbproxy_service.h"
#include <cstdio>
#include <sstream>
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(db_connect_info, "dbname=oceanim user=root", "消息数据库连接信息");
DEFINE_string(db_name, "oceanim", "数据库名字");

DEFINE_string(db_group_member_connect_info, "dbname=oceanim user=root", "群成员数据库连接信息");
DEFINE_string(db_group_member_name, "oceanim", "数据库名字");

DEFINE_string(redis_connection_type, "", "redis连接类型:single, pooled, short");
DEFINE_string(redis_server, "127.0.0.1:6379", "redis实例:端口");
DEFINE_int32(redis_timeout_ms, 1000, "redis超时时间,毫秒");
DEFINE_int32(redis_max_retry, 3, "redis最大重试次数");

namespace oceanim
{
    constexpr int kConnectNumEachDb = 10;

    DbproxyServiceImpl::DbproxyServiceImpl() : db_message_connect_pool_(kConnectNumEachDb), db_group_members_connect_pool_(kConnectNumEachDb)
    {
        for (int i = 0; i < kConnectNumEachDb; i++)
        {
            db_message_connect_pool_.at(i).open(FLAGS_db_name, FLAGS_db_connect_info);
        }

        for (int i = 0; i < kConnectNumEachDb; i++)
        {
            db_group_members_connect_pool_.at(i).open(FLAGS_db_group_member_name, FLAGS_db_group_member_connect_info);
        }

        brpc::ChannelOptions options;
        options.protocol = brpc::PROTOCOL_REDIS;
        options.connection_type = FLAGS_redis_connection_type;
        options.timeout_ms = FLAGS_redis_timeout_ms;
        options.max_retry = FLAGS_redis_max_retry;

        if (redis_channel_.Init(FLAGS_redis_server.c_str(), &options) != 0)
        {
            LOG(ERROR) << "初始化channel失败";
            exit(-1);
        }
    }

    DbproxyServiceImpl::~DbproxyServiceImpl() {}

    void DbproxyServiceImpl::Test(google::protobuf::RpcController *controller,
                                  const Ping *ping,
                                  Pong *pong,
                                  google::protobuf::Closure *done)
    {
        brpc::ClosureGuard done_guard(done);
        DLOG(INFO) << "running test";
    }

    void DbproxyServiceImpl::AuthAndSaveSession(google::protobuf::RpcController *controller,
                                                const SigninData *new_msg,
                                                Pong *pong,
                                                google::protobuf::Closure *done)
    {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller *pcntl = static_cast<brpc::Controller *>(controller);

        brpc::RedisRequest request;
        if (!request.AddCommand("SET {%ld}a %s EX 3600", new_msg->user_id(), new_msg->access_addr().c_str()))
        {
            LOG(ERROR) << "fail to add command";
        }

        brpc::RedisResponse response;
        brpc::Controller cntl;
        redis_channel_.CallMethod(NULL, &cntl, &request, &response, NULL);

        if (cntl.Failed())
        {
            LOG(ERROR) << "连接redis失败," << cntl.ErrorText();
            pcntl->SetFailed(cntl.ErrorCode(), cntl.ErrorText().c_str());
        }
        else
        {
            LOG(INFO) << "redis reply=" << response;
        }
    }

    void DbproxyServiceImpl::ClearSession(google::protobuf::RpcController *controller,
                                          const UserId *user_id,
                                          Pong *,
                                          google::protobuf::Closure *done)
    {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller *pcntl = static_cast<brpc::Controller *>(controller);
        brpc::RedisRequest request;

        if (!request.AddCommand("DEL {%ld}a", user_id->user_id()))
        {
            LOG(ERROR) << "fail to del command";
        }

        brpc::RedisResponse response;
        brpc::Controller cntl;
        redis_channel_.CallMethod(NULL, &cntl, &request, &response, NULL);

        if (cntl.Failed())
        {
            LOG(ERROR) << "Fail to del" << user_id->user_id() << ". " << cntl.ErrorText();
            pcntl->SetFailed(cntl.ErrorCode(), cntl.ErrorText().c_str());
        }
        else
        {
            DLOG(INFO) << "Redis reply=" << response;
        }
    }

    void DbproxyServiceImpl::SavePrivateMsg(google::protobuf::RpcController *controller,
                                            const NewPrivateMsg *new_msg,
                                            Reply *reply,
                                            google::protobuf::Closure *done)
    {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller *cntl = static_cast<brpc::Controller *>(controller);

        try
        {
            auto pool = ChooseDatabase(new_msg->sender());
            auto peer_pool = ChooseDatabase(new_msg->receiver());
            if (pool == peer_pool)
            {
                soci::session sql(*pool);
                sql << "INSERT INTO messages(user_id, sender, receiver, msg_id, group_id, message, client_time, msg_time) "
                       "VALUES (:user_id, :sender, :receiver, :msg_id, :group_id, :message, FROM_UNIXTIME(:client_time), FROM_UNIXTIME(:msg_time)), "
                       "(:receiver2, :sender2, :receiver2, :receiver_msg_id2, :group_id2, :message2, FROM_UNIXTIME(:client_time2), FROM_UNIXTIME(:msg_time2));",
                    soci::use(new_msg->sender()),
                    soci::use(new_msg->sender()),
                    soci::use(new_msg->receiver()),
                    soci::use(new_msg->sender_msg_id()),

                    soci::use(0),
                    soci::use(new_msg->message()),
                    soci::use(new_msg->client_time()),
                    soci::use(new_msg->msg_time()),

                    soci::use(new_msg->receiver()),
                    soci::use(new_msg->sender()),
                    soci::use(new_msg->receiver()),
                    soci::use(new_msg->receiver_msg_id()),

                    soci::use(0),
                    soci::use(new_msg->message()),
                    soci::use(new_msg->client_time()),
                    soci::use(new_msg->msg_time());
                LOG(INFO) << "sql=" << sql.get_query();
            }
            else
            {
                soci::session sql(*pool);
                sql << "INSERT INTO messages(user_id, sender,receiver,msg_id,group_id,message,client_time,msg_time) "
                       "VALUES (:user_id,:sender,:receiver,:msg_id,:group_id,:message, FROM_UNIXTIME(:client_time), FROM_UNIXTIME(:msg_time));",
                    soci::use(new_msg->sender()),
                    soci::use(new_msg->sender()),
                    soci::use(new_msg->receiver()),
                    soci::use(new_msg->sender_msg_id()),

                    soci::use(0),
                    soci::use(new_msg->message()),
                    soci::use(new_msg->client_time()),
                    soci::use(new_msg->msg_time());

                soci::session sql(*peer_pool);
                sql << "INSERT INTO messages(user_id, sender, receiver, msg_id, group_id, message, client_time, msg_time) "
                       "VALUES (:user_id, :sender, :receiver, :receiver_msg_id, :group_id, :message, FROM_UNIXTIME(:client_send), FROM_UNIXTIME(:msg_time));",
                    soci::use(new_msg->receiver()),
                    soci::use(new_msg->sender()),
                    soci::use(new_msg->receiver()),
                    soci::use(new_msg->receiver_msg_id()),

                    soci::use(0),
                    soci::use(new_msg->message()),
                    soci::use(new_msg->client_time()),
                    soci::use(new_msg->msg_time());
            }
        }
        catch (const soci::soci_error &e)
        {
            LOG(ERROR) << "Fail to insert int messages"
                       << "sender=" << new_msg->sender()
                       << "receiver=" << new_msg->receiver()
                       << "msg_id=" << new_msg->sender_msg_id()
                       << ". " << e.what();
            cntl->SetFailed(EINVAL, "Fail to insert to message.");
            return;
        }

        UserLastSendData user_last_send_data;
        user_last_send_data.set_user_id(new_msg->sender());
        user_last_send_data.set_client_time(new_msg->client_time());
        user_last_send_data.set_msg_time(new_msg->msg_time());
        user_last_send_data.set_msg_id(new_msg->sender_msg_id());

        SetUserLastSendData_(cntl, &user_last_send_data);
    }

    void DbproxyServiceImpl::SaveGroupMsg(google::protobuf::RpcController *controller,
                                          const NewGroupMsg *new_msg,
                                          Reply *reply,
                                          google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::SetUserLastSendData(google::protobuf::RpcController *controller,
                                                 const UserLastSendData *,
                                                 Pong *,
                                                 google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::GetUserLastSendData(google::protobuf::RpcController *controller,
                                                 const UserId *,
                                                 UserLastSendData *,
                                                 google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::GetSessions(google::protobuf::RpcController *controller,
                                         const UserIds *,
                                         Sessions *,
                                         google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::GetMsgs(google::protobuf::RpcController *controller,
                                     const MsgIdRange *msg_range,
                                     Msgs *msgs,
                                     google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::GetFriends(google::protobuf::RpcController *controller,
                                        const UserId *user_id,
                                        UserInfos *user_infos,
                                        google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::GetGroups(google::protobuf::RpcController *controller,
                                       const UserId *user_id,
                                       GroupInfos *group_infos,
                                       google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::GetGroupMembers(google::protobuf::RpcController *controller,
                                             const GroupId *group_id,
                                             UserInfos *user_infos,
                                             google::protobuf::Closure *done)
    {
    }

    void DbproxyServiceImpl::SetUserLastSendData_(brpc::Controller *pcntl, const UserLastSendData *user_last_send_data)
    {
        const char *cmd = "eval \""
                          "local a = redis.call('GET,KEYS[1])"
                          " if (a == false or tonumber(cjson.decode(a)[2]) < tonumber(ARGC[2])) then"
                          " redis.call('SETEX',KEYS[1],3600, cjson.encode({ARGV[1],ARGV[2],ARGV[3]}))"
                          "return 1"
                          " else"
                          "return 0"
                          " end\" ";

        std::ostringstream oss;
        oss << cmd << 1 << " "
            << "{" << user_last_send_data->user_id() << "}"
            << "u "
            << user_last_send_data->msg_id() << " "
            << user_last_send_data->client_time() << " "
            << user_last_send_data->msg_time();

        brpc::RedisRequest request;
        if (!request.AddCommand(oss.str()))
        {
            LOG(ERROR) << "Fail to add command";
        }

        brpc::RedisResponse response;
        brpc::Controller cntl;

        redis_channel_.CallMethod(NULL, &cntl, &request, &response, NULL);
        if (cntl.Failed())
        {
            LOG(ERROR) << "fail to access redis, " << cntl.ErrorText();
            pcntl->SetFailed(EINVAL, "fail to set user last send data");
        }
        else
        {
            LOG(INFO) << "redis reply=" << response;
        }
    }

} // namespace oceanim
