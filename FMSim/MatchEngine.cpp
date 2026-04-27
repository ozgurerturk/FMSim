#include <string.h>
#include "MatchEngine.h"
#include "math.h"
#include <cstdlib>

MatchEngine::MatchEngine() {
}

MatchEngine::MatchEngine(const Team& homeTeam, const Team& awayTeam, bool enableHomeAdvantage, int simulationTimeInMinutes)
    : _homeTeam(homeTeam),
    _awayTeam(awayTeam),
    _homeTeamStrength(homeTeam.getTeamStrength()),
    _awayTeamStrength(awayTeam.getTeamStrength()) {
    (void)enableHomeAdvantage;

    // Calculate the total number of events that can be simulated based on the simulation time
    int totalSimulationTimeInSeconds = simulationTimeInMinutes * 60;
    int eventIntervalInSeconds = 5; // We will simulate an event every 5 seconds
    int totalEvents = totalSimulationTimeInSeconds / eventIntervalInSeconds;
    for (int i = 0; i < totalEvents; ++i) {
        totalNormalAvailableEventsCounter.push(1); // We can simulate one normal event in each interval
    }
}

void MatchEngine::simulateStart() {
    // Coin toss to decide which team kicks off
    bool doesHomeTeamKicksOff = coinToss();
    kickOff(doesHomeTeamKicksOff);
    simulateNextEvent();
}

void MatchEngine::simulateNextEvent() {
    // Main simulation logic here, the team who got the ball will
    // perform an action based on their tactics and its zone paths
    // with randomly chosen (and wwill be weighted later) path and event from the tactic
    // Also defending team will have a chance to perform a defensive action based
    // on their tactics and the attacking event

    if (totalNormalAvailableEventsCounter.empty()) {
        // No more events can be simulated in this match, end the simulation
        _eventLogs.push_back("Match simulation ended.");
        events.push_back(EventStruct{ .isOtherEvent = true, .elapsedSeconds = _elapsedSimulationSeconds, .otherEvent = OtherEvent::FullTime });
        return;
    }

    EventStruct nextEvent{};
    nextEvent.elapsedSeconds = _elapsedSimulationSeconds;

    // Current zone will be the same as the last event's zone
    nextEvent.currentZone = events.back().currentZone;
    nextEvent.fromZone = events.back().currentZone;
    nextEvent.toZone = events.back().currentZone;
    nextEvent.possessionState = events.back().possessionState;

    AttackEvent nextAttackEvent{};
    DefenseEvent nextDefenceEvent{};
    Zone tacticZone = getTacticZone(nextEvent.currentZone, nextEvent.possessionState);
    Zone tacticNextZone = tacticZone;

    if (nextEvent.possessionState == PossessionState::Home) {
        nextAttackEvent = _homeTeam.getCurrentTeamTactic().StartAttack(tacticZone);
        tacticNextZone = _homeTeam.getCurrentTeamTactic().GetNextZone(tacticZone);
        nextDefenceEvent = _awayTeam.getCurrentTeamDefenseTactic().RespondToAttack(nextAttackEvent);
    }
    else {
        nextAttackEvent = _awayTeam.getCurrentTeamTactic().StartAttack(tacticZone);
        tacticNextZone = _awayTeam.getCurrentTeamTactic().GetNextZone(tacticZone);
        nextDefenceEvent = _homeTeam.getCurrentTeamDefenseTactic().RespondToAttack(nextAttackEvent);
    }

    nextEvent.attackEvent = nextAttackEvent;
    nextEvent.defenseEvent = nextDefenceEvent;
    nextEvent.toZone = getMatchZone(tacticNextZone, nextEvent.possessionState);
    nextEvent.currentZone = nextEvent.toZone;
    const std::string actingTeam = toString(nextEvent.possessionState);

    // Next event added, now for attack and defense interaction and outcome determination

    EventOutcome result = determineEventOutcome(nextEvent.possessionState, nextAttackEvent, nextDefenceEvent);

    if (result == EventOutcome::Saved) {
        determineGoalKeeperSave(result);
    }

    nextEvent.eventOutcome = result;
    applyEventOutcome(nextEvent);

    events.push_back(nextEvent);
    _eventLogs.push_back(
        actingTeam + " - " +
        toString(nextEvent.attackEvent) + " - " +
        toString(nextEvent.eventOutcome));

    totalNormalAvailableEventsCounter.pop(); // Decrease the counter for available events in this interval
    _elapsedSimulationSeconds += 5;

    simulateNextEvent(); // Simulate the next event after the current one is processed
}

