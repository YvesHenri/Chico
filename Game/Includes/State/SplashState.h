#pragma once

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include "State.h"

class SplashState final : public State
{
public:
	void onEnter() override;
	void onLeave() override;

	void onEvent(const sf::Event& event) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	sts::StateResult update(float dt) override;

private:
	float m_duration;
	float m_elapsed;

	sf::Sprite m_sprite;
	sf::Texture m_texture;
};