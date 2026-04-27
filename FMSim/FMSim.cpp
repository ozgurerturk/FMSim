#pragma comment(lib, "Comdlg32.lib")

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <array>
#include <filesystem>
#include <optional>
#include <windows.h>
#include <commdlg.h>
#include "Commentator.h"
#include "MatchEngine.h"
#include "TeamJsonLoader.h"

namespace {
    struct MatchOutputPaths {
        std::filesystem::path debugPath;
        std::filesystem::path eventPath;
        std::filesystem::path commentaryPath;
    };

    struct MatchResultSummary {
        int homeScore = 0;
        int awayScore = 0;
    };

    std::vector<Player> createPlayers(const std::string& prefix, int baseAttack, int baseDefense, int baseGoalkeeping) {
        return
        {
            Player(prefix + " GK", Position::Goalkeeper, Attributes(12, 8, 10, 72, 50, 15, 10, 8, 55, baseGoalkeeping, baseGoalkeeping)),
            Player(prefix + " RB", Position::Defender, Attributes(baseDefense, 45, 66, 78, 58, 68, 42, 30, 64, 58, 62)),
            Player(prefix + " CB1", Position::Defender, Attributes(baseDefense + 4, 35, 54, 76, 52, 74, 30, 28, 74, 72, 68)),
            Player(prefix + " CB2", Position::Defender, Attributes(baseDefense + 2, 36, 52, 75, 50, 72, 28, 28, 76, 74, 66)),
            Player(prefix + " LB", Position::Defender, Attributes(baseDefense - 1, 48, 67, 78, 60, 66, 44, 32, 64, 60, 63)),
            Player(prefix + " DM", Position::Midfielder, Attributes(62, 58, 60, 82, 68, 64, 55, 44, 68, 62, 64)),
            Player(prefix + " CM1", Position::Midfielder, Attributes(58, 66, 64, 84, 74, 58, 64, 56, 66, 60, 65)),
            Player(prefix + " CM2", Position::Midfielder, Attributes(56, 68, 65, 83, 76, 54, 66, 58, 64, 58, 66)),
            Player(prefix + " RW", Position::Attacker, Attributes(34, baseAttack, 78, 79, 68, 32, 74, 66, 60, 58, 63)),
            Player(prefix + " ST", Position::Attacker, Attributes(30, baseAttack + 4, 74, 77, 60, 28, 72, 74, 72, 68, 64)),
            Player(prefix + " LW", Position::Attacker, Attributes(34, baseAttack - 1, 79, 79, 69, 30, 75, 67, 59, 57, 63))
        };
    }

    std::string tacticToString(TacticType tacticType) {
        switch (tacticType) {
        case TacticType::Possession: return "Possession";
        case TacticType::WingPlay: return "Wing Play";
        case TacticType::LongBall: return "Long Ball";
        case TacticType::TikiTaka: return "Tiki Taka";
        case TacticType::CounterAttack: return "Counter Attack";
        default: return "Unknown";
        }
    }

    std::string defenseTacticToString(DefenseTacticType tacticType) {
        switch (tacticType) {
        case DefenseTacticType::Pressing: return "Pressing";
        case DefenseTacticType::ManMarking: return "Man Marking";
        case DefenseTacticType::ZonalMarking: return "Zonal Marking";
        case DefenseTacticType::CounterPressing: return "Counter Pressing";
        default: return "Unknown";
        }
    }

    std::string zoneToString(Zone zone) {
        switch (zone) {
        case Zone::H1: return "H1";
        case Zone::H2: return "H2";
        case Zone::H3: return "H3";
        case Zone::M1: return "M1";
        case Zone::M2: return "M2";
        case Zone::M3: return "M3";
        case Zone::A1: return "A1";
        case Zone::A2: return "A2";
        case Zone::A3: return "A3";
        default: return "Unknown";
        }
    }

    std::string attackEventToString(AttackEvent attackEvent) {
        switch (attackEvent) {
        case AttackEvent::ShortPass: return "Short Pass";
        case AttackEvent::LongPass: return "Long Pass";
        case AttackEvent::ThroughBall: return "Through Ball";
        case AttackEvent::Dribble: return "Dribble";
        case AttackEvent::Shoot: return "Shoot";
        case AttackEvent::Cross: return "Cross";
        case AttackEvent::Clearance: return "Clearance";
        default: return "Attack";
        }
    }

