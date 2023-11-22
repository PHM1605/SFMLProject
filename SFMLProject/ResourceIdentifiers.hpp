#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"

namespace Textures {
	enum ID { Eagle, Raptor, Avenger, Bullet, Missile, Desert, HealthRefill, MissleRefill, FireSpread, FireRate, TitleScreen, ButtonNormal, ButtonSelected, ButtonPressed};
}

namespace Fonts {
	enum ID { Main };
}

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
