# Discord bot library in c++
currently in a very early release stage
---
### Libraries used: 
  - [Websocketpp](https://github.com/zaphoyd/websocketpp)
  - [Nlohmann json](https://github.com/nlohmann/json)
  - [Boost](https://www.boost.org/)
  - [Curl](https://curl.se/)
---

## How to use
  - #### i highly advise to compile boost and curl manually to avoid issues
  - Currently only supports release x64 and uses std c++ 20. support for other releases will be added in future versions
  - precompiled X64, release libraries of Boost and Curl are already included in the project files ```discord++/libs```
     
    
Included example: 
```c++
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
```
  
  
  
