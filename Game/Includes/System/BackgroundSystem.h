#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <Engine/Entities/System/System.hpp>

// Applies parallax effect
class BackgroundSystem final : public ecs::System
{
public:
	explicit BackgroundSystem(sf::RenderWindow& window) : window(window) {
		TextureStore::get("grid.png").setRepeated(true);
		background.setTexture(TextureStore::get("grid.png"));
		background.setTextureRect(sf::IntRect(0, 0, 100000, 100000));
		background.setPosition(-50000.f, -50000.f);
	}

	void update(float delta) override {
		window.draw(background);
	}

private:
	sf::Sprite background;
	sf::RenderWindow& window;
};