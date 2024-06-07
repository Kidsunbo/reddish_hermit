#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <common/utils/types.h>

namespace reddish::common::network
{
    using namespace reddish::common::utils;

    /*
    This class used to provide a simple wrapper of boost::asio. It supports timeout and connection management.    
    Notice that you should use the connection in the thread running the same io_context binding to the connection.
    */
    class Connection{
 
        boost::asio::any_io_executor ctx;
        boost::asio::ip::tcp::socket sock;
        int timeout;
        boost::asio::steady_timer timer;
        std::string data;
        Buffer buf;

        public:
        /*
        Constructor
        accept the any_io_executor and timeout parameter. If timeout is zero, there will be no timeout being configured.
        */
        explicit Connection(boost::asio::any_io_executor ctx, int timeout=0);
        
        /*
        Delegation Constructor
        accept the left value of io_context and timeout parameter. Then calls the other constructor.
        */
        explicit Connection(boost::asio::io_context& ctx, int timeout=0);
        
        /*
        Delegation Constructor
        accept the right value of io_context and timeout parameter. Then calls the other constructor.
        */
        explicit Connection(boost::asio::io_context&& ctx, int timeout=0);

        /*
        Constructor accepts a connected socket
        */
        Connection(boost::asio::any_io_executor ctx, boost::asio::ip::tcp::socket&& sock, int timeout = 0);

        /*
        Connect to the remote with hostname and port. This will first connect to DNS to fetch the ip address
        of the remote server.
        */
        [[nodiscard]] AsyncResult<boost::asio::ip::tcp::endpoint> connect_with_host_name(std::string_view host, unsigned short port);
        
        /*
        Connect to the remote with ip and port. This will first connect to DNS to fetch the ip address
        of the remote server.
        */
        [[nodiscard]] AsyncResult<boost::asio::ip::tcp::endpoint> connect_with_ip(const std::string& ip, unsigned short port);

        /*
        Write to the connection with bunch of bytes. The bytes is represented by string_view.
        The return value represents how many bytes are written successfully.
        */
        [[nodiscard]] AsyncResult<std::size_t> write(std::string_view value);
        
        /*
        Read from the connection until the pattern sep is met.
        Notice the Buffer is of boost::asio::dynamic_string_buffer.
        The return value represents how many bytes are read successfully.
        */
        [[nodiscard]] AsyncResult<std::string> read_until(std::string_view sep);
        
        /*
        Read from the connection until the pattern sep is met.
        Notice the Buffer is of boost::asio::dynamic_string_buffer.
        The return value represents how many bytes are read successfully.
        */
        [[nodiscard]] AsyncResult<std::string> read_exact(std::uint64_t size);
        
        /*
        Close the connection. It will try to shutdown both side and then close the socket.
        */
        [[nodiscard]] boost::system::error_code close();

        /*
        Reset the buffer and clean all the unread data.
        */
        void reset_buffer();

        /*
        The destructor. If the socket is still open, it will close it.
        */
        ~Connection()noexcept;
    };
}