#pragma once

#include <Engine/Entities/System/SystemManager.hpp>

#include "State.h"

class PlayingState final : public State
{
public:
	explicit PlayingState(const std::shared_ptr<ecs::SystemManager>& systems) : systems(systems) {}

	void onEnter() override;
	void onLeave() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	sts::StateResult update(float dt) override;

private:
	std::shared_ptr<ecs::SystemManager> systems;
};