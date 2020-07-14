#pragma once

#include "State.h"

class LoadingState final : public State
{
public:
	sts::StateResult update(float dt) override;

	void onEnter() override;
	void onLeave() override;
	void onEvent(const sf::Event& event) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};