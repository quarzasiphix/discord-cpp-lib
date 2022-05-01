#pragma once
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/transport/asio/security/tls.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/function.hpp>
#include <nlohmann/json.hpp>

#include "functional"
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <httplib.h>
#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

#pragma comment(lib, "libs/libboost_date_time-vc142-mt-s-x64-1_76.lib")
#pragma comment(lib, "libs/libboost_random-vc142-mt-s-x64-1_76.lib")
#pragma comment(lib, "libs/libcurl_a.lib")
#pragma comment(lib, "libs/libssl.lib")

#include <thread>
#include <vector>


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

            void command(std::string command, void(*func)(client::message msg));

            std::vector<std::string> param;
            char prefix = '!';

            friend struct discord;
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
        bool is_valid;
        std::string id;
        std::string username;
        std::string discriminator;
        std::string avatar;
        std::string profile_url;

        std::string response;
    };

    class discord
    {
        void on_message(clientpp* c, websocketpp::connection_hdl hdl, message_ptr msg);
        void on_close(clientpp* c, websocketpp::connection_hdl hdl, discord* _disc);
        void on_open(clientpp* c, websocketpp::connection_hdl hdl);
        //void on_fail(clientpp* c, websocketpp::connection_hdl hdl, discord* _disc);
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
        bool get_user(std::string id);
        void ban_user(client::message* msg, std::string id);
        void start(const char* token, bool bot, void(*on_invalid)(discord* _disc), void(*on_msg)(client::message msg), void(*on_login)(client client));
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