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

	void Container::select(std::size_t index) {

	}
}
