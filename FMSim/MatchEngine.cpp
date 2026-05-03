#include <string.h>
#include "MatchEngine.h"
#include "math.h"
#include <cstdlib>
#include <random>
#include <algorithm>

namespace {
    std::mt19937& randomGenerator() {
        static thread_local std::mt19937 generator{ std::random_device{}() };
        return generator;
    }

    double randomProbability() {
        static thread_local std::uniform_real_distribution distribution(0.0, 1.0);
        return distribution(randomGenerator());
    }
}

MatchEngine::MatchEngine() = default;

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
}

void MatchEngine::simulateFullMatch() {
    simulateStart();

    while (simulateNextEvent()) {
        // Match loop
    }
}

bool MatchEngine::simulateNextEvent() {
    // Main simulation logic here, the team who got the ball will
    // perform an action based on their tactics and its zone paths
    // with randomly chosen (and will be weighted later) path and event from the tactic
    // Also defending team will have a chance to perform a defensive action based
    // on their tactics and the attacking event

    if (totalNormalAvailableEventsCounter.empty()) {
        // No more events can be simulated in this match, end the simulation
        _eventLogs.emplace_back("Match simulation ended.");
        EventStruct fullTimeEvent{};
        fullTimeEvent.isOtherEvent = true;
        fullTimeEvent.elapsedSeconds = _elapsedSimulationSeconds;
        fullTimeEvent.otherEvent = OtherEvent::FullTime;
        if (!_events.empty()) {
            fullTimeEvent.possessionState = _events.back().possessionState;
            fullTimeEvent.currentZone = _events.back().currentZone;
            fullTimeEvent.fromZone = _events.back().currentZone;
            fullTimeEvent.toZone = _events.back().currentZone;
        }
        _events.push_back(fullTimeEvent);
        return false;
    }

    EventStruct nextEvent{};
    nextEvent.elapsedSeconds = _elapsedSimulationSeconds;

    // Current zone will be the same as the last event's zone
    nextEvent.currentZone = _events.back().currentZone;
    nextEvent.fromZone = _events.back().currentZone;
    nextEvent.toZone = _events.back().currentZone;
    nextEvent.possessionState = _events.back().possessionState;

    AttackEvent nextAttackEvent{};
    DefenseEvent nextDefenceEvent{};
    Zone tacticZone = getTacticZone(nextEvent.currentZone, nextEvent.possessionState);
    Zone tacticNextZone;

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

    _events.push_back(nextEvent);
    _eventLogs.push_back(
        actingTeam + " - " +
        toString(nextEvent.attackEvent) + " - " +
        toString(nextEvent.eventOutcome));

    totalNormalAvailableEventsCounter.pop(); // Decrease the counter for available events in this interval
    _elapsedSimulationSeconds += 5;

    return true;
}

void MatchEngine::determineGoalKeeperSave(EventOutcome& result) const {
    using enum EventOutcome;
    std::uniform_int_distribution distribution(1, 10000);

    const int roll = distribution(randomGenerator());

    constexpr int heldThreshold = 5008;
    constexpr int cornerThreshold = heldThreshold + 1883;
    constexpr int reboundThreshold = cornerThreshold + 1287;
    constexpr int reboundDefenseThreshold = reboundThreshold + 1390;
    // Remaining 4.32% probability is added to cleared for now

    // Determine the outcome of the goalkeeper save based on the probabilities

    if (roll <= heldThreshold) {
        result = GoalKeeperHeld;
    }
    else if (roll <= cornerThreshold) {
        result = CornerKick;
    }
    else if (roll <= reboundThreshold) {
        result = Rebound;
    }
    else if (roll <= reboundDefenseThreshold) {
        result = Cleared;
    }
    else {
        result = Cleared;
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
    _events.push_back(kickoffEvent);
}

bool MatchEngine::coinToss() {
    std::bernoulli_distribution distribution(0.5);
    if (distribution(randomGenerator())) {
        _eventLogs.push_back(_homeTeam.getName() + " wins the coin toss and will kick off.");
        return true; // Home team kicks off
    }
    else {
        _eventLogs.push_back(_awayTeam.getName() + " wins the coin toss and will kick off.");
        return false; // Away team kicks off
    }
}

EventOutcome MatchEngine::determineEventOutcome(PossessionState possessionState, AttackEvent attackEvent, DefenseEvent defenseEvent) const {
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
        double randomValue = randomProbability(); // Random value between 0 and 1

        if (randomValue < defenseSuccessChance) {
            return EventOutcome::Blocked; // Shot is blocked by the defense
        }

        // If the shot is not blocked, determine if it will be saved by the goalkeeper

        double goalkeepingSuccessChance = goalkeepingStrength / (goalkeepingStrength + attackerAttackStrength);
        randomValue = randomProbability(); // Random value between 0 and 1

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
    double randomValue = randomProbability(); // Random value between 0 and 1

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

const std::vector<EventStruct>& MatchEngine::getEvents() const {
    return _events;
}

Zone MatchEngine::mirrorZone(Zone zone) const {
    switch (zone) {
        using enum Zone;
    case H1:
        return A1;
    case H2:
        return A2;
    case H3:
        return A3;
    case A1:
        return H1;
    case A2:
        return H2;
    case A3:
        return H3;
    case M1:
    case M2:
    case M3:
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
        using enum PossessionState;
        using enum EventOutcome;

    case Success:
    case Rebound:
    case CornerKick:
        break;
    case Goal:
        if (nextEvent.possessionState == Home) {
            _homeScore++;
            nextEvent.possessionState = Away;
            nextEvent.currentZone = Zone::A2;
        }
        else {
            _awayScore++;
            nextEvent.possessionState = Home;
            nextEvent.currentZone = Zone::H2;
        }
        resetAttackState();
        break;
    case Saved:
    case GoalKeeperHeld:
    case Fail:
    case Blocked:
    case Cleared:
    case Out:
    case ThrownIn:
    case Fouled:
    default:
        nextEvent.possessionState =
            nextEvent.possessionState == Home ? Away : Home;
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
        using enum PossessionState;
    case Home:
        return _homeTeam.getName();
    case Away:
        return _awayTeam.getName();
    case None:
    default:
        return "None";
    }
}

std::string MatchEngine::toString(AttackEvent attackEvent) const {
    switch (attackEvent) {
        using enum AttackEvent;
    case ShortPass:
        return "Short Pass";
    case LongPass:
        return "Long Pass";
    case ThroughBall:
        return "Through Ball";
    case Dribble:
        return "Dribble";
    case Shoot:
        return "Shoot";
    case Cross:
        return "Cross";
    case Clearance:
        return "Clearance";
    default:
        return "Attack";
    }
}

std::string MatchEngine::toString(EventOutcome eventOutcome) const {
    switch (eventOutcome) {
        using enum EventOutcome;
    case Success:
        return "Success";
    case Fail:
        return "Fail";
    case Goal:
        return "Goal";
    case Saved:
        return "Saved";
    case Blocked:
        return "Blocked";
    case Cleared:
        return "Cleared";
    case CornerKick:
        return "Corner";
    case Rebound:
        return "Rebound";
    case GoalKeeperHeld:
        return "Held";
    case Out:
        return "Out";
    case ThrownIn:
        return "Throw In";
    case Fouled:
        return "Foul";
    default:
        return "Outcome";
    }
}
