#include "bubbleContainer.h"
#include <cheerp/clientlib.h>
#include <cheerp/types.h>
#include <cheerp/client.h>
#include <deque>
#include <cmath>
#include <iostream>

Bubble::Bubble(BubbleContainer& container, client::HTMLElement* inner)
{
	innerDiv = inner;
	div = (client::CircleDiv*)client::document.createElement("div");

	auto innerStyle = inner->get_style();
	innerStyle -> set_height("100%");
	innerStyle -> set_width("auto");
	innerStyle -> set_backgroundColor("white");
	div->appendChild(inner);

	auto style = div->get_style();
	style -> set_overflow("hidden");
	style -> set_position("absolute");
	style -> set_borderRadius("50%");
	style -> set_justifyContent("center");
	style -> set_display("flex");

	container.container->appendChild(div);

	radius = 0.0;
	const double k = rand() / (1.0* RAND_MAX) * 4.0;
	if (k < 2.0)
		posX = k - (int)k;
	else
		posY = k - (int)k;
	if (k >= 1.0 && k < 2.0)
		posY = 1.0;
	else if (k >= 3.0)
		posX = 1.0;


	setPosition();
}

double Bubble::x() const
{
	return posX;
}

double Bubble::y() const
{
	return posY;
}

double Bubble::get_radius() const
{
	return radius;
}

void Bubble::changeRadius(double newRadius)
{
	radius = std::min(0.5, newRadius);
	posX = std::max(posX, radius);
	posX = std::min(posX, 1.0-radius);
	posY = std::max(posY, radius);
	posY = std::min(posY, 1.0-radius);
}

void Bubble::invisible(BubbleContainer& bubbleContainer)
{
	isInvisible = true;
	div->removeChild(innerDiv);
	bubbleContainer.container->removeChild(div);
}

void Bubble::setPosition()
{
	div->get_style()->set_left(client::String(100.0*(x() - radius)).concat("%"));
	div->get_style()->set_top(client::String(100.0*(y() - radius)).concat("%"));
	div->get_style()->set_width(client::String(100.0*(2.0*radius)).concat("%"));
	div->get_style()->set_height(client::String(100*(2.0*radius)).concat("%"));
}

BubbleContainer::BubbleContainer(client::SquareDiv* container) :
	container(container) {srand(34);};

void BubbleContainer::addBubble(client::HTMLElement* inner)
{
	if (!bubbles)
		bubbles = new std::deque<Bubble>;
	bubbles->emplace_back(*this, inner);
}

void BubbleContainer::recalculateAll()
{
	for (unsigned int i = 0; i<bubbles->size(); i++)
	{
		Bubble& B = (*bubbles)[i];
		B.setPosition();
	}
}

void BubbleContainer::insert(client::HTMLElement* inner)
{
	if (!bubbles)
		bubbles = new std::deque<Bubble>;
	//Check it's not already present
	for (const Bubble& bubble : (*bubbles))
	{
		if (bubble.innerDiv == inner)
			return;
	}

	addBubble(inner);
}

void BubbleContainer::erase(client::HTMLElement* inner)
{
	if (!bubbles)
		bubbles = new std::deque<Bubble>;
	//Delete if present
	bool found = false;
	for (Bubble& bubble : (*bubbles))
	{
		if (bubble.innerDiv == inner)
		{
			std::swap(bubble, bubbles->back());
			found = true;
			break;
		}
	}
	if (found)
	{
		bubbles->back().invisible(*this);
		bubbles->pop_back();
	}
}

void BubbleContainer::doStep()
{
	double timeStep = 1.0f / 60.0f;
	int velocityIterations = 6;
	int positionIterations = 2;

	double minD = 0.5;
	for (unsigned int i = 0; i<bubbles->size(); i++)
	{
		const Bubble& B = (*bubbles)[i];
		minD = fmin(minD, B.x());
		minD = fmin(minD, 1.0-B.x());
		minD = fmin(minD, B.y());
		minD = fmin(minD, 1.0-B.y());
	}
	minD += 0.01;
	//TODO: fix this somehow
	double minD2 = 4*minD*minD;
	for (unsigned int i = 0; i<bubbles->size(); i++)
	{
		const Bubble& B = (*bubbles)[i];
		for (unsigned int j = i+1; j<bubbles->size(); j++)
		{
			const Bubble& C = (*bubbles)[j];
			const double dist2 = pow(C.x() - B.x(), 2.0) + pow(C.y() - B.y(), 2.0);
			minD2 = fmin(minD2, dist2);
		}
	}
	minD = sqrt(minD2)/2;
	for (unsigned int i = 0; i<bubbles->size(); i++)
	{
		Bubble& B = (*bubbles)[i];
		B.changeRadius(minD);
	}

	for(int k = 0; k < 20; k++)
	{
	std::vector<double> fx(bubbles->size(), 0.0);
	std::vector<double> fy(bubbles->size(), 0.0);
	for (unsigned int i = 0; i<bubbles->size(); i++)
	{
		const Bubble& B = (*bubbles)[i];
		for (unsigned int j = 0; j<bubbles->size(); j++)
			if (j!=i)
		{
			const Bubble& C = (*bubbles)[j];
			const double dist2 = pow(C.x() - B.x(), 2.0) + pow(C.y() - B.y(), 2.0);
			double dist = sqrt(dist2);// - 1.9*minD;
			fx[j] += 1/dist2 * (C.x() - B.x()) / dist;
			fy[j] += 1/dist2 * (C.y() - B.y()) / dist;
		}
		const double F = bubbles->size();
		if (B.x() > 0.0)
			fx[i] += F/(B.x());
		else fx[i] += 2.0*F/minD;
		if (B.x() < 1.0)
			fx[i] -= F/((1.0-B.x()));
		else fx[i] -= 2.0*F/minD;
		if (B.y() > 0.0)
			fy[i] += F/(B.y());
		else fy[i] += 2.0*F/minD;
		if (B.y() < 1.0)
			fy[i] -= F/((1.0-B.y()));
		else fy[i] -= 2.0*F/minD;

	}

	for (unsigned int i = 0; i<bubbles->size(); i++)
	{
		Bubble& B = (*bubbles)[i];
		B.posX += fx[i]/100000.0;
		B.posY += fy[i]/100000.0;
	}
	}
	recalculateAll();
}
