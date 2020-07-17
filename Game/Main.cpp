#include <SFML/Graphics.hpp>

// Managers
#include "../Engine/States/StateManager.hpp"
#include "../Engine/Messages/MessageManager.hpp"
#include "../Engine/Entities/Entity/Entity.hpp"
#include "../Engine/Entities/Entity/EntityManager.hpp"
#include "../Engine/Entities/System/SystemManager.hpp"

// States
#include "Includes/State/SplashState.h"
#include "Includes/State/LoadingState.h"
#include "Includes/State/MenuState.h"
#include "Includes/State/PlayingState.h"
#include "Includes/State/PausedState.h"

// Messaging
#include "Includes/Message/StartGameMessage.h"
#include "Includes/Message/PauseGameMessage.h"
#include "Includes/Message/LoadStartMessage.h"
#include "Includes/Message/LoadProgressMessage.h"
#include "Includes/Message/LoadCompleteMessage.h"

// Systems
#include "Includes/System/KinematicSystem.h"
#include "Includes/System/RenderSystem.h"
#include "Includes/System/KeyboardControllerSystem.h"
#include "Includes/System/MouseControllerSystem.h"
#include "Includes/System/DebugSystem.h"
#include "Includes/System/CollisionResolverSystem.h"
#include "Includes/System/CollisionDetectionSystem.h"

#include "Includes/Factory/EntityFactory.h"
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
	auto fps = sf::Text();
	auto clock = sf::Clock();
	auto video = sf::VideoMode(800U, 600U);
	auto window = std::make_shared<sf::RenderWindow>(video, "", sf::Style::Titlebar | sf::Style::Close);

	// Managers startup
	auto messages = std::make_shared<mqs::MessageManager>();
	auto entities = std::make_shared<ecs::EntityManager>();
	auto systems = std::make_shared<ecs::SystemManager>(entities, messages);
	auto states = std::make_shared<sts::StateManager>(messages); // (systems, messages)

	// Resources startup: Fonts
	FontStore::load("Resources\\Fonts", [](const std::string& path) {
		sf::Font font;
		font.loadFromFile(path);
		return font;
	});

	// Resources startup: Textures
	TextureStore::load("Resources\\Images", [](const std::string& path) {
		sf::Texture texture;
		//texture.loadFromFile(path);
		return texture;
	});

	// Message hooking for debugging purposes
	messages->hook([](const mqs::Message& message) {
		//TRACE("Hooked message: id = %d, uid = %d, family = %d", message.id, message.uid(), message.family);
	});

	// State transition setup
	states->
		 map<SplashState>().onDone().go<MenuState>()
		.map<MenuState>()
			.onDone().go<LoadingState>()
			.onMessage<StartGameMessage>().go<LoadingState>()
		.map<LoadingState>().onDone().go<PlayingState>()
		.map<PlayingState>()
			.onMessage<LoadStartMessage>().go<LoadingState>()
			.onMessage<PauseGameMessage>().go<PausedState>()
		.map<PausedState>().onMessage<PauseGameMessage>().go<PlayingState>()
		.done();

	systems->add<MouseControllerSystem>(*window.get());
	systems->add<KeyboardControllerSystem>();
	systems->add<KinematicSystem>();
	systems->add<CollisionDetectionSystem>();
	systems->add<CollisionResolverSystem>();
	systems->add<RenderSystem>(window);
	systems->add<DebugSystem>(window);
	
	auto player = entities->create();

	player.assign<KeyboardController>();
	//player.assign<MouseController>();
	player.assign<Motion>();
	player.assign<Body>(30.f);
	player.assign<Transform>(400.f, 300.f);
	player.assign<Render>(sf::Color::Red);

	// SFML window setup
	window->setMouseCursorVisible(true);
	window->setKeyRepeatEnabled(true);
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	fps.setFont(FontStore::get("Carlito.ttf"));
	fps.setCharacterSize(16U);
	fps.setPosition(5.f, 5.f);

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
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Left:
				{
					auto e = entities->create();
					e.assign<Body>(rand() % 50 + 30);
					e.assign<Motion>(); // rand() % 10 + 10);
					e.assign<Render>(sf::Color(rand() % 255, rand() % 255, rand() % 255));
					e.assign<Transform>(event.mouseButton.x, event.mouseButton.y);
				}
					break;
				case sf::Mouse::Right:
					break;
				}
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
					messages->publish<PauseGameMessage>();
					break;
				case sf::Keyboard::Space:
					window->close();
					break;
				}
				break;
			}
		}

		float delta = clock.restart().asSeconds();

		window->clear(sf::Color(128, 128, 128));
		systems->update(delta);
		states->update(delta);
		fps.setString(std::to_string(1.f / delta));
		window->draw(fps);
		window->display();
	}

	return 0;
}