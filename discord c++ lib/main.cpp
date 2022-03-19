#include "../discord++/discord++.hpp"
#include <iostream>

using namespace Discord;

void on_login(client client)
{
	std::cout << "welcome " + client.username + "#" + client.discriminator << std::endl;
}

void on_invalid(discord* _discord)
{
	std::cout << "invalid token";
	std::cin.get();
}

bool prefix(client::message& msg)
{
	if (msg.content[0] == msg.prefix)
	{
		if (msg.param[0].starts_with(msg.prefix))
			msg.param[0].replace(0, 1, "");
		return true;
	}
	else return false;
}

void on_msg(client::message msg)
{
	if (prefix(msg) == false) return;
	if (prefix(msg) == true && msg.param[0] == "ping") {
		msg.respond(&msg, "pong");
		std::cout << "pinged!" << std::endl;
	}
	if (prefix(msg) == true && msg.param[0] == "pong") {
		msg.respond(&msg, "ping");
		std::cout << "ponged!" << std::endl;
	}
}

int main()
{
	const char* token = "token";

	discord bot;
	bot.on_invalid = on_invalid;
	bot.on_login = on_login;
	bot.on_msg = on_msg;

	bot.start(token, true);
}