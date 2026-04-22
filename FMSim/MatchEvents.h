#pragma once
#include <string>

enum class AttackEvent
{
	ShortPass,
	LongPass,
	ThroughBall,
	Dribble,
	Shoot,
	Cross,
	FreeKickShot,
	FreeKickPass,
	FreeKickCross,
	FreeKickHeader,
	Header,
	LobbedShot,
	OneTwoPass,
	PenaltyShot,
	SwitchOfPlay,
	GoalKick,
	GoalkeeperThrow,
	CornerKick,
	CornerKickShort,
	ThrowIn,
	ThrowInLong,
	DribbleCutInside,
	BackPass,
	TapIn,
	CutBack,
	Clearance
};

enum class DefenseEvent
{
	Tackle,
	Interception,
	Block,
	HeadingClearance,
	SlidingTackle,
	Pressure,
	ForwardPress,
	FreeKickClearance,
	FreeKickIntercept,
	FreeKickHeadingClearance,
	GoalkeeperSave,
	GoalkeeperPunch,
	GoalkeeperCatch,
	Clearance,
	Idle
};

enum class EventOutcome
{
	Success,
	Fail,
	Goal,
	Saved,
	Blocked,
	Cleared,
	Out,
	ThrownIn,
	CornerKick,
	GoalKeeperHeld,
	Rebound,
	Fouled
};

enum class OtherEvent
{
	Kickoff,
	HalfTime,
	FullTime,
	Rebound,
	OwnGoal,
	Offside,
	Foul,
	Injury,
	OutOfPlay,
	ThrowIn,
	CornerKick
};

enum class PossessionState
{
	Home,
	Away,
	None
};

// Represent an event that occurs during a match
class MatchEvent
{
public:
	MatchEvent(AttackEvent attackEvent);
	MatchEvent(DefenseEvent defenseEvent);
	MatchEvent(OtherEvent otherEvent);

private:

};

struct BallState
{
	Zone currentZone;
	PossessionState possessionState;
};