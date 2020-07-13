#include <iostream>

#include "Entities/System/SystemManager.hpp"
#include "Entities/Entity/Entity.hpp"
#include "Messages/MessageManager.hpp"
#include "States/StateManager.hpp"
#include "Logger.hpp"

class IntroState : public sts::State
{
	void onEnter() override {
		INFO("--------> IntroState");
	}

	void onLeave() override {
		INFO("<-------- IntroState");
	}

	sts::StateResult update(float dt) override {
		return sts::StateResult::Finished;
	}
};

class LoadingState : public sts::State // Probably pass a loadable object, which will support passing assets?
{
	void onEnter() override {
		DEBUG("--------> LoadingState");
	}

	void onLeave() override {
		DEBUG("<-------- LoadingState");
	}

	sts::StateResult update(float dt) override {
		return sts::StateResult::Finished;
	}
};

class MenuState : public sts::State // Main menu and playing menu
{
	void onEnter() override {
		TRACE("--------> MenuState");
	}

	void onLeave() override {
		TRACE("<-------- MenuState");
	}

	sts::StateResult update(float dt) override {
		return sts::StateResult::Finished;
	}
};

class PausedState : public sts::State
{
	void onEnter() override {
		WARN("--------> PausedState");
	}

	void onLeave() override {
		WARN("<-------- PausedState");
	}

	sts::StateResult update(float dt) override {
		return sts::StateResult::Finished;
	}
};

class PlayingState : public sts::State
{
	void onEnter() override {
		ERR("--------> PlayingState");
	}

	void onLeave() override {
		ERR("<-------- PlayingState");
	}

	sts::StateResult update(float dt) override {
		return sts::StateResult::Finished;
	}
};

struct Position
{
	Position() : x(0), y(0) {}
	Position(int x, int y) : x(x), y(y) {}
	int x, y;
};

struct TeleportMessage : public mqs::ManagedMessage<TeleportMessage>
{
	TeleportMessage() : mqs::ManagedMessage<TeleportMessage>(0U) {}
};

struct CollisionMessage : public mqs::ManagedMessage<CollisionMessage>
{
	CollisionMessage() : mqs::ManagedMessage<CollisionMessage>(0U) {}
};

struct WhateverMessage : public mqs::ManagedMessage<WhateverMessage>
{
	WhateverMessage() : mqs::ManagedMessage<WhateverMessage>(0U) {}
};

struct CollisionSystem : public ecs::SystemListener<CollisionMessage, TeleportMessage>
{
	void update(float dt) {}
	void handle(const CollisionMessage&) override { std::cout << "CollisionMessage message!" << std::endl; }
	void handle(const TeleportMessage&) override { std::cout << "TeleportMessage message!" << std::endl; }
	//void handle(const int&) override { std::cout << "Received int message!" << std::endl; }
	//void handle(const float&) override { std::cout << "Received float message!" << std::endl; }
};

void entity_manager_creation(const std::shared_ptr<ecs::EntityManager>& m)
{
	auto c1 = 10;
	auto c2 = 240.f;

	auto e1 = m->create(); // Empty
	auto e2 = m->create(c1, c2); // Use component
	auto e3 = m->create<int>(22); // Construct component

	assert(e2.has<int>() && e2.component<int>() == 10);
	assert(e3.has<int>() && e3.component<int>() == 22);
	assert(e2.has<float>() && e2.component<float>() == 240.f);
}

void entity_manager_assignment(const std::shared_ptr<ecs::EntityManager>& m)
{
	auto c1 = 10;
	auto c2 = 240.f;

	auto e1 = m->create();
	auto e2 = m->create();

	e1.assign<int>(40); // Construct component
	e1.assign(c2); // Use component
	e2.assign(c1, c2); // Use components
	e2.assign<Position>(22, 33); // Construct component

	assert(e1.has<int>() && e1.component<int>() == 40);
	assert(e1.has<float>() && e1.component<float>() == 240.f);
	assert(e2.has<int>() && e2.component<int>() == 10);
	assert(e2.has<float>() && e2.component<float>() == 240.f);
	assert(e2.has<Position>() && e2.component<Position>().x == 22);
}

void entity_manager_replacement(const std::shared_ptr<ecs::EntityManager>& m)
{
	auto c1 = 10;
	auto c2 = 240.f;
	auto c3 = 30;
	auto c4 = 320.f;

	auto e1 = m->create(c1, c2);
	auto e2 = m->create<int>(34);

	e1.replace(c3, c4); // Use components
	e1.replace<int>(60); // Construct component
	e2.replace(c1); // Use component
	//e2.replace<float>(30.bar); // Theres no float component: error!

	assert(e1.has<int>() && e1.component<int>() == 60);
	assert(e1.has<float>() && e1.component<float>() == 320.f);
	assert(e2.has<int>() && e2.component<int>() == 10);
}

