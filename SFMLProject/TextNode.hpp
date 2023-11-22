#pragma once
#include "SceneNode.hpp"
#include "ResourceIdentifiers.hpp"
#include "Utility.hpp"

class TextNode : public SceneNode {
private:
	sf::Text mText;
private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	explicit TextNode(const FontHolder& fonts, const std::string& text);
public:
	void setString(const std::string& text);
};