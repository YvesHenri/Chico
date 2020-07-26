#pragma once

#include "State.h"

class SplashState final : public State
{
public:
	void onEnter() override;
	void onLeave() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	sts::StateResult update(float dt) override;

private:
	float m_duration = 0.f; // TODO set value
	float m_elapsed = 0.f;
};