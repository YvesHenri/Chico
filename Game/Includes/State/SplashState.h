#pragma once

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include "State.h"

class SplashState final : public State
{
public:
	sts::StateResult update(float dt) override;

	void onEnter() override;
	void onLeave() override;
	void onEvent(const sf::Event& event) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	float m_duration = 0.f; // TODO set value
	float m_elapsed = 0.f;

	sf::Sprite m_sprite;
	sf::Texture m_texture;
};