void MatchEngine::determineGoalKeeperSave(EventOutcome& result) {
    double r = static_cast<double>(rand()) / RAND_MAX;

    double held = 0.5008;
    double corner = 0.1883;
    double rebound = 0.1287;
    double reboundDefense = 0.1390;
    double other = 0.0432;

    double total = held + corner + rebound + reboundDefense + other;

    held /= total;
    corner /= total;
    rebound /= total;
    reboundDefense /= total;
    other /= total;

    // Determine the outcome of the goalkeeper save based on the probabilities

    if (r < held) {
        result = EventOutcome::GoalKeeperHeld;
    }
    else if ((r -= held) < corner) {
        result = EventOutcome::CornerKick;
    }
    else if ((r -= corner) < rebound) {
        result = EventOutcome::Rebound;
    }
    else if ((r -= rebound) < reboundDefense) {
        result = EventOutcome::Cleared;
    }
}

void MatchEngine::kickOff(bool doesHomeTeamKicksOff) {
    if (doesHomeTeamKicksOff) {
        _eventLogs.push_back(_homeTeam.getName() + " kicks off.");
    }
    else {
        _eventLogs.push_back(_awayTeam.getName() + " kicks off.");
    }

    // Add kickoff event to the events vector and start the match simulation
    EventStruct kickoffEvent{};
    kickoffEvent.isOtherEvent = true;
    kickoffEvent.elapsedSeconds = 0;
    kickoffEvent.possessionState = doesHomeTeamKicksOff ? PossessionState::Home : PossessionState::Away;

    // Although ball is in the middle at the start,
    // we can consider it in the home or away half based on who kicks off
    // for simplicity and to give a starting point for the simulation.
    kickoffEvent.currentZone = doesHomeTeamKicksOff ? Zone::H2 : Zone::A2;
    kickoffEvent.fromZone = kickoffEvent.currentZone;
    kickoffEvent.toZone = kickoffEvent.currentZone;
    kickoffEvent.otherEvent = OtherEvent::Kickoff;
    resetAttackState();
    events.push_back(kickoffEvent);
}

bool MatchEngine::coinToss() {
    int tossResult = rand() % 2;
    if (tossResult == 0) {
        _eventLogs.push_back(_homeTeam.getName() + " wins the coin toss and will kick off.");
        return true; // Home team kicks off
    }
    else {
        _eventLogs.push_back(_awayTeam.getName() + " wins the coin toss and will kick off.");
        return false; // Away team kicks off
    }
}

EventOutcome MatchEngine::determineEventOutcome(PossessionState possessionState, AttackEvent attackEvent, DefenseEvent defenseEvent) {
    (void)defenseEvent;

    const TeamStrength& attackingTeamStrength =
        possessionState == PossessionState::Home ? _homeTeamStrength : _awayTeamStrength;
    const TeamStrength& defendingTeamStrength =
        possessionState == PossessionState::Home ? _awayTeamStrength : _homeTeamStrength;

    double attackerAttackStrength = attackingTeamStrength.getAttack();
    double defenseStrength = defendingTeamStrength.getDefense();
    double goalkeepingStrength = defendingTeamStrength.getGoalkeeping();

    if (attackEvent == AttackEvent::Shoot) {
        // Can be either an shoot on target but saved or goal if successful EventOutcome
        // EventOutcome is unsuccessful if the shot is off target or blocked by the defense

        // First determine if shoot will be intercepted or blocked by the defense
        double defenseSuccessChance = defenseStrength / (defenseStrength + attackerAttackStrength);
        double randomValue = static_cast<double>(rand()) / RAND_MAX; // Random value between 0 and 1

        if (randomValue < defenseSuccessChance) {
            return EventOutcome::Blocked; // Shot is blocked by the defense
        }

        // If the shot is not blocked, determine if it will be saved by the goalkeeper

        double goalkeepingSuccessChance = goalkeepingStrength / (goalkeepingStrength + attackerAttackStrength);
        randomValue = static_cast<double>(rand()) / RAND_MAX; // Random value between 0 and 1

        if (randomValue < goalkeepingSuccessChance) {
            //Saved by the keeper but is it a corner kick or tipped by the keeper and goes out for a throw in or is it a clear save?
            //Or is it a save that results in a rebound and another attack opportunity for the attacking team?

            return EventOutcome::Saved; // Shot is saved by the goalkeeper
        }
        else {
            return EventOutcome::Goal; // Shot results in a goal
        }
    }

    // For other attack events, we can determine success or failure based on the attack and defense strengths

    double attackSuccessChance = attackerAttackStrength / (attackerAttackStrength + defenseStrength);
    double randomValue = static_cast<double>(rand()) / RAND_MAX; // Random value between 0 and 1

    if (randomValue < attackSuccessChance) {
        return EventOutcome::Success; // Attack is successful
    }
    else {
        return EventOutcome::Fail; // Attack fails due to successful defense
    }
}

int MatchEngine::getHomeScore() const {
    return _homeScore;
}

int MatchEngine::getAwayScore() const {
    return _awayScore;
}

const std::string& MatchEngine::getHomeTeamName() const {
    return _homeTeam.getName();
}

