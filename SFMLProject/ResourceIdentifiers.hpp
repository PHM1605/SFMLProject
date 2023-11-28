#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
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

namespace SoundEffect {
	enum ID {
		AlliedGunfire,
		EnemyGunfire,
		Explosion1,
		Explosion2,
		LaunchMissile,
		CollectPickup,
		Button
	};
}

namespace Music {
	enum ID {MenuTheme, MissionTheme};
}

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID> ShaderHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffect::ID> SoundBufferHolder;