void entity_manager_accomodation(const std::shared_ptr<ecs::EntityManager>& m)
{
	auto c1 = 30;
	auto c2 = 320.f;

	auto e1 = m->create(c1, c2);
	auto e2 = m->create<int>(34);

	e1.save<int>(2); // Build and replace
	e1.save<float>(50.f); // Build and replace
	e2.save(c1); // Replace
	e2.save(c2); // Assign
	e2.save<double>(68.0); // Build and assign

	assert(e1.has<int>() && e1.component<int>() == 2);
	assert(e1.has<float>() && e1.component<float>() == 50.f);
	assert(e2.has<float>() && e2.component<float>() == 320.f);
	assert(e2.has<double>() && e2.component<double>() == 68.0);
}

void entity_manager_resetion(const std::shared_ptr<ecs::EntityManager>& m)
{
	auto c1 = 89;
	auto c2 = 76.f;
	auto c3 = 12;

	auto e1 = m->create(c1);
	auto e2 = m->create(c1, c2);

	e1.reset(&c3);
	e1.reset<double>(); // Do nothing
	e2.reset<int, float>();

	assert(!e2.has<double>());
	assert(!e1.has<int>());
	assert(!e2.has<int>());
	assert(!e2.has<float>());
}

void entity_manager_remotion(const std::shared_ptr<ecs::EntityManager>& m)
{
	auto c1 = 4;
	auto c2 = 30.f;

	auto e1 = m->create(c1, c2);
	auto e2 = m->create(c2);

	e1.remove<int, float>();
	e2.remove(&c2);

	assert(!e1.has<int>());
	assert(!e1.has<float>());
	assert(!e2.has<float>());
}

void entity_manager_checkup(const std::shared_ptr<ecs::EntityManager>& m) {
	auto c1 = 4;
	auto c2 = 30.f;

	auto e1 = m->create(c1, c2);
	auto e2 = m->create(c2);

	assert(e1.has<int>());
	assert(e2.has<float>());
	assert(e2.has<float>());
	assert(!e1.has<double>());
}

void entity_manager_counting() {
	ecs::EntityManager m;

	auto c1 = 4;
	auto c2 = 30.f;

	auto e1 = m.create<int>();
	auto e2 = m.create(c1, c2);

	assert(m.count<int>() == 2);
	assert(m.count<float>() == 1);
}

void entity_manager_sizes() {
	ecs::EntityManager m;

	auto e1 = m.create();
	auto e2 = m.create();
	auto e3 = m.create();

	e1.destroy();
	e2.destroy();

	assert(m.size() == 1U);
}

void entity_manager_iteration(const std::shared_ptr<ecs::EntityManager>& m) {
	m->each([](ecs::Entity& e) {
		printf("Iterating entity %d (v%d)... \n", e.id(), e.version());
	});

	m->each<int>([](ecs::Entity& e, int& i) {
		printf("Entity %d has int component: %d \n", e.id(), i);
	});

	m->each<int, float>([](ecs::Entity& e, int& i, float& f) {
		printf("Entity %d has both int and float components: %d (int) and %.2f (float) \n", e.id(), i, f);
	});
}

void entity_system(std::shared_ptr<ecs::SystemManager>& systems, std::shared_ptr<mqs::MessageManager>& messages)
{
	{
		systems->add<CollisionSystem>();
		messages->publish<TeleportMessage>();
		messages->publish<CollisionMessage>();
		messages->publish<WhateverMessage>();
	}

	systems.reset();
	messages->publish(TeleportMessage());
	messages->publish(CollisionMessage());
	messages->publish(WhateverMessage());
}

void entity_manager(const std::shared_ptr<ecs::EntityManager>& entities)
{
	entity_manager_creation(entities);
	entity_manager_assignment(entities);
	entity_manager_replacement(entities);
	entity_manager_accomodation(entities);
	entity_manager_resetion(entities);
	entity_manager_remotion(entities);
	entity_manager_checkup(entities);	
	entity_manager_iteration(entities);
	entity_manager_counting();
	entity_manager_sizes();
}

void state_manager(const std::shared_ptr<sts::StateManager>& states, const std::shared_ptr<mqs::MessageManager>& messages)
{
	// Intro -> Load required assets -> Menu -> Load level <-> Playing <-> Paused
	states->
		 map<IntroState>().onMessage<CollisionMessage>().go<PlayingState>()
		.map<LoadingState>()
			.onFinished().go<IntroState>()
			.onFinished().go<PlayingState>()
			.onFinished().go<MenuState>()
		.map<MenuState>()
		.map<PausedState>()
		.map<PlayingState>().onMessage<TeleportMessage>().go<IntroState>()
		.done();

	for (int i = 0; i < 20; i++) {
		states->update(0.f);
	}
	
	messages->publish<CollisionMessage>();

	for (int i = 0; i < 20; i++) {
		states->update(0.f);
	}

	messages->publish<TeleportMessage>();

	for (int i = 0; i < 20; i++) {
		states->update(0.f);
	}
}

int main()
{
	auto messages = std::make_shared<mqs::MessageManager>();
	auto entities = std::make_shared<ecs::EntityManager>();
	auto states = std::make_shared<sts::StateManager>(messages);
	auto systems = std::make_shared<ecs::SystemManager>(entities, messages);

	entity_system(systems, messages);
	entity_manager(entities);
	state_manager(states, messages);

	return getchar();
}