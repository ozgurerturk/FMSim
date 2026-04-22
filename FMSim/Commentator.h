#pragma once

#include "MatchEngine.h"
#include <string>
#include <vector>

class Commentator
{
public:
	Commentator(std::string homeTeamName, std::string awayTeamName);

	std::vector<std::string> BuildCommentary(const MatchEngine& matchEngine) const;
	void SaveCommentary(const MatchEngine& matchEngine, const std::string& outputPath) const;

private:
	std::string _homeTeamName;
	std::string _awayTeamName;

	std::string FormatTimestamp(int elapsedSeconds) const;
	std::string DescribeOtherEvent(const EventStruct& eventStruct) const;
	std::string DescribeNormalEvent(const EventStruct& eventStruct) const;
	std::string BuildIntroLine(const MatchEngine& matchEngine, bool isHomeTeam) const;
	std::string AttackEventToString(AttackEvent attackEvent) const;
	std::string DefenseEventToString(DefenseEvent defenseEvent) const;
	std::string OtherEventToString(OtherEvent otherEvent) const;
	std::string OutcomeToString(EventOutcome eventOutcome) const;
	std::string PossessionToString(PossessionState possessionState) const;
	std::string ZoneToString(Zone zone) const;
	std::string TacticToString(TacticType tacticType) const;
	std::string DefenseTacticToString(DefenseTacticType tacticType) const;
};
