#include "Commentator.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

Commentator::Commentator(std::string homeTeamName, std::string awayTeamName)
	: _homeTeamName(std::move(homeTeamName)),
	  _awayTeamName(std::move(awayTeamName))
{
}

std::vector<std::string> Commentator::BuildCommentary(const MatchEngine& matchEngine) const
{
	std::vector<std::string> commentary;
	commentary.push_back("Mac anlatimi");
	commentary.push_back(_homeTeamName + " " + std::to_string(matchEngine.getHomeScore()) +
		" - " + std::to_string(matchEngine.getAwayScore()) + " " + _awayTeamName);
	commentary.push_back(BuildIntroLine(matchEngine, true));
	commentary.push_back(BuildIntroLine(matchEngine, false));
	commentary.push_back("");

	for (const auto& eventStruct : matchEngine.events)
	{
		const std::string prefix = "[" + FormatTimestamp(eventStruct.elapsedSeconds) + "] ";
		if (eventStruct.isOtherEvent)
		{
			commentary.push_back(prefix + DescribeOtherEvent(eventStruct));
		}
		else
		{
			commentary.push_back(prefix + DescribeNormalEvent(eventStruct));
		}
	}

	return commentary;
}

void Commentator::SaveCommentary(const MatchEngine& matchEngine, const std::string& outputPath) const
{
	std::ofstream outputFile(outputPath, std::ios::trunc);
	outputFile.setf(std::ios::unitbuf);

	for (const auto& line : BuildCommentary(matchEngine))
	{
		outputFile << line << '\n';
	}
}

std::string Commentator::FormatTimestamp(int elapsedSeconds) const
{
	std::ostringstream stream;
	const int minutes = elapsedSeconds / 60;
	const int seconds = elapsedSeconds % 60;
	stream << std::setw(2) << std::setfill('0') << minutes
		<< ":" << std::setw(2) << std::setfill('0') << seconds;
	return stream.str();
}

std::string Commentator::DescribeOtherEvent(const EventStruct& eventStruct) const
{
	switch (eventStruct.otherEvent)
	{
	case OtherEvent::Kickoff:
		return PossessionToString(eventStruct.possessionState) + " santra ile maca basladi.";
	case OtherEvent::FullTime:
		return "Hakem maci bitirdi.";
	default:
		return "Diger olay: " + OtherEventToString(eventStruct.otherEvent);
	}
}

std::string Commentator::DescribeNormalEvent(const EventStruct& eventStruct) const
{
	const std::string teamName = PossessionToString(eventStruct.possessionState);
	const std::string fromZoneName = ZoneToString(eventStruct.fromZone);
	const std::string toZoneName = ZoneToString(eventStruct.toZone);
	const std::string zoneFlow = fromZoneName + "'den " + toZoneName + "'ye";

	switch (eventStruct.eventOutcome)
	{
	case EventOutcome::Goal:
		return teamName + " " + zoneFlow + " gelirken firsati buldu ve topu aglara gonderdi.";
	case EventOutcome::CornerKick:
		return teamName + " " + zoneFlow + " yuklendi, savunmanin mudahalesiyle top kornere gitti.";
	case EventOutcome::GoalKeeperHeld:
		return teamName + " " + zoneFlow + " yokladi, kaleci topu iki hamlede kontrol etti.";
	case EventOutcome::Blocked:
		return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
			" denedi ama savunma " + DefenseEventToString(eventStruct.defenseEvent) + " ile duvari ordu.";
	case EventOutcome::Cleared:
		return teamName + " " + zoneFlow + " zorladi, savunma topu uzaklastirdi.";
	case EventOutcome::Success:
		return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
			" ile akini surdurdu.";
	case EventOutcome::Fail:
		return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
			" denedi ama savunma bunu bozdu.";
	default:
		return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
			" denedi, savunma " + DefenseEventToString(eventStruct.defenseEvent) +
			" ile karsilik verdi. Sonuc: " + OutcomeToString(eventStruct.eventOutcome) + ".";
	}
}

std::string Commentator::BuildIntroLine(const MatchEngine& matchEngine, bool isHomeTeam) const
{
	const TeamStrength strength = isHomeTeam ? matchEngine.getHomeTeamStrength() : matchEngine.getAwayTeamStrength();
	const std::string teamName = isHomeTeam ? _homeTeamName : _awayTeamName;
	const TacticType attackTactic = isHomeTeam ? matchEngine.getHomeTacticType() : matchEngine.getAwayTacticType();
	const DefenseTacticType defenseTactic = isHomeTeam ? matchEngine.getHomeDefenseTacticType() : matchEngine.getAwayDefenseTacticType();

	std::ostringstream stream;
	stream << teamName
		<< " baslangic duzeni: hucum " << TacticToString(attackTactic)
		<< ", savunma " << DefenseTacticToString(defenseTactic)
		<< ", guc ortalamalari A:" << std::fixed << std::setprecision(1) << strength.getAttack()
		<< " D:" << strength.getDefense()
		<< " G:" << strength.getGoalkeeping();
	return stream.str();
}

