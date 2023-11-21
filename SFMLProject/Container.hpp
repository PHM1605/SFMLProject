#pragma once
#include "Component.hpp"
#include <vector>

namespace GUI {

	class Container : public Component {
	public:
		Container();
		void pack(Component::Ptr component);
		virtual bool isSelectable() const;
		virtual void handleEvent(const sf::Event& event);
	private:
		std::vector<Component::Ptr> mChildren;
		int mSelectedChild;
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		bool hasSelection() const; // Container has child(ren) Components(s) or not
		void select(std::size_t index);
		void selectNext();
		void selectPrevious();
	};
}