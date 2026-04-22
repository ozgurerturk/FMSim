#pragma once
#include "Event.h"

enum class DribbleType
{
	Dribble,
	DribbleCutInside
};

class DribbleEvent : public Event
{
public:
	explicit DribbleEvent(DribbleType dribbleType);
	double getEffortCoefficient() const override;
	std::string getName() const override;
	DribbleType getDribbleType() const;
private:
	DribbleType dribbleType;
};
