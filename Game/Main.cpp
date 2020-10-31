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
#include "Includes/System/CollisionSystem.h"
#include "Includes/System/CollisionResolverSystem.h"
#include "Includes/System/CollisionDetectionSystem.h"
#include "Includes/System/CameraSystem.h"
#include "Includes/System/BackgroundSystem.h"
#include "Includes/System/WeatherSystem.h"

#include "Includes/Resource/ResourceStore.hpp"

#include <stack>

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

	std::stack<unsigned> actions;
	QuadTree tree(0.f, 0.f, 800.f, 600.f);

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
		//DEBUG("Hooked message: id = %d, uid = %d, family = %d", message.id, message.uid(), message.family);
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
	//systems->add<WeatherSystem>();	
	systems->add<JoystickSystem>(window);
	systems->add<KinematicSystem>(window);
	//systems->add<CollisionSystem>(window);
	//systems->add<CollisionDetectionSystem>();
	//systems->add<CollisionResolverSystem>();
	//systems->add<CameraSystem>(window);
	//systems->add<BackgroundSystem>(window);
	systems->add<RenderSystem>(window);
	systems->add<DebugSystem>(window);

	// SFML window setup
	window->setMouseCursorVisible(true);
	window->setKeyRepeatEnabled(true);
	window->setVerticalSyncEnabled(false);
	window->setFramerateLimit(60);
	
	auto player = entities->create();
	/*
	player.assign<Body>(1.f);
	player.assign<Motion>();	
	player.assign<Transform>(400.f, 300.f);
	player.assign<Render>(sf::Color(128, 128, 128));
	player.assign<Joystick>();
	player.assign<Camera>();
	
	*/

	sf::RectangleShape cursor;
	cursor.setSize(sf::Vector2f(150.f, 150.f));
	cursor.setOrigin(cursor.getSize().x / 2.f, cursor.getSize().y / 2.f);
	cursor.setOutlineThickness(1.2f);
	cursor.setOutlineColor(sf::Color::Black);
	cursor.setFillColor(sf::Color::Transparent);
	//cursor.setRotation(45.f);

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
			case sf::Event::MouseMoved:
				cursor.setPosition(event.mouseMove.x, event.mouseMove.y);
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					auto e = entities->create();
					auto v = window->getView().getCenter();
					auto p = window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					//e.assign<Body>(1.f, rand() % 112 + 16);
					e.assign<Body>(1.f, 32.f);
					e.assign<Motion>();
					e.assign<Render>(sf::Color(rand() % 255, rand() % 255, rand() % 255));
					e.assign<Transform>(p.x, p.y);
					tree.addCircle(e.id(), p.x, p.y, e.component<Body>().radius);
					actions.push(e.id());
				}
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					auto qqx = cursor.getPosition().x - cursor.getOrigin().x;
					auto qqy = cursor.getPosition().y - cursor.getOrigin().y;
					std::vector<std::shared_ptr<QuadNode>> n;
					tree.query(qqx, qqy, cursor.getSize().x, cursor.getSize().y, n);
					DEBUG("Fetched %d entities:", n.size());
					for (auto& entity : n) {
						DEBUG("%d", entity->id);
					}
					//auto p = window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					//messages->publish<Explosion>(rand() % 20 + 10, p.x, p.y);
					break;
				}
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
					messages->publish<PauseGameMessage>();
					break;
				case sf::Keyboard::Space:
					window->close();
					break;
				case sf::Keyboard::W:
				{
					auto view = window->getView();
					view.move(0.f, -15.f);
					window->setView(view);
				}
					break;
				case sf::Keyboard::S:
				{
					auto view = window->getView();
					view.move(0.f, 15.f);
					window->setView(view);
				}
					break;
				case sf::Keyboard::A:
				{
					auto view = window->getView();
					view.move(-15.f, 0.f);
					window->setView(view);
				}
					break;
				case sf::Keyboard::D:
				{
					auto view = window->getView();
					view.move(15.f, 0.f);
					window->setView(view);
				}
					break;
				case sf::Keyboard::Z:
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
						if (!actions.empty()) {
							auto id = actions.top();
							DEBUG("Destroying %d", id);
							entities->destroy(id);
							actions.pop();
						}
					}
					break;
				}
				break;
			}
		}

		window->clear(sf::Color(128, 128, 128));
		states->update(clock.restart().asSeconds());
		tree.draw(*window);
		window->draw(cursor);
		window->display();
	}

	return 0;
}