#include "../../Includes/State/SplashState.h"

void SplashState::onEnter()
{
	INFO("-----> Splash");
}

void SplashState::onLeave()
{
	INFO("<----- Splash");
}

void SplashState::onEvent(const sf::Event& event)
{

}

void SplashState::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

}

sts::StateResult SplashState::update(float delta)
{
	m_elapsed += delta;

	if (m_elapsed >= m_duration) {
		return sts::StateResult::Done;
	}
	else {
		return sts::StateResult::Running;
	}
}