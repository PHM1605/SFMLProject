#pragma once
#include "Component.hpp"
#include <vector>

namespace GUI {

	class Container : public Component {
	public:
		Container();
		void pack(Component::Ptr component);
	private:
		std::vector<Component::Ptr> mChildren;
		int mSelectedChild;
	private:
		bool hasSelection() const; // Container has child(ren) Components(s) or not
		void select(std::size_t index);
	};
}