#include <SFML/Graphics/RenderWindow.hpp>

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
#include "Includes/Message/LoadStartMessage.h"
#include "Includes/Message/LoadProgressMessage.h"
#include "Includes/Message/LoadCompleteMessage.h"

// Systems
#include "Includes/System/KinematicSystem.h"
#include "Includes/System/RenderSystem.h"
#include "Includes/System/JoystickSystem.h"
#include "Includes/System/DebugSystem.h"
#include "Includes/System/CollisionResolverSystem.h"
#include "Includes/System/CollisionDetectionSystem.h"
#include "Includes/System/CameraSystem.h"

#include "Includes/Resource/ResourceStore.hpp"

#ifdef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:console")
#define ENTRY_POINT int main()
#else
#pragma comment(linker, "/SUBSYSTEM:windows")
//#define ENTRY_POINT int __stdcall WinMain(void*, void*, char*, int)
#define ENTRY_POINT int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif

float vx = 400.f;
float vy = 300.f;
float o1x = 50.f;
float o1y = 100.f;
float o2x = 500.f;
float o2y = 250.f;
float o3x = 700.f;
float o3y = 500.f;

ENTRY_POINT
{
	// SFML startup
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
		texture.loadFromFile(path);
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

	systems->add<JoystickSystem>(window);
	systems->add<KinematicSystem>();
	systems->add<CollisionDetectionSystem>();
	systems->add<CollisionResolverSystem>();
	systems->add<CameraSystem>(window);
	systems->add<RenderSystem>(window);
	systems->add<DebugSystem>(window);

	// SFML window setup
	window->setMouseCursorVisible(true);
	window->setKeyRepeatEnabled(true);
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);

	auto player = entities->create();

	player.assign<Body>(30.f);
	player.assign<Motion>();
	player.assign<Joystick>();
	player.assign<Camera>();
	player.assign<Transform>(200.f, 300.f);
	player.assign<Render>(sf::Color::Red);

	auto enemy = entities->create();

	enemy.assign<Body>(rand() % 10 + 30);
	enemy.assign<Motion>();
	enemy.assign<Transform>(600.f, 300.f);
	enemy.assign<Render>(sf::Color::Yellow);

	sf::Sprite o1;
	o1.setTexture(TextureStore::get("1.png"));
	o1.setPosition(o1x, o1y);

	sf::Sprite o2;
	o2.setTexture(TextureStore::get("2.png"));
	o2.setPosition(o2x, o2y);

	sf::Sprite o3;
	o3.setTexture(TextureStore::get("3.png"));
	o3.setPosition(o3x, o3y);

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
					auto v = window->getView().getViewport();
					auto p = window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					e.assign<Body>(rand() % 50 + 30);
					e.assign<Motion>();
					e.assign<Render>(sf::Color(rand() % 255, rand() % 255, rand() % 255));
					e.assign<Transform>(p.x, p.y);
					break;
				}
				case sf::Mouse::Right:
				{
					auto v = window->getView().getViewport();
					auto p = window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					messages->publish<Explosion>(10.f, p.x, p.y);
					break;
				}
				}
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Return:
				{
					messages->publish<StartGameMessage>();

					// Toggle controller type
					switch (player.component<Joystick>().type)
					{
					case Joystick::Type::Mouse:
						player.component<Joystick>().type = Joystick::Type::Keyboard;
						break;
					case Joystick::Type::Keyboard:
						player.component<Joystick>().type = Joystick::Type::Mouse;
						break;
					}
					break;
				}
				case sf::Keyboard::Num1:
					player.assign<Camera>();
					enemy.remove<Camera>();
					messages->publish<CameraChanged>();
					break;
				case sf::Keyboard::Num2:
					player.remove<Camera>();
					enemy.assign<Camera>();
					messages->publish<CameraChanged>();
					break;
				case sf::Keyboard::Num3:
					break;
				case sf::Keyboard::Num4:
					break;
				case sf::Keyboard::Num8:
					messages->publish<Explosion>(25.f, player.component<Transform>().x - 20.f, player.component<Transform>().y);
					break;
				case sf::Keyboard::Num9:
					messages->publish<Explosion>(25.f, player.component<Transform>().x, player.component<Transform>().y);
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
		systems->update(delta);
		states->update(delta);
		window->draw(o1);
		window->draw(o2);
		window->draw(o3);	
		window->display();
	}

	return 0;
}