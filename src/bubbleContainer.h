#include <cheerp/clientlib.h>
#include <cheerp/types.h>
#include <cheerp/client.h>
#include <deque>

namespace client
{
	class [[cheerp::genericjs]] CircleDiv : public HTMLDivElement {};
	class [[cheerp::genericjs]] SquareDiv : public HTMLDivElement {};
};

class [[cheerp::genericjs]][[cheerp::jsexport]] BubbleContainer;

class [[cheerp::genericjs]] Bubble
{
public:
	Bubble(BubbleContainer& container, client::HTMLElement* inner);
	double x() const;
	double y() const;
	double get_radius() const;
	void changeRadius(double newRadius);
	void setPosition();
	void invisible(BubbleContainer& container);
	double posX, posY;
	client::HTMLElement* innerDiv;
	client::CircleDiv* div;
	bool isInvisible{false};
private:
	double radius;
};

class [[cheerp::genericjs]][[cheerp::jsexport]] BubbleContainer
{
public:
	BubbleContainer(client::SquareDiv* container);
	void insert(client::HTMLElement* inner);
	void erase(client::HTMLElement* inner);
	void doStep();
private:
	void addBubble(client::HTMLElement* inner);
	void recalculateAll();
	client::SquareDiv* container;
	std::deque<Bubble>* bubbles{nullptr};
	friend Bubble;
};