    std::string defenseEventToString(DefenseEvent defenseEvent) {
        switch (defenseEvent) {
        case DefenseEvent::Tackle: return "Tackle";
        case DefenseEvent::Interception: return "Interception";
        case DefenseEvent::Block: return "Block";
        case DefenseEvent::HeadingClearance: return "Heading Clearance";
        case DefenseEvent::SlidingTackle: return "Sliding Tackle";
        case DefenseEvent::Pressure: return "Pressure";
        case DefenseEvent::ForwardPress: return "Forward Press";
        case DefenseEvent::GoalkeeperSave: return "Goalkeeper Save";
        case DefenseEvent::GoalkeeperPunch: return "Goalkeeper Punch";
        case DefenseEvent::GoalkeeperCatch: return "Goalkeeper Catch";
        case DefenseEvent::Clearance: return "Clearance";
        case DefenseEvent::Idle: return "Idle";
        default: return "Defense";
        }
    }

    std::string outcomeToString(EventOutcome eventOutcome) {
        switch (eventOutcome) {
        case EventOutcome::Success: return "Success";
        case EventOutcome::Fail: return "Fail";
        case EventOutcome::Goal: return "Goal";
        case EventOutcome::Saved: return "Saved";
        case EventOutcome::Blocked: return "Blocked";
        case EventOutcome::Cleared: return "Cleared";
        case EventOutcome::Out: return "Out";
        case EventOutcome::ThrownIn: return "Thrown In";
        case EventOutcome::CornerKick: return "Corner";
        case EventOutcome::GoalKeeperHeld: return "Held";
        case EventOutcome::Rebound: return "Rebound";
        case EventOutcome::Fouled: return "Fouled";
        default: return "Outcome";
        }
    }

    std::string otherEventToString(OtherEvent otherEvent) {
        switch (otherEvent) {
        case OtherEvent::Kickoff: return "Kickoff";
        case OtherEvent::HalfTime: return "Half Time";
        case OtherEvent::FullTime: return "Full Time";
        case OtherEvent::Rebound: return "Rebound";
        case OtherEvent::OwnGoal: return "Own Goal";
        case OtherEvent::Offside: return "Offside";
        case OtherEvent::Foul: return "Foul";
        case OtherEvent::Injury: return "Injury";
        case OtherEvent::OutOfPlay: return "Out Of Play";
        case OtherEvent::ThrowIn: return "Throw In";
        case OtherEvent::CornerKick: return "Corner Kick";
        default: return "Other Event";
        }
    }

    std::string teamNameForEvent(PossessionState possessionState, const MatchEngine& engine) {
        switch (possessionState) {
        case PossessionState::Home: return engine.getHomeTeamName();
        case PossessionState::Away: return engine.getAwayTeamName();
        case PossessionState::None:
        default: return "None";
        }
    }

    std::string eventTimestamp(int elapsedSeconds) {
        std::ostringstream stream;
        const int minutes = elapsedSeconds / 60;
        const int seconds = elapsedSeconds % 60;
        stream << "[" << std::setw(2) << std::setfill('0') << minutes
            << ":" << std::setw(2) << std::setfill('0') << seconds << "]";
        return stream.str();
    }

    std::string detailedEventLine(const EventStruct& eventStruct, const MatchEngine& engine) {
        const std::string prefix = eventTimestamp(eventStruct.elapsedSeconds);
        const std::string teamName = teamNameForEvent(eventStruct.possessionState, engine);
        const std::string fromZoneName = zoneToString(eventStruct.fromZone);
        const std::string toZoneName = zoneToString(eventStruct.toZone);

        if (eventStruct.isOtherEvent) {
            return prefix + " " + teamName + " | Zone: " + fromZoneName + " -> " + toZoneName + " | Other: " + otherEventToString(eventStruct.otherEvent);
        }

        return prefix + " " + teamName
            + " | Zone: " + fromZoneName + " -> " + toZoneName
            + " | Attack: " + attackEventToString(eventStruct.attackEvent)
            + " | Defense: " + defenseEventToString(eventStruct.defenseEvent)
            + " | Outcome: " + outcomeToString(eventStruct.eventOutcome);
    }

