#pragma once
#include <boost/asio/ssl.hpp>
#include <boost/function.hpp>
#include "include/websocketpp/config/asio_client.hpp"
#include "include/websocketpp/client.hpp"
#include "include/websocketpp/transport/asio/security/tls.hpp"
#include "include/nlohmann/json.hpp"
#include "functional"
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "include/httplib.h"
#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

#include <thread>
#include <vector>
#include <iostream>

typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace Discord
{
    #define clientpp websocketpp::client<websocketpp::config::asio_tls_client>

    struct client
    {
        bool initialised;
        std::string prefix;
        std::string id;
        std::string username;
        std::string discriminator;

        std::vector<std::string> guilds;

        struct message
        {
            std::string response;
            std::string author;
            std::string discriminator;
            std::string content;
            std::string channelid;
            std::string id;
            std::string guild_id;

            void respond(client::message* msg, std::string message);

            std::vector<std::string> param;
            char prefix = '!';
        };

        message msg;

        struct friends
        {
            std::vector<std::string> name;
        };
    };

    struct socket_msg
    {
        websocketpp::connection_hdl hdl;
        clientpp* c;

        void send(clientpp* c, const char* message);
    };

    struct embed
    {
        std::string color;
        std::string title;
        std::string image;
    };

    struct fetched_user
    {
        std::string id;
        std::string username;
        std::string discriminator;
        std::string avatar;
    };

    class discord
    {
        void on_message(clientpp* c, websocketpp::connection_hdl hdl, message_ptr msg);
        void on_close(clientpp* c, websocketpp::connection_hdl hdl, discord* _disc);
        void on_open(clientpp* c, websocketpp::connection_hdl hdl);
        void on_fail(clientpp* c, websocketpp::connection_hdl hdl, discord* _disc);
        clientpp c;
    public:
        const char* token;
        bool bot;

        bool initialised;
        bool connected;
        bool ConClose;
        bool retry;

        void (*on_invalid)(discord* _disc);
        void (*on_msg)(client::message msg);
        void (*on_login)(client client);

        //void send_message(std::string channelid, std::string message);
        //void send_embed(std::string channel, std::string message);

        void del_message(client::message* msg, std::string messageid);
        void del_message(std::string channelid, std::string messageid);
        void get_user(std::string id);
        void ban_user(client::message* msg, std::string id);
        void start(const char* token, bool bot);
        ~discord();
        std::thread threadClient;
        socket_msg _msg;
        fetched_user fetched;
        embed emb;
        client cli;
    };

    extern bool contains(std::string soos, std::string saas);
    extern bool contains(client::message* msg, std::string saas);
}