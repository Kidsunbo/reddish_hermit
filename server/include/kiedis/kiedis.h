#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <memory>
#include <string_view>

struct KiedisServerConfig
{
    std::uint16_t port = 6379;
    std::string_view role = "master";
    std::uint16_t connected_slaves = 0;
    std::string master_replid = "";
    std::uint16_t master_repl_offset = 0;
};

class KiedisServer
{

    struct private_dummy{};

    KiedisServerConfig config;

    static std::shared_ptr<KiedisServer> instance_ptr;

  private:
    KiedisServer() = default;

  public:
    static std::shared_ptr<KiedisServer> &instance();

    explicit KiedisServer(private_dummy){}

    void parse_config_from_command_line(int argc, char** argv);
    const KiedisServerConfig &get_config();

    boost::asio::awaitable<void> run();
};