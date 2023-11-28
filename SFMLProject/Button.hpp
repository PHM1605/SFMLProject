#pragma once
#include "Component.hpp"
#include "ResourceIdentifiers.hpp"
#include "Utility.hpp"
#include "SoundPlayer.hpp"
#include "State.hpp"
#include <functional>

namespace GUI {
	class Button : public Component {
	public:
		typedef std::shared_ptr<Button> Ptr;
		typedef std::function<void()> Callback;
		enum Type {Normal, Selected, Pressed, ButtonCount};
	public:
		Button(State::Context context);
		void setCallback(Callback callback);
		void setText(const std::string& text);
		void setToggle(bool flag);

		virtual bool isSelectable() const;
		virtual void select();
		virtual void deselect();
		virtual void activate();
		virtual void deactivate();
		
		virtual void handleEvent(const sf::Event& event);

	private:
		Callback mCallback;
		sf::Sprite mSprite;
		sf::Text mText;
		bool mIsToggle;
		SoundPlayer& mSounds;
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void changeTexture(Type buttonType);
	};
}