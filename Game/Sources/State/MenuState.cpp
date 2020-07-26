#include "../../Includes/State/MenuState.h"

void MenuState::onEnter()
{
	DEBUG("-----> Menu");
}

void MenuState::onLeave()
{
	DEBUG("<----- Menu");
}

void MenuState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

}

sts::StateResult MenuState::update(float dt)
{
	return sts::StateResult::Done;
}