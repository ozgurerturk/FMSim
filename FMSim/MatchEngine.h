#pragma once
#include "Team.h"
#include "MatchEvents.h"
#include <queue>
#include <vector>
#include <string>

struct EventStruct
{
	bool isOtherEvent = false;
	int elapsedSeconds = 0;
	PossessionState possessionState;
	Zone fromZone;
	Zone toZone;
	Zone currentZone;
	AttackEvent attackEvent;
	DefenseEvent defenseEvent;
	OtherEvent otherEvent;
	EventOutcome eventOutcome;
};

class MatchEngine
{
public:
	// constant declarations
	static inline constexpr double STARTING_STAMINA = 100;
	static inline constexpr double HOME_ADVANTAGE_BONUS = 1.1;
	static inline constexpr double ZONE_ATTACK_WEIGHTS[9][9] = {
		{ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 },
		{ 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 },
		{ 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1 },
		{ 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2 },
		{ 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3 },
		{ 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4 },
		{ 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5 },
		{ 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6 },
		{ 0.9 ,1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
	};
	static const double ZONE_DEFENSE_WEIGHTS[9][9];

	MatchEngine();
	MatchEngine(const Team& homeTeam, const Team& awayTeam, bool enableHomeAdvantage = true, int simulationTimeInMinutes = 6);
	void simulateStart();
	int getHomeScore() const;
	int getAwayScore() const;
	const std::string& getHomeTeamName() const;
	const std::string& getAwayTeamName() const;
	TeamStrength getHomeTeamStrength() const;
	TeamStrength getAwayTeamStrength() const;
	TacticType getHomeTacticType() const;
	TacticType getAwayTacticType() const;
	DefenseTacticType getHomeDefenseTacticType() const;
	DefenseTacticType getAwayDefenseTacticType() const;
	const std::vector<std::string>& getEventLogs() const;
	std::vector<EventStruct> events;

private:
	Team _homeTeam;
	Team _awayTeam;
	TeamStrength _homeTeamStrength;
	TeamStrength _awayTeamStrength;
	int _homeScore = 0;
	int _awayScore = 0;
	int _elapsedSimulationSeconds = 0;
	std::vector<std::string> _eventLogs;
	void simulateNextEvent();
	void determineGoalKeeperSave(EventOutcome& result);
	void kickOff(bool doesHomeTeamKicksOff);
	bool coinToss();
	Zone mirrorZone(Zone zone) const;
	Zone getTacticZone(Zone zone, PossessionState possessionState) const;
	Zone getMatchZone(Zone tacticZone, PossessionState possessionState) const;
	void applyEventOutcome(EventStruct& nextEvent);
	void resetAttackState();
	std::string toString(PossessionState possessionState) const;
	std::string toString(AttackEvent attackEvent) const;
	std::string toString(EventOutcome eventOutcome) const;
	std::queue<int> totalNormalAvailableEventsCounter;
	EventOutcome determineEventOutcome(PossessionState possessionState, AttackEvent attackEvent, DefenseEvent defenseEvent);
};
