#include "Container.hpp"

namespace GUI {
	Container::Container() :
		mChildren(),
		mSelectedChild(-1)
	{}

	void Container::pack(Component::Ptr component) {
		mChildren.push_back(component);
		if (!hasSelection() && component->isSelectable())
			select(mChildren.size() - 1);
	}

	bool Container::hasSelection() const {
		return mSelectedChild >= 0;
	}

	bool Container::isSelectable() const {
		return false;
	}

	void Container::handleEvent(const sf::Event& event) {
		if (hasSelection() && mChildren[mSelectedChild]->isActive()) {
			mChildren[mSelectedChild]->handleEvent(event);
		}

	}

	void Container::select(std::size_t index) {
		if (mChildren[index]->isSelectable()) {
			if (hasSelection())
				mChildren[mSelectedChild]->deselect();
			mChildren[index]->select();
			mSelectedChild = index;
		}
	}
}