    std::string scoreboardLineAtEvent(const MatchEngine& engine, std::size_t eventIndex) {
        int homeScore = 0;
        int awayScore = 0;

        for (std::size_t index = 0; index <= eventIndex && index < engine.events.size(); ++index) {
            const EventStruct& currentEvent = engine.events[index];
            if (!currentEvent.isOtherEvent || currentEvent.eventOutcome != EventOutcome::Goal) {
                continue;
            }

            if (currentEvent.possessionState == PossessionState::Away) {
                homeScore++;
            }
            else if (currentEvent.possessionState == PossessionState::Home) {
                awayScore++;
            }
        }

        std::ostringstream stream;
        stream << engine.getHomeTeamName() << " " << homeScore
            << " - " << awayScore << " " << engine.getAwayTeamName();
        return stream.str();
    }

    std::string possessionShortName(PossessionState possessionState) {
        switch (possessionState) {
        case PossessionState::Home: return "H";
        case PossessionState::Away: return "A";
        case PossessionState::None:
        default: return "-";
        }
    }

    std::pair<int, int> zoneToGridPosition(Zone zone) {
        switch (zone) {
        case Zone::H1: return { 0, 0 };
        case Zone::H2: return { 0, 1 };
        case Zone::H3: return { 0, 2 };
        case Zone::M1: return { 1, 0 };
        case Zone::M2: return { 1, 1 };
        case Zone::M3: return { 1, 2 };
        case Zone::A1: return { 2, 0 };
        case Zone::A2: return { 2, 1 };
        case Zone::A3: return { 2, 2 };
        default: return { 1, 1 };
        }
    }

    std::string padCell(const std::string& text) {
        if (text.size() >= 8) {
            return text.substr(0, 8);
        }

        return text + std::string(8 - text.size(), ' ');
    }

    void printZoneReplayFrame(const EventStruct& eventStruct, const MatchEngine& engine, std::size_t eventIndex, const std::string& commentaryLine) {
        std::array<std::array<std::string, 3>, 3> cells =
        { {
            { "H1", "H2", "H3" },
            { "M1", "M2", "M3" },
            { "A1", "A2", "A3" }
        } };

        const auto [row, col] = zoneToGridPosition(eventStruct.toZone);
        cells[row][col] += " O(" + possessionShortName(eventStruct.possessionState) + ")";

        std::cout << std::string(30, '\n');
        std::cout << "Replay " << eventTimestamp(eventStruct.elapsedSeconds) << '\n';
        std::cout << "Skor: " << scoreboardLineAtEvent(engine, eventIndex) << '\n';
        std::cout << "Top: " << zoneToString(eventStruct.toZone)
            << " | Akis: " << zoneToString(eventStruct.fromZone) << " -> " << zoneToString(eventStruct.toZone) << '\n';
        std::cout << "Topa sahip: " << teamNameForEvent(eventStruct.possessionState, engine) << "\n\n";

        for (int currentRow = 0; currentRow < 3; ++currentRow) {
            std::cout << "+----------+----------+----------+\n";
            std::cout << "| " << padCell(cells[currentRow][0])
                << " | " << padCell(cells[currentRow][1])
                << " | " << padCell(cells[currentRow][2]) << " |\n";
        }
        std::cout << "+----------+----------+----------+\n\n";
        std::cout << commentaryLine << "\n\n";
        std::cout << "Devam etmek icin Enter'a basin...";
        std::string discard;
        std::getline(std::cin, discard);
    }

    void replayMatchInConsole(const MatchEngine& engine, const std::vector<std::string>& commentaryLines) {
        if (engine.events.empty()) {
            return;
        }

        std::cout << "\nReplay basliyor. Her aksiyon icin Enter'a basin.\n";
        std::cout << "Baslamak icin Enter'a basin...";
        std::string discard;
        std::getline(std::cin, discard);

        const std::size_t commentaryOffset = 5;
        for (std::size_t index = 0; index < engine.events.size(); ++index) {
            std::string commentaryLine = "Anlatim bulunamadi.";
            const std::size_t commentaryIndex = commentaryOffset + index;
            if (commentaryIndex < commentaryLines.size()) {
                commentaryLine = commentaryLines[commentaryIndex];
            }

            printZoneReplayFrame(engine.events[index], engine, index, commentaryLine);
        }
    }

