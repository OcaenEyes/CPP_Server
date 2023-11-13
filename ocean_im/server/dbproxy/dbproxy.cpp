#include "server/dbproxy/dbproxy_service.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <brpc/server.h>
#include <brpc/channel.h>
#include <butil/status.h>

#include "util/initialize.h"
#include "type.h"

DEFINE_int32(port, 7000, "TCP Port of this server");
DEFINE_int32(idle_timeout_s, -1, "connection will be closed if there is no "
                                 "read/write operations during the last `idle_timeout_s`");
DEFINE_int32(logoff_ms, 2000, "Maxinum duration of server's LOGOFF state "
                              "<Wairing for client th close connection before server stops)");

DEFINE_string(id_server_addr, "127.0.0.1:9000", "Id server address");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");

DEFINE_string(connection_type, "single", "Connection type. Available values: single,pooled,short");
DEFINE_int32(timeout_ms, 100, "RPC timeout is millseconds");

DEFINE_string(db_addr, "127.0.0.1:8000", "Id server address");

int main(int argc, char *argv[])
{
    oceanim::Initialize init(argc, &argv);
    oceanim::DbproxyServiceImpl dbproxy_service_impl;

    brpc::Server server;
    if (server.AddService(&dbproxy_service_impl, brpc::SERVER_DOESNT_OWN_SERVICE) != 0)
    {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    brpc::ServerOptions server_options;
    server_options.idle_timeout_sec = FLAGS_idle_timeout_s;
    if (server.Start(FLAGS_port, &server_options) != 0)
    {
        LOG(ERROR) << "Fail to start access";
        return -1;
    }

    server.RunUntilAskedToQuit();
    return 0;
}