const std::string& MatchEngine::getAwayTeamName() const {
    return _awayTeam.getName();
}

TeamStrength MatchEngine::getHomeTeamStrength() const {
    return _homeTeamStrength;
}

TeamStrength MatchEngine::getAwayTeamStrength() const {
    return _awayTeamStrength;
}

TacticType MatchEngine::getHomeTacticType() const {
    return _homeTeam.getCurrentTeamTacticName();
}

TacticType MatchEngine::getAwayTacticType() const {
    return _awayTeam.getCurrentTeamTacticName();
}

DefenseTacticType MatchEngine::getHomeDefenseTacticType() const {
    return _homeTeam.getCurrentTeamDefenseTacticName();
}

DefenseTacticType MatchEngine::getAwayDefenseTacticType() const {
    return _awayTeam.getCurrentTeamDefenseTacticName();
}

const std::vector<std::string>& MatchEngine::getEventLogs() const {
    return _eventLogs;
}

Zone MatchEngine::mirrorZone(Zone zone) const {
    switch (zone) {
    case Zone::H1:
        return Zone::A1;
    case Zone::H2:
        return Zone::A2;
    case Zone::H3:
        return Zone::A3;
    case Zone::A1:
        return Zone::H1;
    case Zone::A2:
        return Zone::H2;
    case Zone::A3:
        return Zone::H3;
    case Zone::M1:
    case Zone::M2:
    case Zone::M3:
    default:
        return zone;
    }
}

Zone MatchEngine::getTacticZone(Zone zone, PossessionState possessionState) const {
    return possessionState == PossessionState::Away ? mirrorZone(zone) : zone;
}

Zone MatchEngine::getMatchZone(Zone tacticZone, PossessionState possessionState) const {
    return possessionState == PossessionState::Away ? mirrorZone(tacticZone) : tacticZone;
}

void MatchEngine::applyEventOutcome(EventStruct& nextEvent) {
    switch (nextEvent.eventOutcome) {
    case EventOutcome::Success:
    case EventOutcome::Rebound:
    case EventOutcome::CornerKick:
        break;
    case EventOutcome::Goal:
        if (nextEvent.possessionState == PossessionState::Home) {
            _homeScore++;
            nextEvent.possessionState = PossessionState::Away;
            nextEvent.currentZone = Zone::A2;
        }
        else {
            _awayScore++;
            nextEvent.possessionState = PossessionState::Home;
            nextEvent.currentZone = Zone::H2;
        }
        resetAttackState();
        break;
    case EventOutcome::Saved:
    case EventOutcome::GoalKeeperHeld:
    case EventOutcome::Fail:
    case EventOutcome::Blocked:
    case EventOutcome::Cleared:
    case EventOutcome::Out:
    case EventOutcome::ThrownIn:
    case EventOutcome::Fouled:
    default:
        nextEvent.possessionState =
            nextEvent.possessionState == PossessionState::Home ? PossessionState::Away : PossessionState::Home;
        resetAttackState();
        break;
    }
}

void MatchEngine::resetAttackState() {
    _homeTeam.getCurrentTeamTactic().ResetAttack();
    _awayTeam.getCurrentTeamTactic().ResetAttack();
}

std::string MatchEngine::toString(PossessionState possessionState) const {
    switch (possessionState) {
    case PossessionState::Home:
        return _homeTeam.getName();
    case PossessionState::Away:
        return _awayTeam.getName();
    case PossessionState::None:
    default:
        return "None";
    }
}

std::string MatchEngine::toString(AttackEvent attackEvent) const {
    switch (attackEvent) {
    case AttackEvent::ShortPass:
        return "Short Pass";
    case AttackEvent::LongPass:
        return "Long Pass";
    case AttackEvent::ThroughBall:
        return "Through Ball";
    case AttackEvent::Dribble:
        return "Dribble";
    case AttackEvent::Shoot:
        return "Shoot";
    case AttackEvent::Cross:
        return "Cross";
    case AttackEvent::Clearance:
        return "Clearance";
    default:
        return "Attack";
    }
}

std::string MatchEngine::toString(EventOutcome eventOutcome) const {
    switch (eventOutcome) {
    case EventOutcome::Success:
        return "Success";
    case EventOutcome::Fail:
        return "Fail";
    case EventOutcome::Goal:
        return "Goal";
    case EventOutcome::Saved:
        return "Saved";
    case EventOutcome::Blocked:
        return "Blocked";
    case EventOutcome::Cleared:
        return "Cleared";
    case EventOutcome::CornerKick:
        return "Corner";
    case EventOutcome::Rebound:
        return "Rebound";
    case EventOutcome::GoalKeeperHeld:
        return "Held";
    case EventOutcome::Out:
        return "Out";
    case EventOutcome::ThrownIn:
        return "Throw In";
    case EventOutcome::Fouled:
        return "Foul";
    default:
        return "Outcome";
    }
}
