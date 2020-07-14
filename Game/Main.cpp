#include <SFML/Graphics.hpp>

#include "../Engine/States/StateManager.hpp"
#include "../Engine/Messages/MessageManager.hpp"
#include "../Engine/Entities/Entity/Entity.hpp"
#include "../Engine/Entities/Entity/EntityManager.hpp"
#include "../Engine/Entities/System/SystemManager.hpp"

#include "Includes/State/SplashState.h"
#include "Includes/State/LoadingState.h"
#include "Includes/State/MenuState.h"
#include "Includes/State/PlayingState.h"
#include "Includes/State/PausedState.h"

#include "Includes/Message/StartGameMessage.h"
#include "Includes/Message/PauseGameMessage.h"
#include "Includes/Message/UnpauseGameMessage.h"
#include "Includes/Message/LoadStartMessage.h"
#include "Includes/Message/LoadProgressMessage.h"
#include "Includes/Message/LoadCompleteMessage.h"

#ifdef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:console")
#define ENTRY_POINT int main()
#else
#pragma comment(linker, "/SUBSYSTEM:windows")
//#define ENTRY_POINT int __stdcall WinMain(void*, void*, char*, int)
#define ENTRY_POINT int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif

ENTRY_POINT
{
	sf::Clock clock;
	sf::VideoMode video(800U, 600U);
	sf::RenderWindow window(video, "Chico - Spacebar to close");
	//auto videoo = std::make_shared<sf::VideoMode>(800U, 600U);
	//auto windoww = std::make_shared<sf::RenderWindow>(video, "Chico");

	auto messages = std::make_shared<mqs::MessageManager>();
	auto entities = std::make_shared<ecs::EntityManager>();
	auto states = std::make_shared<sts::StateManager>(messages);
	auto systems = std::make_shared<ecs::SystemManager>(entities, messages);

	window.setMouseCursorVisible(true);
	window.setKeyRepeatEnabled(true);
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(60);

	states->
		 map<SplashState>().onFinished().go<MenuState>()
		.map<MenuState>()
			.onFinished().go<LoadingState>()
			.onMessage<StartGameMessage>().go<LoadingState>()
		.map<LoadingState>().onFinished().go<PlayingState>()
		.map<PlayingState>()
			.onMessage<LoadStartMessage>().go<LoadingState>()
			.onMessage<PauseGameMessage>().go<PausedState>()
		.map<PausedState>().onMessage<UnpauseGameMessage>().go<PlayingState>()
		.done();

	messages->hook([](const mqs::Message& message) {
		//TRACE("Hooked message (uid = %d)", message.uid());
	});

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			// Debugging
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Return:
					messages->publish<StartGameMessage>();
					break;
				case sf::Keyboard::Num2:
					window.close();
					break;
				case sf::Keyboard::Num3:
					window.close();
					break;
				case sf::Keyboard::Num4:
					window.close();
					break;
				case sf::Keyboard::Escape:
					states->current<PausedState>() ? messages->publish<UnpauseGameMessage>() : messages->publish<PauseGameMessage>();
					break;
				case sf::Keyboard::Space:
					window.close();
					break;
				}
				break;
			}
		}

		states->update(clock.restart().asSeconds());
		window.clear(sf::Color(128, 128, 128));
		window.display();
	}

	return 0;
}