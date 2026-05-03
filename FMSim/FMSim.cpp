#pragma comment(lib, "Comdlg32.lib")

#include <iostream>
#include <vector>
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

    struct SimulationOptions {
        std::filesystem::path homeTeamPath;
        std::filesystem::path awayTeamPath;
        int simulationMinutes = 6;
        int simulationCount = 1;
        std::string suffix;
        std::filesystem::path logsDirectory;
        std::filesystem::path summaryPath;
    };

    struct MatchSeriesResult {
        SimulationOptions options;
        std::string homeTeamName;
        std::string awayTeamName;
        std::vector<MatchResultSummary> matchResults;
    };

    std::vector<Player> createPlayers(const std::string& prefix, int baseAttack, int baseDefense, int baseGoalkeeping) {
        using enum Position;
        return
        {
            Player(prefix + " GK", Goalkeeper, Attributes(12, 8, 10, 72, 50, 15, 10, 8, 55, baseGoalkeeping, baseGoalkeeping)),
            Player(prefix + " RB", Defender, Attributes(baseDefense, 45, 66, 78, 58, 68, 42, 30, 64, 58, 62)),
            Player(prefix + " CB1", Defender, Attributes(baseDefense + 4, 35, 54, 76, 52, 74, 30, 28, 74, 72, 68)),
            Player(prefix + " CB2", Defender, Attributes(baseDefense + 2, 36, 52, 75, 50, 72, 28, 28, 76, 74, 66)),
            Player(prefix + " LB", Defender, Attributes(baseDefense - 1, 48, 67, 78, 60, 66, 44, 32, 64, 60, 63)),
            Player(prefix + " DM", Midfielder, Attributes(62, 58, 60, 82, 68, 64, 55, 44, 68, 62, 64)),
            Player(prefix + " CM1", Midfielder, Attributes(58, 66, 64, 84, 74, 58, 64, 56, 66, 60, 65)),
            Player(prefix + " CM2", Midfielder, Attributes(56, 68, 65, 83, 76, 54, 66, 58, 64, 58, 66)),
            Player(prefix + " RW", Attacker, Attributes(34, baseAttack, 78, 79, 68, 32, 74, 66, 60, 58, 63)),
            Player(prefix + " ST", Attacker, Attributes(30, baseAttack + 4, 74, 77, 60, 28, 72, 74, 72, 68, 64)),
            Player(prefix + " LW", Attacker, Attributes(34, baseAttack - 1, 79, 79, 69, 30, 75, 67, 59, 57, 63))
        };
    }

    std::string tacticToString(TacticType tacticType) {
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

    std::string defenseTacticToString(DefenseTacticType tacticType) {
        switch (tacticType) {
            using enum DefenseTacticType;
        case Pressing: return "Pressing";
        case ManMarking: return "Man Marking";
        case ZonalMarking: return "Zonal Marking";
        case CounterPressing: return "Counter Pressing";
        default: return "Unknown";
        }
    }

    std::string zoneToString(Zone zone) {
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
        default: return "Unknown";
        }
    }

    std::string attackEventToString(AttackEvent attackEvent) {
        switch (attackEvent) {
            using enum AttackEvent;
        case ShortPass: return "Short Pass";
        case LongPass: return "Long Pass";
        case ThroughBall: return "Through Ball";
        case Dribble: return "Dribble";
        case Shoot: return "Shoot";
        case Cross: return "Cross";
        case Clearance: return "Clearance";
        default: return "Attack";
        }
    }

    std::string defenseEventToString(DefenseEvent defenseEvent) {
        switch (defenseEvent) {
            using enum DefenseEvent;
        case Tackle: return "Tackle";
        case Interception: return "Interception";
        case Block: return "Block";
        case HeadingClearance: return "Heading Clearance";
        case SlidingTackle: return "Sliding Tackle";
        case Pressure: return "Pressure";
        case ForwardPress: return "Forward Press";
        case GoalkeeperSave: return "Goalkeeper Save";
        case GoalkeeperPunch: return "Goalkeeper Punch";
        case GoalkeeperCatch: return "Goalkeeper Catch";
        case Clearance: return "Clearance";
        case Idle: return "Idle";
        default: return "Defense";
        }
    }

    std::string outcomeToString(EventOutcome eventOutcome) {
        switch (eventOutcome) {
            using enum EventOutcome;
        case Success: return "Success";
        case Fail: return "Fail";
        case Goal: return "Goal";
        case Saved: return "Saved";
        case Blocked: return "Blocked";
        case Cleared: return "Cleared";
        case Out: return "Out";
        case ThrownIn: return "Thrown In";
        case CornerKick: return "Corner";
        case GoalKeeperHeld: return "Held";
        case Rebound: return "Rebound";
        case Fouled: return "Fouled";
        default: return "Outcome";
        }
    }

    std::string otherEventToString(OtherEvent otherEvent) {
        switch (otherEvent) {
            using enum OtherEvent;
        case Kickoff: return "Kickoff";
        case HalfTime: return "Half Time";
        case FullTime: return "Full Time";
        case Rebound: return "Rebound";
        case OwnGoal: return "Own Goal";
        case Offside: return "Offside";
        case Foul: return "Foul";
        case Injury: return "Injury";
        case OutOfPlay: return "Out Of Play";
        case ThrowIn: return "Throw In";
        case CornerKick: return "Corner Kick";
        default: return "Other Event";
        }
    }

    std::string teamNameForEvent(PossessionState possessionState, const MatchEngine& engine) {
        switch (possessionState) {
            using enum PossessionState;
        case Home: return engine.getHomeTeamName();
        case Away: return engine.getAwayTeamName();
        case None:
        default: return "None";
        }
    }

    std::string eventTimestamp(int elapsedSeconds) {
        std::string timeStamp;

        const int minutes = elapsedSeconds / 60;
        const int seconds = elapsedSeconds % 60;

        timeStamp = std::format("{:02}:{:02}", minutes, seconds);

        return timeStamp;
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

        for (std::size_t index = 0; index <= eventIndex && index < engine.getEvents().size(); ++index) {
            const EventStruct& currentEvent = engine.getEvents()[index];
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
            using enum PossessionState;
        case Home: return "H";
        case Away: return "A";
        case None:
        default: return "-";
        }
    }

    std::pair<int, int> zoneToGridPosition(Zone zone) {
        switch (zone) {
            using enum Zone;
        case H1: return { 0, 0 };
        case H2: return { 0, 1 };
        case H3: return { 0, 2 };
        case M1: return { 1, 0 };
        case M2: return { 1, 1 };
        case M3: return { 1, 2 };
        case A1: return { 2, 0 };
        case A2: return { 2, 1 };
        case A3: return { 2, 2 };
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
        if (engine.getEvents().empty()) {
            return;
        }

        std::cout << "\nReplay basliyor. Her aksiyon icin Enter'a basin.\n";
        std::cout << "Baslamak icin Enter'a basin...";
        std::string discard;
        std::getline(std::cin, discard);

        const std::size_t commentaryOffset = 5;
        for (std::size_t index = 0; index < engine.getEvents().size(); ++index) {
            std::string commentaryLine = "Anlatim bulunamadi.";

            if (const std::size_t commentaryIndex = commentaryOffset + index; commentaryIndex < commentaryLines.size()) {
                commentaryLine = commentaryLines[commentaryIndex];
            }

            printZoneReplayFrame(engine.getEvents()[index], engine, index, commentaryLine);
        }
    }

    std::string teamSummaryLine(const MatchEngine& engine, bool isHomeTeam) {
        const TeamStrength strength = isHomeTeam ? engine.getHomeTeamStrength() : engine.getAwayTeamStrength();
        const std::string teamName = isHomeTeam ? engine.getHomeTeamName() : engine.getAwayTeamName();
        const TacticType attackTactic = isHomeTeam ? engine.getHomeTacticType() : engine.getAwayTacticType();
        const DefenseTacticType defenseTactic = isHomeTeam ? engine.getHomeDefenseTacticType() : engine.getAwayDefenseTacticType();

        std::string summary = std::format("{} | attack tactic: {} | defense tactic: {} | power A:{:.1f} D:{:.1f} G:{:.1f}",
            teamName,
            tacticToString(attackTactic),
            defenseTacticToString(defenseTactic),
            strength.getAttack(),
            strength.getDefense(),
            strength.getGoalkeeping());

        return summary;
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
            candidate = directory / std::format("{}({}){}", stem, index, extension);
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
        constexpr int defaultSimulationCount = 1;
        constexpr int warningSimulationCount = 1000;
        constexpr int maxSimulationCount = 10000;

        std::cout << "How many matches should be simulated? ";

        std::string input;
        std::getline(std::cin, input);

        try {
            std::size_t pos = 0;
            int parsedValue = std::stoi(input, &pos);

            if (pos != input.size()) {
                return defaultSimulationCount;
            }

            if (parsedValue <= 0) {
                return defaultSimulationCount;
            }

            if (parsedValue > maxSimulationCount) {
                std::cout << "A maximum of " << maxSimulationCount
                    << " matches can be simulated. Using "
                    << maxSimulationCount << ".\n";

                return maxSimulationCount;
            }

            if (parsedValue > warningSimulationCount) {
                std::cout << "Warning: Simulating more than "
                    << warningSimulationCount
                    << " matches may take a long time.\n";

                std::cout << "Do you want to continue? (y/n): ";

                std::string confirm;
                std::getline(std::cin, confirm);

                if (confirm.empty() || (confirm[0] != 'y' && confirm[0] != 'Y')) {
                    std::cout << "Operation cancelled. Using default value "
                        << defaultSimulationCount << ".\n";

                    return defaultSimulationCount;
                }
            }

            return parsedValue;
        }
        catch (const std::invalid_argument&) {
            return defaultSimulationCount;
        }
        catch (const std::out_of_range&) {
            std::cout << "The entered value is too large. Using maximum value "
                << maxSimulationCount << ".\n";

            return maxSimulationCount;
        }
    }

    std::optional<std::string> readEnvironmentVariable(const char* variableName) {
        char* rawValue = nullptr;

        if (std::size_t length = 0; _dupenv_s(&rawValue, &length, variableName) != 0 || rawValue == nullptr || length == 0) {
            return std::nullopt;
        }

        std::unique_ptr<char, decltype(&free)> value(rawValue, &free);
        return std::string(value.get());
    }

    std::optional<std::filesystem::path> readEnvironmentPathVariable(const wchar_t* variableName) {
        wchar_t* rawValue = nullptr;

        if (std::size_t length = 0; _wdupenv_s(&rawValue, &length, variableName) != 0 || rawValue == nullptr || length == 0) {
            return std::nullopt;
        }

        std::unique_ptr<wchar_t, decltype(&free)> value(rawValue, &free);
        return std::filesystem::path(value.get());
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

        std::string selectedFile(MAX_PATH, '\0');
        const std::filesystem::path teamsDirectory = std::filesystem::current_path() / "Teams";
        const std::string initialDirectory = teamsDirectory.string();

        f.lpstrFilter = "JSON Files\0*.json\0All Files\0*.*\0";
        f.lpstrTitle = title;
        f.lpstrInitialDir = initialDirectory.c_str();
        f.nMaxFile = static_cast<DWORD>(selectedFile.size());
        f.lpstrFile = selectedFile.data();

        if (GetOpenFileNameA(&f) == TRUE) {
            return std::filesystem::path(selectedFile.c_str());
        }

        return std::nullopt;
    }

    bool shouldSkipReplay() {
        const std::optional<std::string> skipReplay = readEnvironmentVariable("FMSIM_SKIP_REPLAY");
        return skipReplay.has_value() && skipReplay.value() == "1";
    }

    bool shouldUseLiveEvents() {
        const std::optional<std::string> liveEvents = readEnvironmentVariable("FMSIM_LIVE_EVENTS");
        return liveEvents.has_value() && liveEvents.value() == "1";
    }

    bool isNonInteractive() {
        const std::optional<std::string> nonInteractive = readEnvironmentVariable("FMSIM_NON_INTERACTIVE");
        return nonInteractive.has_value() && nonInteractive.value() == "1";
    }

    std::string jsonEscape(const std::string& value) {
        std::ostringstream stream;
        for (const char ch : value) {
            switch (ch) {
            case '\\': stream << R"(\\)"; break;
            case '"': stream << R"(\")"; break;
            case '\n': stream << "\\n"; break;
            case '\r': stream << "\\r"; break;
            case '\t': stream << "\\t"; break;
            default: stream << ch; break;
            }
        }
        return stream.str();
    }

    std::string latestCommentaryLine(const MatchEngine& engine, const Commentator& commentator) {
        const std::vector<std::string> commentaryLines = commentator.BuildCommentary(engine);
        return commentaryLines.empty() ? std::string() : commentaryLines.back();
    }

    void writeLiveEventLine(const MatchEngine& engine, const Commentator& commentator) {
        if (engine.getEvents().empty()) {
            return;
        }

        const EventStruct& eventStruct = engine.getEvents().back();
        std::cout << "FMSIM_EVENT {"
            << R"("time":")" << jsonEscape(eventTimestamp(eventStruct.elapsedSeconds)) << "\","
            << R"("homeTeam":")" << jsonEscape(engine.getHomeTeamName()) << "\","
            << R"("awayTeam":")" << jsonEscape(engine.getAwayTeamName()) << "\","
            << R"("team":")" << jsonEscape(teamNameForEvent(eventStruct.possessionState, engine)) << "\","
            << R"("fromZone":")" << jsonEscape(zoneToString(eventStruct.fromZone)) << "\","
            << R"("toZone":")" << jsonEscape(zoneToString(eventStruct.toZone)) << "\","
            << R"("homeScore":)" << engine.getHomeScore() << ","
            << R"("awayScore":)" << engine.getAwayScore() << ","
            << R"("isFullTime":)" << (eventStruct.isOtherEvent && eventStruct.otherEvent == OtherEvent::FullTime ? "true" : "false") << ","
            << R"("commentary":")" << jsonEscape(latestCommentaryLine(engine, commentator)) << "\""
            << "}" << std::endl;
    }

    void waitForLiveNextCommand() {
        std::string command;
        std::getline(std::cin, command);
    }

    void runLiveSimulation(MatchEngine& engine, const Commentator& commentator) {
        engine.simulateStart();
        writeLiveEventLine(engine, commentator);
        waitForLiveNextCommand();

        while (engine.simulateNextEvent()) {
            writeLiveEventLine(engine, commentator);
            waitForLiveNextCommand();
        }

        writeLiveEventLine(engine, commentator);
    }

    std::optional<SimulationOptions> readSimulationOptions() {
        const std::optional<std::filesystem::path> homeTeamPath = selectTeamJsonFile("Choose Home Team JSON");
        if (!homeTeamPath.has_value()) {
            std::cerr << "Home team selection cancelled.\n";
            return std::nullopt;
        }

        const std::optional<std::filesystem::path> awayTeamPath = selectTeamJsonFile("Choose Away Team JSON");
        if (!awayTeamPath.has_value()) {
            std::cerr << "Away team selection cancelled.\n";
            return std::nullopt;
        }

        const int simulationMinutes = promptSimulationMinutes();
        const int simulationCount = promptSimulationCount();
        const std::string suffix = std::to_string(simulationMinutes);
        const std::filesystem::path logsDirectory = "Logs";
        std::filesystem::create_directories(logsDirectory);

        return SimulationOptions{
            homeTeamPath.value(),
            awayTeamPath.value(),
            simulationMinutes,
            simulationCount,
            suffix,
            logsDirectory,
            buildSummaryOutputPath(logsDirectory, suffix)
        };
    }

    void runSimulationMode(MatchEngine& engine, const Commentator& commentator) {
        if (shouldUseLiveEvents()) {
            runLiveSimulation(engine, commentator);
            return;
        }

        engine.simulateFullMatch();
    }

    void writeDebugLog(std::ofstream& debugFile, const MatchEngine& engine) {
        debugFile << "simulation finished" << std::endl;
        debugFile << "event logs count: " << engine.getEventLogs().size() << std::endl;
        debugFile << "score: " << engine.getHomeScore() << "-" << engine.getAwayScore() << std::endl;
        debugFile << teamSummaryLine(engine, true) << std::endl;
        debugFile << teamSummaryLine(engine, false) << std::endl;

        for (const auto& eventStruct : engine.getEvents()) {
            debugFile << "log: " << detailedEventLine(eventStruct, engine) << std::endl;
        }
    }

    void writeEventLog(
        std::ofstream& eventFile,
        const MatchEngine& engine,
        const Team& homeTeam,
        const Team& awayTeam,
        int simulationMinutes) {
        eventFile << simulationMinutes << " dakikalik mac sonucu" << std::endl;
        eventFile << homeTeam.getName() << " " << engine.getHomeScore()
            << " - " << engine.getAwayScore() << " " << awayTeam.getName() << std::endl << std::endl;
        eventFile << teamSummaryLine(engine, true) << std::endl;
        eventFile << teamSummaryLine(engine, false) << std::endl << std::endl;

        for (const auto& eventStruct : engine.getEvents()) {
            eventFile << detailedEventLine(eventStruct, engine) << std::endl;
        }
    }

    void printMatchResult(const MatchEngine& engine, int matchIndex, int simulationCount) {
        std::cout << "[" << (matchIndex + 1) << "/" << simulationCount << "] "
            << engine.getHomeTeamName() << " " << engine.getHomeScore()
            << " - " << engine.getAwayScore() << " " << engine.getAwayTeamName() << '\n';
    }

    void printReplayIfNeeded(
        const MatchEngine& engine,
        const std::vector<std::string>& commentaryLines,
        int simulationCount) {
        if (simulationCount != 1 || shouldSkipReplay()) {
            return;
        }

        std::cout << '\n';
        for (const auto& eventStruct : engine.getEvents()) {
            std::cout << detailedEventLine(eventStruct, engine) << '\n';
        }

        replayMatchInConsole(engine, commentaryLines);
    }

    MatchResultSummary runSingleMatch(
        const SimulationOptions& options,
        int matchIndex,
        std::string& homeTeamName,
        std::string& awayTeamName) {
        const MatchOutputPaths outputPaths = buildMatchOutputPaths(options.logsDirectory, options.suffix);
        std::ofstream debugFile(outputPaths.debugPath, std::ios::trunc);
        std::ofstream eventFile(outputPaths.eventPath, std::ios::trunc);
        debugFile.setf(std::ios::unitbuf);
        eventFile.setf(std::ios::unitbuf);

        debugFile << "main started" << std::endl;

        Team homeTeam = loadTeamFromJsonFile(options.homeTeamPath);
        Team awayTeam = loadTeamFromJsonFile(options.awayTeamPath);
        if (homeTeamName.empty()) {
            homeTeamName = homeTeam.getName();
            awayTeamName = awayTeam.getName();
        }
        debugFile << "teams created" << std::endl;

        MatchEngine engine(homeTeam, awayTeam, true, options.simulationMinutes);
        debugFile << "engine created" << std::endl;
        Commentator commentator(homeTeam.getName(), awayTeam.getName());

        runSimulationMode(engine, commentator);
        writeDebugLog(debugFile, engine);
        writeEventLog(eventFile, engine, homeTeam, awayTeam, options.simulationMinutes);

        const std::vector<std::string> commentaryLines = commentator.BuildCommentary(engine);
        commentator.SaveCommentary(engine, outputPaths.commentaryPath.string());
        debugFile << "commentator created from same match events" << std::endl;
        debugFile << "commentary saved to " << outputPaths.commentaryPath.string() << std::endl;
        debugFile << "stdout write completed" << std::endl;

        printMatchResult(engine, matchIndex, options.simulationCount);
        printReplayIfNeeded(engine, commentaryLines, options.simulationCount);

        return MatchResultSummary{ engine.getHomeScore(), engine.getAwayScore() };
    }

    MatchSeriesResult runMatchSeries(const SimulationOptions& options) {
        MatchSeriesResult result{ options };
        result.matchResults.reserve(options.simulationCount);

        for (int matchIndex = 0; matchIndex < options.simulationCount; ++matchIndex) {
            result.matchResults.push_back(
                runSingleMatch(options, matchIndex, result.homeTeamName, result.awayTeamName));
        }

        return result;
    }

    void updateSeriesStats(
        const MatchResultSummary& result,
        int& homeWins,
        int& awayWins,
        int& draws,
        int& totalHomeGoals,
        int& totalAwayGoals) {
        totalHomeGoals += result.homeScore;
        totalAwayGoals += result.awayScore;

        if (result.homeScore > result.awayScore) {
            homeWins++;
            return;
        }

        if (result.homeScore < result.awayScore) {
            awayWins++;
            return;
        }

        draws++;
    }

    void writeSeriesSummary(const MatchSeriesResult& result) {
        std::ofstream summaryFile(result.options.summaryPath, std::ios::trunc);
        int homeWins = 0;
        int awayWins = 0;
        int draws = 0;
        int totalHomeGoals = 0;
        int totalAwayGoals = 0;

        summaryFile << result.options.simulationCount << " maclik "
            << result.options.simulationMinutes << " dakikalik seri ozeti" << std::endl;
        summaryFile << "Takimlar: " << result.homeTeamName << " vs " << result.awayTeamName << std::endl << std::endl;

        for (std::size_t index = 0; index < result.matchResults.size(); ++index) {
            const MatchResultSummary& matchResult = result.matchResults[index];
            updateSeriesStats(matchResult, homeWins, awayWins, draws, totalHomeGoals, totalAwayGoals);

            summaryFile << "[" << (index + 1) << "] " << result.homeTeamName << " "
                << matchResult.homeScore << " - " << matchResult.awayScore
                << " " << result.awayTeamName << std::endl;
        }

        summaryFile << std::endl;
        summaryFile << result.homeTeamName << " galibiyet: " << homeWins << std::endl;
        summaryFile << result.awayTeamName << " galibiyet: " << awayWins << std::endl;
        summaryFile << "Beraberlik: " << draws << std::endl;
        summaryFile << "Toplam gol | " << result.homeTeamName << ": " << totalHomeGoals
            << " | " << result.awayTeamName << ": " << totalAwayGoals << std::endl;
        summaryFile << "Mac basi gol | " << result.homeTeamName << ": " << std::fixed << std::setprecision(2)
            << (result.options.simulationCount > 0 ? static_cast<double>(totalHomeGoals) / result.options.simulationCount : 0.0)
            << " | " << result.awayTeamName << ": "
            << (result.options.simulationCount > 0 ? static_cast<double>(totalAwayGoals) / result.options.simulationCount : 0.0) << std::endl;
    }
}

int main() {
    try {
        const std::optional<SimulationOptions> options = readSimulationOptions();
        if (!options.has_value()) {
            return 1;
        }

        const MatchSeriesResult result = runMatchSeries(options.value());
        writeSeriesSummary(result);

        std::cout << "Ozet dosyasi yazildi: " << result.options.summaryPath.string() << '\n';

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
