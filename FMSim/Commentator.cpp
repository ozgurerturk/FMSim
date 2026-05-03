#include "Commentator.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <format>

Commentator::Commentator(std::string homeTeamName, std::string awayTeamName)
    : _homeTeamName(std::move(homeTeamName)),
    _awayTeamName(std::move(awayTeamName)) {
}

std::vector<std::string> Commentator::BuildCommentary(const MatchEngine& matchEngine) const {
    std::vector<std::string> commentary;
    commentary.emplace_back("Mac anlatimi");
    commentary.push_back(std::format("{} {} - {} {}", _homeTeamName, matchEngine.getHomeScore(), matchEngine.getAwayScore(), _awayTeamName));
    commentary.push_back(BuildIntroLine(matchEngine, true));
    commentary.push_back(BuildIntroLine(matchEngine, false));
    commentary.emplace_back("");

    for (const auto& eventStruct : matchEngine.getEvents()) {
        const std::string prefix = "[" + FormatTimestamp(eventStruct.elapsedSeconds) + "] ";
        if (eventStruct.isOtherEvent) {
            commentary.push_back(prefix + DescribeOtherEvent(eventStruct));
        }
        else {
            commentary.push_back(prefix + DescribeNormalEvent(eventStruct));
        }
    }

    return commentary;
}

void Commentator::SaveCommentary(const MatchEngine& matchEngine, const std::string& outputPath) const {
    std::ofstream outputFile(outputPath, std::ios::trunc);
    outputFile.setf(std::ios::unitbuf);

    for (const auto& line : BuildCommentary(matchEngine)) {
        outputFile << line << '\n';
    }
}

std::string Commentator::FormatTimestamp(int elapsedSeconds) const {
    std::string timeStamp;

    const int minutes = elapsedSeconds / 60;
    const int seconds = elapsedSeconds % 60;

    timeStamp = std::format("{:02}:{:02}", minutes, seconds);

    return timeStamp;
}

std::string Commentator::DescribeOtherEvent(const EventStruct& eventStruct) const {
    switch (eventStruct.otherEvent) {
    case OtherEvent::Kickoff:
        return PossessionToString(eventStruct.possessionState) + " santra ile maca basladi.";
    case OtherEvent::FullTime:
        return "Hakem maci bitirdi.";
    default:
        return "Diger olay: " + OtherEventToString(eventStruct.otherEvent);
    }
}

std::string Commentator::DescribeNormalEvent(const EventStruct& eventStruct) const {
    const std::string teamName = PossessionToString(eventStruct.possessionState);
    const std::string fromZoneName = ZoneToString(eventStruct.fromZone);
    const std::string toZoneName = ZoneToString(eventStruct.toZone);
    const std::string zoneFlow = fromZoneName + "'den " + toZoneName + "'ye";

    switch (eventStruct.eventOutcome) {
        using enum EventOutcome;
    case Goal:
        return teamName + " " + zoneFlow + " gelirken firsati buldu ve topu aglara gonderdi.";
    case CornerKick:
        return teamName + " " + zoneFlow + " yuklendi, savunmanin mudahalesiyle top kornere gitti.";
    case GoalKeeperHeld:
        return teamName + " " + zoneFlow + " yokladi, kaleci topu iki hamlede kontrol etti.";
    case Blocked:
        return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
            " denedi ama savunma " + DefenseEventToString(eventStruct.defenseEvent) + " ile duvari ordu.";
    case Cleared:
        return teamName + " " + zoneFlow + " zorladi, savunma topu uzaklastirdi.";
    case Success:
        return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
            " ile akini surdurdu.";
    case Fail:
        return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
            " denedi ama savunma bunu bozdu.";
    default:
        return teamName + " " + zoneFlow + " " + AttackEventToString(eventStruct.attackEvent) +
            " denedi, savunma " + DefenseEventToString(eventStruct.defenseEvent) +
            " ile karsilik verdi. Sonuc: " + OutcomeToString(eventStruct.eventOutcome) + ".";
    }
}

