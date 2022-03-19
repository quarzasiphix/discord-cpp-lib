#include "discord++.hpp"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace Discord
{
    httplib::Headers headers;

    std::string uri = "wss://gateway.discord.gg/?v=9&encoding=json";

    size_t my_write_function(const void* indata, const size_t size, const size_t count, void* out) {
        (*(std::string*)out).append((const char*)indata, size * count);
        return size * count;
    }

    void socket_msg::send(clientpp* c, const char* text)
    {
        c->send(this->hdl, text, websocketpp::frame::opcode::text);
    }

    template <typename Out>
    void split(const std::string& s, char delim, Out result) {
        std::istringstream iss(s);
        std::string item;
        while (std::getline(iss, item, delim)) {
            *result++ = item;
        }
    }

    std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    bool contains(client::message* msg, std::string saas) {
        if (msg->content.find(saas) != std::string::npos && msg->content.find(saas) < (saas.size() + 1)) return true;
        else return false;
    }

    bool contains(std::string soos, std::string saas) {

        if (soos.find(saas) != std::string::npos) return true;
        else return false;
    }

    void heartbeat(discord* _disc)
    {
        nlohmann::json hb;
        hb["op"] = 1;
        hb["d"] = 45000;
        while (_disc->connected == true)
        {
            _disc->_msg.send(_disc->_msg.c, hb.dump().c_str());
            Sleep(10000);
        }
    }

    void discord::on_fail(clientpp* c, websocketpp::connection_hdl hdl, discord* _disc) {
        this->ConClose = true;
        this->connected = false;
        c->stop();
        on_invalid(_disc);
    }

    void discord::on_close(clientpp* c, websocketpp::connection_hdl hdl, discord* _disc) {
        this->ConClose = true;
        this->connected = false;
        std::cout << "closed connection" << std::endl;
        on_invalid(_disc);
    }

    void discord::on_message(clientpp* c, websocketpp::connection_hdl hdl, message_ptr msg) {
        nlohmann::json t = nlohmann::json::parse(msg->get_payload());
        if (contains(t["t"].dump(), "READY"))
        {
            std::thread(heartbeat, this).detach();
            if (t["op"].get<int>() == 0 && t["t"].get<std::string>() == "READY")
            {
                cli.id = t["d"]["user"]["id"].get<std::string>();
                cli.discriminator = t["d"]["user"]["discriminator"].get<std::string>();
                cli.username = t["d"]["user"]["username"].get<std::string>();
                if (!t["d"]["guild_id"].is_null())
                    for (int i = 0; i < t["d"]["guilds"].size(); i++)
                    {
                        cli.guilds.push_back(t["d"]["guilds"][i]["id"].get<std::string>());
                    }
                cli.initialised = true;
                if (on_login) on_login(cli);
            }
        }
        if (contains(t["t"].dump(), "MESSAGE_CREATE"))
        {
            cli.msg.author = t["d"]["author"]["username"].get<std::string>();
            cli.msg.discriminator = t["d"]["author"]["discriminator"].get<std::string>();
            cli.msg.content = t["d"]["content"].get<std::string>();
            cli.msg.channelid = t["d"]["channel_id"].get<std::string>();
            cli.msg.id = t["d"]["id"].get<std::string>();
            if (!t["d"]["guild_id"].is_null())
                cli.msg.guild_id = t["d"]["guild_id"].get<std::string>();
            cli.msg.param = split(cli.msg.content, ' ');
            on_msg(cli.msg);
        }
    }

    void discord::on_open(clientpp* c, websocketpp::connection_hdl hdl) {
        if (bot)
        {
            nlohmann::json t;
            t["op"] = 2;
            t["d"]["token"] = token;
            t["d"]["intents"] = 513;
            t["d"]["properties"]["$os"] = "linux";
            t["d"]["properties"]["$browser"] = "discordpp";
            t["d"]["properties"]["$device"] = "discordpp";
            c->send(hdl, t.dump() , websocketpp::frame::opcode::text);
        }
        else c->send(hdl, "{\"op\":2,\"d\":{\"token\":\"" + std::string(this->token) + "\",\"capabilities\":253,\"properties\":{\"os\":\"Windows\",\"browser\":\"Chrome\",\"device\":\"\",\"system_locale\":\"en-US\",\"browser_user_agent\":\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.45 Safari/537.36\",\"browser_version\":\"96.0.4664.45\",\"os_version\":\"10\",\"referrer\":\"\",\"referring_domain\":\"\",\"referrer_current\":\"\",\"referring_domain_current\":\"\",\"release_channel\":\"stable\",\"client_build_number\":108924,\"client_event_source\":null},\"compress\":false,\"client_state\":{\"guild_hashes\":{},\"highest_lastmessage_id\":\"0\",\"read_state_version\":0,\"user_guild_settings_version\":-1,\"user_settings_version\":-1}}}", websocketpp::frame::opcode::text);
        _msg.hdl = hdl;
        _msg.c = c;
        this->connected = true;
    }

    void discord::start(const char* token, bool bot)
    {
        threadClient = std::thread([=]() 
        {
            /*this->emb.color = "16758465";
            this->emb.image = "https://media.discordapp.net/attachments/771831923563298847/829894508426035210/619-6197837_kannakamui-cute-loli-dragon-purple-anime-misskobayashid-kunna.png?width=628&height=630";
            this->emb.title = "hello!";*/
            this->bot = bot;
            this->token = token;

            if(bot == true) 
                 headers.emplace("Authorization", "Bot " + std::string(token));
            else headers.emplace("Authorization", token);
            
            headers.emplace("User-Agent", "Discord");
    
            c.set_tls_init_handler (
                [this](websocketpp::connection_hdl) 
                {
                    return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
                }
            );
    
            try {
                c.init_asio();
                c.set_access_channels(websocketpp::log::alevel::none);
                c.set_message_handler(std::bind(&discord::on_message, this, &c, ::_1, ::_2)); //set event handlers, on_close and on_error are also possible.
                c.set_open_handler   (std::bind(&discord::on_open,    this, &c, ::_1));
                c.set_fail_handler   (std::bind(&discord::on_fail,    this, &c, ::_1, this));
                c.set_close_handler  (std::bind(&discord::on_close,   this, &c, ::_1, this));

                websocketpp::lib::error_code ec;
                clientpp::connection_ptr con = c.get_connection(uri, ec);
                if (ec) {
                    std::cout << "could not create connection because: " << ec.message() << std::endl;
                    return;
                }
                c.connect(con);
                this->initialised = true;
                c.run();
            }

            catch (nlohmann::json::exception& e)
            {
                MessageBoxA(nullptr, e.what(), nullptr, MB_OK);
            }
            catch (websocketpp::exception const& e) 
            {
                MessageBoxA(nullptr, e.what(), nullptr, MB_OK);
            }
            catch (...) 
            {
                MessageBoxA(nullptr, "unknown", nullptr, MB_OK);
            }

        });
        threadClient.join();

    }

    discord::~discord()
    {
        // nothing yet 
    }

    void discord::get_user(std::string id)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/users/" + id;
        auto res = cli.Get(url.c_str(), headers);
        nlohmann::json t = nlohmann::json::parse(res->body);
        fetched.id = t["id"].get<std::string>();
        fetched.username = t["username"].get<std::string>();
        fetched.discriminator = t["discriminator"].get<std::string>();
        fetched.avatar = t["avatar"].get<std::string>();
        cli.stop();
    }

    void discord::ban_user(client::message* msg, std::string id)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/guilds/" + msg->guild_id + "/bans/" + id;
        auto res = cli.Put(url.c_str(), headers, "{\"deletemessage_days\": \"1\"}", "application/json");
        cli.stop();
    }

    /*
    void discord::sendmessage(std::string channelid, std::string message)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/channels/" + channelid + "/messages";
        nlohmann::json t;
        t["content"] = message;
        auto res = cli.Post(url.c_str(), this->headers, t.dump(), "application/json");
        cli.stop();
    }*/

    void discord::del_message(std::string channelid, std::string messageid)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/channels/" + channelid + "/messages/" + channelid;
        auto res = cli.Delete(url.c_str(), headers);
        cli.stop();
    }

    void discord::del_message(client::message* msg, std::string messageid)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/channels/" + msg->channelid + "/messages/" + msg->id;
        auto res = cli.Delete(url.c_str(), headers);
        cli.stop();
    }

    /*
    void discord::onmessage(std::string channel, std::string message)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/channels/" + channel + "/messages";
        nlohmann::json t;
        t["content"] = message;
        auto res = cli.Post(url.c_str(), this->headers, t.dump(), "application/json");
        cli.stop();
    }

    void discord::respond(client::message* msg, std::string message)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/channels/" + msg->channelid + "/messages";
        nlohmann::json t;
        t["content"] = message;
        auto res = cli.Post(url.c_str(), headers, t.dump(), "application/json");
        cli.stop();
    }*/

    /*
    void discord::send_embed(std::string channel, std::string message)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v8/channels/" + channel + "/messages";
        nlohmann::json t;
        t["embed"]["description"] = message;
        t["embed"]["color"] = this->emb.color;
        t["embed"]["title"] = this->emb.title;
        t["embed"]["thumbnail"]["url"] = this->emb.image;
        auto res = cli.Post(url.c_str(), this->headers, t.dump(), "application/json");
        cli.stop();
    }*/

    void client::message::respond(client::message* msg, std::string message)
    {
        httplib::SSLClient cli("discord.com");
        std::string url = "https://discord.com/api/v9/channels/" + msg->channelid + "/messages";
        nlohmann::json t;
        t["content"] = message;
        auto res = cli.Post(url.c_str(), headers, t.dump(), "application/json");
        cli.stop();
    }
}