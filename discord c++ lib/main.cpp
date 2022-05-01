#include "../discord++/discord++.hpp"

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

void on_msg(client::message msg)
{

	msg.command("test", [](client::message msg) {
		msg.respond(&msg, "received test");
		std::cout << "received test" << std::endl;
	});

	msg.command("pong", [](client::message msg) {
		msg.respond(&msg, "ping");
		std::cout << "received pong" << std::endl;
	});

	msg.command("ping", [](client::message msg) {
		msg.respond(&msg, "pong");
		std::cout << "received ping" << std::endl;
	});

	msg.command("user", [](client::message msg) {
		discord disc;
		if (disc.get_user(msg.param[1]))
			msg.respond(&msg, std::string("```"
				"name: " + disc.fetched.username + "#" + disc.fetched.discriminator + "\n" +
				"id: " + disc.fetched.id + "\n" + "```" +
				disc.fetched.profile_url + "\n"
			));
		else msg.respond(&msg, "```invalid id```");
	});
}

int main()
{
	const char* sbtoken = "token";

	discord sbot;
	sbot.start(sbtoken, false, on_invalid, on_msg, on_login);

	std::cout << "press enter to stop client" << std::endl;
	std::cin.get();
}