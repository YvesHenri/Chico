#pragma once

#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>

enum class Direction
{
	Right,
	Left,
	Iddle,
	Up,
	Down
};

struct Animator
{
	explicit Animator(const sf::Texture& texture, float size) : animationWidth(size), animationHeight(size) {
		horizontalAnimationsCount = 0;
	}
	
	explicit Animator(const sf::Texture& texture, unsigned animationWidth, unsigned animationHeight, float frameDuration = 0.5f)
		: frameDuration(frameDuration), animationWidth(animationWidth), animationHeight(animationHeight), horizontalAnimationsCount(0U), verticalAnimationsCount(0U)
	{
		auto textureSize = texture.getSize();

		if (textureSize.x % animationWidth == 0) {
			horizontalAnimationsCount = textureSize.x / animationWidth;
		}
		if (textureSize.y % animationHeight == 0) {
			verticalAnimationsCount = textureSize.y / animationHeight;
		}


	}

	unsigned horizontalAnimationsCount;
	unsigned verticalAnimationsCount;

	unsigned animationHeight;
	unsigned animationWidth;

	float frameDuration;
};