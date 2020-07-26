#include "../../Includes/State/LoadingState.h"

void LoadingState::onEnter()
{
	TRACE("-----> Loading");
}

void LoadingState::onLeave()
{
	TRACE("<----- Loading");
}

void LoadingState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

}

sts::StateResult LoadingState::update(float dt)
{
	return sts::StateResult::Done; // Change accordingly
}