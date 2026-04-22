#pragma once
#include "Event.h"

enum class ShotType
{
	Shoot,
	FreeKickShot,
	LobbedShot,
	PenaltyShot,
	TapIn,
	Header,
	FreeKickHeader
};

class ShotEvent : public Event
{
public:
	explicit ShotEvent(ShotType shotType);

	double getEffortCoefficient() const override;

	std::string getName() const override;

	ShotType getShotType() const;
private:
	ShotType shotType;
};
