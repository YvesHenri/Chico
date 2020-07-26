#include "../../Includes/State/PausedState.h"

PausedState::PausedState()
{
	overlap = sf::RectangleShape(sf::Vector2f(800.f, 600.f));
	overlap.setFillColor(sf::Color(0, 0, 0, 185));
}

void PausedState::onEnter()
{
	WARN("-----> Paused");
}

void PausedState::onLeave()
{
	WARN("<----- Paused");
}

void PausedState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	/*
	// Draw the previous state first
	if (m_previous) {
		m_previous->draw(target, states);
	}
	*/

	// The overlap rectangle has to be drawn using the fixed view
	sf::View fixed = target.getDefaultView();
	sf::View active = target.getView();

	target.setView(fixed);
	target.draw(overlap, states);
	target.setView(active);
}

sts::StateResult PausedState::update(float dt)
{
	return sts::StateResult::Running;
}