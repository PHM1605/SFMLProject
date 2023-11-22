#pragma once
#include <SFML/Graphics.hpp>

namespace GUI 
{
	class Component : public sf::Drawable, public sf::Transformable, private sf::NonCopyable {
	public:
		typedef std::shared_ptr<Component> Ptr;
	public:
		Component();
		virtual ~Component();
		// Buttons are selectable; Labels are not
		virtual bool isSelectable() const = 0;

		// Select is for all Buttons
		bool isSelected() const;
		virtual void select();
		virtual void deselect();

		// Active/Activate is for Toggle Buttons (in Settings state)
		bool isActive() const;
		virtual void activate();
		virtual void deactivate();

		virtual void handleEvent(const sf::Event& event) = 0;
	private:
		bool mIsSelected;
		bool mIsActive;
	};
}