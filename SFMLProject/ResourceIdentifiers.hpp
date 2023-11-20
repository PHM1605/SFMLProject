#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"

namespace Textures {
	enum ID { Eagle, Raptor, Desert, TitleScreen };
}

namespace Fonts {
	enum ID { Main };
}

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