std::string Commentator::BuildIntroLine(const MatchEngine& matchEngine, bool isHomeTeam) const {
    const TeamStrength strength = isHomeTeam ? matchEngine.getHomeTeamStrength() : matchEngine.getAwayTeamStrength();
    const std::string teamName = isHomeTeam ? _homeTeamName : _awayTeamName;
    const TacticType attackTactic = isHomeTeam ? matchEngine.getHomeTacticType() : matchEngine.getAwayTacticType();
    const DefenseTacticType defenseTactic = isHomeTeam ? matchEngine.getHomeDefenseTacticType() : matchEngine.getAwayDefenseTacticType();

    std::string introLine = std::format("{} starting information: attack tactic {}, defense tactic {} \n strength: A:{} D:{} G:{}"
        , teamName, TacticToString(attackTactic), DefenseTacticToString(defenseTactic), strength.getAttack(), strength.getDefense(), strength.getGoalkeeping());

    return introLine;
}

std::string Commentator::AttackEventToString(AttackEvent attackEvent) const {
    switch (attackEvent) {
        using enum AttackEvent;
    case ShortPass: return "kisa pas";
    case LongPass: return "uzun pas";
    case ThroughBall: return "ara pas";
    case Dribble: return "dripling";
    case Shoot: return "sut";
    case Cross: return "orta";
    case Clearance: return "uzaklastirma";
    default: return "atak";
    }
}

std::string Commentator::DefenseEventToString(DefenseEvent defenseEvent) const {
    switch (defenseEvent) {
        using enum DefenseEvent;
    case Tackle: return "mudahale";
    case Interception: return "pas arasi";
    case Block: return "blok";
    case HeadingClearance: return "kafa ile uzaklastirma";
    case SlidingTackle: return "kayarak mudahale";
    case Pressure: return "pres";
    case ForwardPress: return "onde baski";
    case GoalkeeperSave: return "kaleci kurtarisi";
    case GoalkeeperPunch: return "kaleci yumrugu";
    case GoalkeeperCatch: return "kaleci kontrolu";
    case Clearance: return "uzaklastirma";
    case Idle: return "bekleme";
    default: return "savunma aksiyonu";
    }
}

std::string Commentator::OtherEventToString(OtherEvent otherEvent) const {
    switch (otherEvent) {
        using enum OtherEvent;
    case Kickoff: return "santra";
    case HalfTime: return "devre arasi";
    case FullTime: return "mac sonu";
    case Rebound: return "seken top";
    case OwnGoal: return "kendi kalesine gol";
    case Offside: return "ofsayt";
    case Foul: return "faul";
    case Injury: return "sakatlik";
    case OutOfPlay: return "oyun disi";
    case ThrowIn: return "taac";
    case CornerKick: return "korner";
    default: return "diger olay";
    }
}

std::string Commentator::OutcomeToString(EventOutcome eventOutcome) const {
    switch (eventOutcome) {
        using enum EventOutcome;
    case Success: return "atak basarili";
    case Fail: return "atak basarisiz";
    case Goal: return "gol";
    case Saved: return "kaleci kurtardi";
    case Blocked: return "bloklandi";
    case Cleared: return "uzaklastirildi";
    case Out: return "top auta gitti";
    case ThrownIn: return "top taca gitti";
    case CornerKick: return "korner";
    case GoalKeeperHeld: return "kaleci topu kontrol etti";
    case Rebound: return "top sekti";
    case Fouled: return "faul";
    default: return "belirsiz";
    }
}

std::string Commentator::PossessionToString(PossessionState possessionState) const {
    switch (possessionState) {
        using enum PossessionState;
    case Home: return _homeTeamName;
    case Away: return _awayTeamName;
    case None:
    default: return "Top bos";
    }
}

std::string Commentator::ZoneToString(Zone zone) const {
    switch (zone) {
        using enum Zone;
    case H1: return "H1";
    case H2: return "H2";
    case H3: return "H3";
    case M1: return "M1";
    case M2: return "M2";
    case M3: return "M3";
    case A1: return "A1";
    case A2: return "A2";
    case A3: return "A3";
    default: return "Bilinmeyen bolge";
    }
}

std::string Commentator::TacticToString(TacticType tacticType) const {
    switch (tacticType) {
        using enum TacticType;
    case Possession: return "Possession";
    case WingPlay: return "Wing Play";
    case LongBall: return "Long Ball";
    case TikiTaka: return "Tiki Taka";
    case CounterAttack: return "Counter Attack";
    default: return "Unknown";
    }
}

std::string Commentator::DefenseTacticToString(DefenseTacticType tacticType) const {
    switch (tacticType) {
        using enum DefenseTacticType;
    case Pressing: return "Pressing";
    case ManMarking: return "Man Marking";
    case ZonalMarking: return "Zonal Marking";
    case CounterPressing: return "Counter Pressing";
    default: return "Unknown";
    }
}