    std::string teamSummaryLine(const MatchEngine& engine, bool isHomeTeam) {
        const TeamStrength strength = isHomeTeam ? engine.getHomeTeamStrength() : engine.getAwayTeamStrength();
        const std::string teamName = isHomeTeam ? engine.getHomeTeamName() : engine.getAwayTeamName();
        const TacticType attackTactic = isHomeTeam ? engine.getHomeTacticType() : engine.getAwayTacticType();
        const DefenseTacticType defenseTactic = isHomeTeam ? engine.getHomeDefenseTacticType() : engine.getAwayDefenseTacticType();

        std::ostringstream stream;
        stream << teamName
            << " | hucum taktik: " << tacticToString(attackTactic)
            << " | savunma taktik: " << defenseTacticToString(defenseTactic)
            << " | guc A:" << std::fixed << std::setprecision(1) << strength.getAttack()
            << " D:" << strength.getDefense()
            << " G:" << strength.getGoalkeeping();
        return stream.str();
    }

    std::filesystem::path buildUniqueOutputPath(
        const std::filesystem::path& directory,
        const std::string& baseName,
        const std::string& suffix,
        const std::string& extension) {
        const std::string stem = baseName + "_" + suffix;
        std::filesystem::path candidate = directory / (stem + extension);
        if (!std::filesystem::exists(candidate)) {
            return candidate;
        }

        for (int index = 1; ; ++index) {
            candidate = directory / (stem + "(" + std::to_string(index) + ")" + extension);
            if (!std::filesystem::exists(candidate)) {
                return candidate;
            }
        }
    }

    MatchOutputPaths buildMatchOutputPaths(const std::filesystem::path& logsDirectory, const std::string& suffix) {
        return MatchOutputPaths
        {
            buildUniqueOutputPath(logsDirectory, "runtime_debug", suffix, ".txt"),
            buildUniqueOutputPath(logsDirectory, "eventlog", suffix, ".txt"),
            buildUniqueOutputPath(logsDirectory, "match_commentary", suffix, ".txt")
        };
    }

    std::filesystem::path buildSummaryOutputPath(const std::filesystem::path& logsDirectory, const std::string& suffix) {
        return buildUniqueOutputPath(logsDirectory, "results", suffix + "_summary", ".txt");
    }

    int promptSimulationMinutes() {
        std::cout << "Mac suresini secin (6 / 12 / 24): ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "24") {
            return 24;
        }

        if (input == "12") {
            return 12;
        }

