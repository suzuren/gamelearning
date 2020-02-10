#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg)
{
    std::string const& str_msg_payload = msg->get_payload();
    websocketpp::frame::opcode::value opcode_value = msg->get_opcode();
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << str_msg_payload << " opcode_value:"<< opcode_value
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

void on_open(server* s, websocketpp::connection_hdl hdl) {
    std::cout << "on_open called with hdl: " << hdl.lock().get()
        << std::endl;
}


void on_close(server* s, websocketpp::connection_hdl hdl) {
    std::cout << "on_close called with hdl: " << hdl.lock().get()
        << std::endl;
}

/*
[root@localhost build]# ./testWebSocketServer
[2020-02-10 13:21:30] [connect] WebSocket Connection [::ffff:127.0.0.1]:43336 v13 "Mozilla/5.0 (X11; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0" / 101
on_open called with hdl: 0xb995b0
on_message called with hdl: 0xb995b0 and message: 111 opcode_value:1
[2020-02-10 13:21:43] [frame_header] Dispatching write containing 1 message(s) containing 2 header bytes and 3 payload bytes
[2020-02-10 13:21:43] [frame_header] Header Bytes:
[0] (2) 81 03

on_message called with hdl: 0xb995b0 and message: 222 opcode_value:1
[2020-02-10 13:21:48] [frame_header] Dispatching write containing 1 message(s) containing 2 header bytes and 3 payload bytes
[2020-02-10 13:21:48] [frame_header] Header Bytes:
[0] (2) 81 03

[2020-02-10 13:21:50] [control] Control frame received with opcode 8
[2020-02-10 13:21:50] [frame_header] Dispatching write containing 1 message(s) containing 2 header bytes and 2 payload bytes
[2020-02-10 13:21:50] [frame_header] Header Bytes:
[0] (2) 88 02

on_close called with hdl: 0xb995b0
[2020-02-10 13:21:50] [disconnect] Disconnect close local:[1000] remote:[1005]

*/
int main() {
    // Create a server endpoint
    server echo_server;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        echo_server.set_open_handler(bind(&on_open, &echo_server, ::_1));
        echo_server.set_close_handler(bind(&on_close, &echo_server, ::_1));

        // Register our message handler
        echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));


        // Listen on port 9002
        echo_server.listen(9002);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}
