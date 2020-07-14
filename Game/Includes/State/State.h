#pragma once

#include <SFML\Window\Event.hpp>
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderStates.hpp>

#include "../Log.h"
#include "../Engine/States/State.hpp"

class State : public sts::State, public sf::Drawable
{
public:
	virtual void onEvent(const sf::Event& event) = 0;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
};