        return 6;
    }

    int promptSimulationCount() {
        std::cout << "Kac mac simule edilsin? ";
        std::string input;
        std::getline(std::cin, input);

        try {
            const int parsedValue = std::stoi(input);
            return parsedValue > 0 ? parsedValue : 1;
        }
        catch (...) {
            return 1;
        }
    }

    std::optional<std::string> readEnvironmentVariable(const char* variableName) {
        char* value = nullptr;
        std::size_t length = 0;
        if (_dupenv_s(&value, &length, variableName) != 0 || value == nullptr || length == 0) {
            return std::nullopt;
        }

        const std::string result(value);
        free(value);
        return result;
    }

    std::optional<std::filesystem::path> readEnvironmentPathVariable(const wchar_t* variableName) {
        wchar_t* value = nullptr;
        std::size_t length = 0;
        if (_wdupenv_s(&value, &length, variableName) != 0 || value == nullptr || length == 0) {
            return std::nullopt;
        }

        const std::filesystem::path result(value);
        free(value);
        return result;
    }

    bool isNonInteractive();

    std::optional<std::filesystem::path> selectTeamJsonFile(const char* title) {
        const std::optional<std::filesystem::path> homeTeamEnv = readEnvironmentPathVariable(L"FMSIM_HOME_JSON");
        const std::optional<std::filesystem::path> awayTeamEnv = readEnvironmentPathVariable(L"FMSIM_AWAY_JSON");
        if (std::string(title) == "Choose Home Team JSON" && homeTeamEnv.has_value()) {
            const std::filesystem::path envPath(homeTeamEnv.value());
            if (std::filesystem::exists(envPath)) {
                return envPath;
            }
        }
        if (std::string(title) == "Choose Away Team JSON" && awayTeamEnv.has_value()) {
            const std::filesystem::path envPath(awayTeamEnv.value());
            if (std::filesystem::exists(envPath)) {
                return envPath;
            }
        }

        if (isNonInteractive()) {
            return std::nullopt;
        }

        OPENFILENAMEA f = { 0 };
        f.lStructSize = sizeof(OPENFILENAMEA);

        char buff[_MAX_PATH]{};
        char initialDirectory[_MAX_PATH]{};
        const std::filesystem::path teamsDirectory = std::filesystem::current_path() / "Teams";
        const std::string initialDirectoryText = teamsDirectory.string();
        strncpy_s(initialDirectory, initialDirectoryText.c_str(), _TRUNCATE);

        f.lpstrFilter = "JSON Files\0*.json\0All Files\0*.*\0";
        f.lpstrTitle = title;
        f.lpstrInitialDir = initialDirectory;
        f.nMaxFile = sizeof(buff);
        f.lpstrFile = buff;

        if (GetOpenFileNameA(&f) == TRUE) {
            return std::filesystem::path(buff);
        }

        return std::nullopt;
    }

    bool shouldSkipReplay() {
        const std::optional<std::string> skipReplay = readEnvironmentVariable("FMSIM_SKIP_REPLAY");
        return skipReplay.has_value() && skipReplay.value() == "1";
    }

    bool isNonInteractive() {
        const std::optional<std::string> nonInteractive = readEnvironmentVariable("FMSIM_NON_INTERACTIVE");
        return nonInteractive.has_value() && nonInteractive.value() == "1";
    }
}

