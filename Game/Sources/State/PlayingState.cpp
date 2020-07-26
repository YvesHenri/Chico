#include "../../Includes/State/PlayingState.h"

void PlayingState::onEnter()
{
	ERR("-----> Playing");
}

void PlayingState::onLeave()
{
	ERR("<----- Playing");
}

void PlayingState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

}

sts::StateResult PlayingState::update(float dt)
{
	systems->update(dt);
	return sts::StateResult::Running;
}