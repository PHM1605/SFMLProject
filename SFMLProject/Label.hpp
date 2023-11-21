#pragma once
#include "Component.hpp"
#include "SFML/Graphics.hpp"
#include "ResourceIdentifiers.hpp"

namespace GUI {
	class Label : public Component {
	public:
		typedef std::shared_ptr<Label>Ptr;
	public:
		Label(const std::string& text, const FontHolder& fonts);
		virtual bool isSelectable() const;
		void setText(const std::string& text);
		virtual void handleEvent(const sf::Event& event);
	private:
		sf::Text mText;
	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};
}
