#pragma once

#include <SFML\Graphics\RectangleShape.hpp>

#include "State.h"

class PausedState final : public State
{
public:

	PausedState();

	void onEnter() override;
	void onLeave() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	sts::StateResult update(float dt) override;

private:
	sf::RectangleShape overlap;
};