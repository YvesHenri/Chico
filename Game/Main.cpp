#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>

// Managers
#include <Engine/States/StateManager.hpp>
#include <Engine/Messages/MessageManager.hpp>
#include <Engine/Entities/Entity/Entity.hpp>
#include <Engine/Entities/Entity/EntityManager.hpp>
#include <Engine/Entities/System/SystemManager.hpp>

// States
#include "Includes/State/SplashState.h"
#include "Includes/State/LoadingState.h"
#include "Includes/State/MenuState.h"
#include "Includes/State/PlayingState.h"
#include "Includes/State/PausedState.h"

// Messaging
#include "Includes/Message/Explosion.h"
#include "Includes/Message/StartGameMessage.h"
#include "Includes/Message/PauseGameMessage.h"
#include "Includes/Message/LoadMessage.h"
#include "Includes/Message/LoadingMessage.h"
#include "Includes/Message/LoadedMessage.h"

// Systems
#include "Includes/System/KinematicSystem.h"
#include "Includes/System/RenderSystem.h"
#include "Includes/System/JoystickSystem.h"
#include "Includes/System/DebugSystem.h"
#include "Includes/System/CollisionResolverSystem.h"
#include "Includes/System/CollisionDetectionSystem.h"
#include "Includes/System/CameraSystem.h"
#include "Includes/System/BackgroundSystem.h"
#include "Includes/System/WeatherSystem.h"

#include "Includes/Resource/ResourceStore.hpp"

#ifdef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:console")
#define ENTRY_POINT int main()
#else
#pragma comment(linker, "/SUBSYSTEM:windows")
#define ENTRY_POINT int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif

ENTRY_POINT
{
	// SFML startup
	auto clock = sf::Clock();
	auto video = sf::VideoMode(800U, 600U);
	auto window = std::make_shared<sf::RenderWindow>(video, "", sf::Style::Titlebar | sf::Style::Close);

	// Managers startup
	auto messages = std::make_shared<mqs::MessageManager>();
	auto entities = std::make_shared<ecs::EntityManager>(messages);
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
		texture.loadFromFile(path);
		return texture;
	});

	// Message hooking for debugging purposes
	messages->hook([](const mqs::Message& message) {
		DEBUG("Hooked message: id = %d, uid = %d, family = %d", message.id, message.uid(), message.family);
	});

	// State transition setup
	states->
		map<PlayingState>(systems).done();
	/*
		 map<SplashState>().onDone().go<MenuState>()
		.map<MenuState>()
			.onDone().go<LoadingState>()
			.onMessage<StartGameMessage>().go<LoadingState>()
		.map<LoadingState>().onDone().go<PlayingState>()
		.map<PlayingState>(systems)
			.onMessage<LoadStartMessage>().go<LoadingState>()
			.onMessage<PauseGameMessage>().go<PausedState>()
		.map<PausedState>().onMessage<PauseGameMessage>().go<PlayingState>()
		.done();
		*/

	// System registration
	systems->add<WeatherSystem>();
	systems->add<JoystickSystem>();
	systems->add<KinematicSystem>(window);
	systems->add<CollisionDetectionSystem>();
	systems->add<CollisionResolverSystem>();
	systems->add<CameraSystem>(window);
	systems->add<BackgroundSystem>();
	systems->add<RenderSystem>(window);
	systems->add<DebugSystem>(window);

	// SFML window setup
	window->setMouseCursorVisible(true);
	window->setKeyRepeatEnabled(true);
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	auto player = entities->create();
	player.assign<Body>(1.f);
	player.assign<Motion>();	
	player.assign<Transform>(400.f, 300.f);
	player.assign<Render>(sf::Color::Red);
	player.assign<Joystick>();
	player.assign<Camera>();

	auto enemy = entities->create();
	enemy.assign<Body>(2.5f);
	enemy.assign<Motion>();
	enemy.assign<Transform>(500.f, 100.f);
	enemy.assign<Render>(sf::Color::Yellow);	

	TextureStore::get("grid.png").setRepeated(true);

	sf::Sprite background;
	background.setTexture(TextureStore::get("grid.png"));
	background.setTextureRect(sf::IntRect(0, 0, 5000, 5000));
	background.setPosition(-2500.f, -2500.f);

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
					auto v = window->getView().getCenter();
					auto p = window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					e.assign<Body>(rand() % 2 + 1);
					e.assign<Motion>();
					e.assign<Render>(sf::Color(rand() % 255, rand() % 255, rand() % 255));
					e.assign<Transform>(p.x, p.y);
					break;
				}
				case sf::Mouse::Right:
				{
					auto p = window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					messages->publish<Explosion>(rand() % 20 + 10, p.x, p.y);
					break;
				}
				}
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Return:
					break;
				case sf::Keyboard::Num1:
					player.assign<Camera>();
					enemy.remove<Camera>();
					player.assign<Joystick>();
					enemy.remove<Joystick>();
					break;
				case sf::Keyboard::Num2:
					player.remove<Camera>();
					enemy.assign<Camera>();
					player.remove<Joystick>();
					enemy.assign<Joystick>();
					break;
				case sf::Keyboard::Num3:
					break;
				case sf::Keyboard::Num4:
					break;
				case sf::Keyboard::Num8:
					//messages->publish<Explosion>(25.f, player.component<Transform>().x - 20.f, player.component<Transform>().y);
					break;
				case sf::Keyboard::Num9:
					//messages->publish<Explosion>(25.f, player.component<Transform>().x, player.component<Transform>().y);
					break;
				case sf::Keyboard::Escape:
					messages->publish<PauseGameMessage>();
					break;
				case sf::Keyboard::Space:
					window->close();
					break;
				case sf::Keyboard::Up:
					break;
				case sf::Keyboard::Down:
					break;
				case sf::Keyboard::Left:
					break;
				case sf::Keyboard::Right:
					break;
				}
				break;
			}
		}

		float delta = clock.restart().asSeconds();

		window->clear(sf::Color(128, 128, 128));
		window->draw(background);
		states->update(delta);
		window->display();
	}

	return 0;
}