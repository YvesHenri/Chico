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

#include "Includes/Resource/ResourceStore.hpp"

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
	// SFML startup
	auto clock = sf::Clock();
	auto video = sf::VideoMode(800U, 600U);
	auto window = std::make_shared<sf::RenderWindow>(video, "Chico", sf::Style::Titlebar | sf::Style::Close);

	// Managers startup
	auto messages = std::make_shared<mqs::MessageManager>();
	auto entities = std::make_shared<ecs::EntityManager>();
	auto states = std::make_shared<sts::StateManager>(messages);
	auto systems = std::make_shared<ecs::SystemManager>(entities, messages);

	// Resources startup: Fonts
	FontStore::load("Resources\\Fonts", [](const std::string& path) {
		sf::Font font;
		font.loadFromFile(path);
		return font;
	});

	// Resources startup: Textures
	TextureStore::load("Resources\\Images", [](const std::string& path) {
		sf::Texture texture;
		texture.loadFromFile(path);
		return texture;
	});

	// Message hooking for debugging purposes
	messages->hook([](const mqs::Message& message) {
		TRACE("Hooked message: id = %d, uid = %d, family = %d", message.id, message.uid(), message.family);
	});

	// State transition setup
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

	auto windowSize = window->getSize();
	auto text = sf::Text();

	text.setCharacterSize(60);
	text.setFont(FontStore::get("stocky.ttf"));
	text.setString("Ola");
	text.setColor(sf::Color::Black);

	auto textRect = text.getLocalBounds();

	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
	
	window->setMouseCursorVisible(true);
	window->setKeyRepeatEnabled(true);
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	while (window->isOpen())
	{
		sf::Event event;

		while (window->pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window->close();
				break;
			// Debugging
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Return:
					messages->publish<StartGameMessage>();
					break;
				case sf::Keyboard::Num2:
					break;
				case sf::Keyboard::Num3:
					break;
				case sf::Keyboard::Num4:
					break;
				case sf::Keyboard::Escape:
					states->current<PausedState>() ? messages->publish<UnpauseGameMessage>() : messages->publish<PauseGameMessage>();
					break;
				case sf::Keyboard::Space:
					window->close();
					break;
				}
				break;
			}
		}

		states->update(clock.restart().asSeconds());
		window->clear(sf::Color(128, 128, 128));
		window->draw(text);
		window->display();
	}

	return 0;
}