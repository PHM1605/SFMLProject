#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"

namespace Textures {
	enum ID { Entities, Jungle, TitleScreen, Buttons, Explosion, Particle, FinishLine};
}

namespace Shaders {
	enum ID {BrightnessPass, DownSamplePass, GaussianBlurPass, AddPass };
}

namespace Fonts {
	enum ID { Main };
}

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID> ShaderHolder;