std::string Commentator::AttackEventToString(AttackEvent attackEvent) const
{
	switch (attackEvent)
	{
	case AttackEvent::ShortPass: return "kisa pas";
	case AttackEvent::LongPass: return "uzun pas";
	case AttackEvent::ThroughBall: return "ara pas";
	case AttackEvent::Dribble: return "dripling";
	case AttackEvent::Shoot: return "sut";
	case AttackEvent::Cross: return "orta";
	case AttackEvent::Clearance: return "uzaklastirma";
	default: return "atak";
	}
}

std::string Commentator::DefenseEventToString(DefenseEvent defenseEvent) const
{
	switch (defenseEvent)
	{
	case DefenseEvent::Tackle: return "mudahale";
	case DefenseEvent::Interception: return "pas arasi";
	case DefenseEvent::Block: return "blok";
	case DefenseEvent::HeadingClearance: return "kafa ile uzaklastirma";
	case DefenseEvent::SlidingTackle: return "kayarak mudahale";
	case DefenseEvent::Pressure: return "pres";
	case DefenseEvent::ForwardPress: return "onde baski";
	case DefenseEvent::GoalkeeperSave: return "kaleci kurtarisi";
	case DefenseEvent::GoalkeeperPunch: return "kaleci yumrugu";
	case DefenseEvent::GoalkeeperCatch: return "kaleci kontrolu";
	case DefenseEvent::Clearance: return "uzaklastirma";
	case DefenseEvent::Idle: return "bekleme";
	default: return "savunma aksiyonu";
	}
}

std::string Commentator::OtherEventToString(OtherEvent otherEvent) const
{
	switch (otherEvent)
	{
	case OtherEvent::Kickoff: return "santra";
	case OtherEvent::HalfTime: return "devre arasi";
	case OtherEvent::FullTime: return "mac sonu";
	case OtherEvent::Rebound: return "seken top";
	case OtherEvent::OwnGoal: return "kendi kalesine gol";
	case OtherEvent::Offside: return "ofsayt";
	case OtherEvent::Foul: return "faul";
	case OtherEvent::Injury: return "sakatlik";
	case OtherEvent::OutOfPlay: return "oyun disi";
	case OtherEvent::ThrowIn: return "taac";
	case OtherEvent::CornerKick: return "korner";
	default: return "diger olay";
	}
}

std::string Commentator::OutcomeToString(EventOutcome eventOutcome) const
{
	switch (eventOutcome)
	{
	case EventOutcome::Success: return "atak basarili";
	case EventOutcome::Fail: return "atak basarisiz";
	case EventOutcome::Goal: return "gol";
	case EventOutcome::Saved: return "kaleci kurtardi";
	case EventOutcome::Blocked: return "bloklandi";
	case EventOutcome::Cleared: return "uzaklastirildi";
	case EventOutcome::Out: return "top auta gitti";
	case EventOutcome::ThrownIn: return "top taca gitti";
	case EventOutcome::CornerKick: return "korner";
	case EventOutcome::GoalKeeperHeld: return "kaleci topu kontrol etti";
	case EventOutcome::Rebound: return "top sekti";
	case EventOutcome::Fouled: return "faul";
	default: return "belirsiz";
	}
}

std::string Commentator::PossessionToString(PossessionState possessionState) const
{
	switch (possessionState)
	{
	case PossessionState::Home: return _homeTeamName;
	case PossessionState::Away: return _awayTeamName;
	case PossessionState::None:
	default: return "Top bos";
	}
}

std::string Commentator::ZoneToString(Zone zone) const
{
	switch (zone)
	{
	case Zone::H1: return "H1";
	case Zone::H2: return "H2";
	case Zone::H3: return "H3";
	case Zone::M1: return "M1";
	case Zone::M2: return "M2";
	case Zone::M3: return "M3";
	case Zone::A1: return "A1";
	case Zone::A2: return "A2";
	case Zone::A3: return "A3";
	default: return "Bilinmeyen bolge";
	}
}

std::string Commentator::TacticToString(TacticType tacticType) const
{
	switch (tacticType)
	{
	case TacticType::Possession: return "Possession";
	case TacticType::WingPlay: return "Wing Play";
	case TacticType::LongBall: return "Long Ball";
	case TacticType::TikiTaka: return "Tiki Taka";
	case TacticType::CounterAttack: return "Counter Attack";
	default: return "Unknown";
	}
}

std::string Commentator::DefenseTacticToString(DefenseTacticType tacticType) const
{
	switch (tacticType)
	{
	case DefenseTacticType::Pressing: return "Pressing";
	case DefenseTacticType::ManMarking: return "Man Marking";
	case DefenseTacticType::ZonalMarking: return "Zonal Marking";
	case DefenseTacticType::CounterPressing: return "Counter Pressing";
	default: return "Unknown";
	}
}