int main() {
    const std::optional<std::filesystem::path> homeTeamPath = selectTeamJsonFile("Choose Home Team JSON");
    if (!homeTeamPath.has_value()) {
        std::cerr << "Home team selection cancelled.\n";
        return 1;
    }

    const std::optional<std::filesystem::path> awayTeamPath = selectTeamJsonFile("Choose Away Team JSON");
    if (!awayTeamPath.has_value()) {
        std::cerr << "Away team selection cancelled.\n";
        return 1;
    }

    const int simulationMinutes = promptSimulationMinutes();
    const int simulationCount = promptSimulationCount();
    const std::string suffix = std::to_string(simulationMinutes);
    const std::filesystem::path logsDirectory = "Logs";
    std::filesystem::create_directories(logsDirectory);
    const std::filesystem::path summaryPath = buildSummaryOutputPath(logsDirectory, suffix);

    try {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        std::vector<MatchResultSummary> matchResults;
        matchResults.reserve(simulationCount);
        std::string homeTeamName;
        std::string awayTeamName;

        for (int matchIndex = 0; matchIndex < simulationCount; ++matchIndex) {
            const MatchOutputPaths outputPaths = buildMatchOutputPaths(logsDirectory, suffix);
            std::ofstream debugFile(outputPaths.debugPath, std::ios::trunc);
            std::ofstream eventFile(outputPaths.eventPath, std::ios::trunc);
            debugFile.setf(std::ios::unitbuf);
            eventFile.setf(std::ios::unitbuf);

            debugFile << "main started" << std::endl;
            debugFile << "rng seeded" << std::endl;

            Team homeTeam = loadTeamFromJsonFile(homeTeamPath.value());
            Team awayTeam = loadTeamFromJsonFile(awayTeamPath.value());
            if (homeTeamName.empty()) {
                homeTeamName = homeTeam.getName();
                awayTeamName = awayTeam.getName();
            }
            debugFile << "teams created" << std::endl;

            MatchEngine engine(homeTeam, awayTeam, true, simulationMinutes);
            debugFile << "engine created" << std::endl;
            engine.simulateStart();
            debugFile << "simulation finished" << std::endl;
            debugFile << "event logs count: " << engine.getEventLogs().size() << std::endl;
            debugFile << "score: " << engine.getHomeScore() << "-" << engine.getAwayScore() << std::endl;
            debugFile << teamSummaryLine(engine, true) << std::endl;
            debugFile << teamSummaryLine(engine, false) << std::endl;
            for (const auto& eventStruct : engine.events) {
                debugFile << "log: " << detailedEventLine(eventStruct, engine) << std::endl;
            }

            eventFile << simulationMinutes << " dakikalik mac sonucu" << std::endl;
            eventFile << homeTeam.getName() << " " << engine.getHomeScore()
                << " - " << engine.getAwayScore() << " " << awayTeam.getName() << std::endl << std::endl;
            eventFile << teamSummaryLine(engine, true) << std::endl;
            eventFile << teamSummaryLine(engine, false) << std::endl << std::endl;

            for (const auto& eventStruct : engine.events) {
                eventFile << detailedEventLine(eventStruct, engine) << std::endl;
            }

            Commentator commentator(homeTeam.getName(), awayTeam.getName());
            const std::vector<std::string> commentaryLines = commentator.BuildCommentary(engine);
            commentator.SaveCommentary(engine, outputPaths.commentaryPath.string());
            debugFile << "commentator created from same match events" << std::endl;
            debugFile << "commentary saved to " << outputPaths.commentaryPath.string() << std::endl;
            debugFile << "stdout write completed" << std::endl;

            std::cout << "[" << (matchIndex + 1) << "/" << simulationCount << "] "
                << homeTeam.getName() << " " << engine.getHomeScore()
                << " - " << engine.getAwayScore() << " " << awayTeam.getName() << '\n';

            matchResults.push_back(MatchResultSummary{ engine.getHomeScore(), engine.getAwayScore() });

            if (simulationCount == 1 && !shouldSkipReplay()) {
                std::cout << '\n';
                for (const auto& eventStruct : engine.events) {
                    std::cout << detailedEventLine(eventStruct, engine) << '\n';
                }
                replayMatchInConsole(engine, commentaryLines);
            }
        }

        std::ofstream summaryFile(summaryPath, std::ios::trunc);
        int homeWins = 0;
        int awayWins = 0;
        int draws = 0;
        int totalHomeGoals = 0;
        int totalAwayGoals = 0;

        summaryFile << simulationCount << " maclik " << simulationMinutes << " dakikalik seri ozeti" << std::endl;
        summaryFile << "Takimlar: " << homeTeamName << " vs " << awayTeamName << std::endl << std::endl;

        for (std::size_t index = 0; index < matchResults.size(); ++index) {
            const MatchResultSummary& result = matchResults[index];
            totalHomeGoals += result.homeScore;
            totalAwayGoals += result.awayScore;

            if (result.homeScore > result.awayScore) {
                homeWins++;
            }
            else if (result.homeScore < result.awayScore) {
                awayWins++;
            }
            else {
                draws++;
            }

            summaryFile << "[" << (index + 1) << "] " << homeTeamName << " "
                << result.homeScore << " - " << result.awayScore
                << " " << awayTeamName << std::endl;
        }

        summaryFile << std::endl;
        summaryFile << homeTeamName << " galibiyet: " << homeWins << std::endl;
        summaryFile << awayTeamName << " galibiyet: " << awayWins << std::endl;
        summaryFile << "Beraberlik: " << draws << std::endl;
        summaryFile << "Toplam gol | " << homeTeamName << ": " << totalHomeGoals
            << " | " << awayTeamName << ": " << totalAwayGoals << std::endl;
        summaryFile << "Mac basi gol | " << homeTeamName << ": " << std::fixed << std::setprecision(2)
            << (simulationCount > 0 ? static_cast<double>(totalHomeGoals) / simulationCount : 0.0)
            << " | " << awayTeamName << ": "
            << (simulationCount > 0 ? static_cast<double>(totalAwayGoals) / simulationCount : 0.0) << std::endl;

        std::cout << "Ozet dosyasi yazildi: " << summaryPath.string() << '\n';

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Runtime error: " << ex.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "Runtime error: unknown exception\n";
        return 1;
    }
}
