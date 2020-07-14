#pragma once

#include <SFML\Graphics\RectangleShape.hpp>

#include "State.h"

class PausedState final : public State
{
public:
	PausedState();

	sts::StateResult update(float dt) override;

	void onEnter() override;
	void onLeave() override;
	void onEvent(const sf::Event& event) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::RectangleShape overlap;
};