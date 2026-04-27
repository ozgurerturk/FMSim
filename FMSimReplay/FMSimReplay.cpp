#include <windows.h>
#include <commdlg.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace {
    struct ReplayEvent {
        std::string timestamp;
        std::string teamName;
        std::string fromZone;
        std::string toZone;
        std::string summary;
        bool isGoal = false;
    };

    struct ReplayMatch {
        std::string title;
        std::string finalScoreLine;
        std::string homeTeamName;
        std::string awayTeamName;
        std::vector<std::string> introLines;
        std::vector<ReplayEvent> events;
    };

    std::optional<std::filesystem::path> selectTextFile() {
        char fileBuffer[MAX_PATH] = {};
        char initialDirectoryBuffer[MAX_PATH] = {};
        const std::filesystem::path logsDirectory = std::filesystem::current_path() / "Logs";
        const std::string initialDirectory = logsDirectory.string();
        strncpy_s(initialDirectoryBuffer, initialDirectory.c_str(), _TRUNCATE);

        OPENFILENAMEA openFileName = {};
        openFileName.lStructSize = sizeof(openFileName);
        openFileName.hwndOwner = nullptr;
        openFileName.lpstrFile = fileBuffer;
        openFileName.nMaxFile = MAX_PATH;
        openFileName.lpstrInitialDir = initialDirectoryBuffer;
        openFileName.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
        openFileName.nFilterIndex = 1;
        openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        openFileName.lpstrTitle = "Event log dosyasini sec";

        if (GetOpenFileNameA(&openFileName) == TRUE) {
            return std::filesystem::path(openFileName.lpstrFile);
        }

        return std::nullopt;
    }

    std::vector<std::string> readAllLines(const std::filesystem::path& filePath) {
        std::ifstream inputFile(filePath);
        std::vector<std::string> lines;
        std::string line;

        while (std::getline(inputFile, line)) {
            lines.push_back(line);
        }

        return lines;
    }

    std::optional<std::filesystem::path> findMatchingCommentaryPath(const std::filesystem::path& eventLogPath) {
        const std::string fileName = eventLogPath.filename().string();
        const std::size_t eventLogPosition = fileName.find("eventlog");
        if (eventLogPosition == std::string::npos) {
            return std::nullopt;
        }

        std::string commentaryFileName = fileName;
        commentaryFileName.replace(eventLogPosition, std::string("eventlog").size(), "match_commentary");
        const std::filesystem::path commentaryPath = eventLogPath.parent_path() / commentaryFileName;
        if (std::filesystem::exists(commentaryPath)) {
            return commentaryPath;
        }

        return std::nullopt;
    }

    std::string trim(const std::string& text) {
        const std::size_t first = text.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return "";
        }

        const std::size_t last = text.find_last_not_of(" \t\r\n");
        return text.substr(first, last - first + 1);
    }

    std::pair<std::string, std::string> parseTeamsFromScoreLine(const std::string& scoreLine) {
        const std::regex scoreRegex(R"(^(.+)\s+(\d+)\s+-\s+(\d+)\s+(.+)$)");
        std::smatch match;
        if (std::regex_match(scoreLine, match, scoreRegex)) {
            return { trim(match[1].str()), trim(match[4].str()) };
        }

        return { "Home", "Away" };
    }

    ReplayMatch parseReplayMatch(
        const std::vector<std::string>& eventLines,
        const std::vector<std::string>& commentaryLines) {
        ReplayMatch replayMatch;
        if (!eventLines.empty()) {
            replayMatch.title = eventLines[0];
        }
        if (eventLines.size() > 1) {
            replayMatch.finalScoreLine = eventLines[1];
            const auto [homeTeamName, awayTeamName] = parseTeamsFromScoreLine(eventLines[1]);
            replayMatch.homeTeamName = homeTeamName;
            replayMatch.awayTeamName = awayTeamName;
        }

        for (const std::string& line : eventLines) {
            if (!line.empty() && line.front() != '[' && line.find('|') != std::string::npos) {
                replayMatch.introLines.push_back(line);
            }
        }

        std::vector<std::string> commentaryEventLines;
        for (const std::string& line : commentaryLines) {
            if (!line.empty() && line.front() == '[') {
                commentaryEventLines.push_back(line);
            }
        }

        const std::regex otherRegex(R"(^\[(\d{2}:\d{2})\]\s+(.+?)\s+\|\s+Zone:\s+([HMA]\d)\s+->\s+([HMA]\d)\s+\|\s+Other:\s+(.+)$)");
        const std::regex normalRegex(R"(^\[(\d{2}:\d{2})\]\s+(.+?)\s+\|\s+Zone:\s+([HMA]\d)\s+->\s+([HMA]\d)\s+\|\s+Attack:\s+(.+?)\s+\|\s+Defense:\s+(.+?)\s+\|\s+Outcome:\s+(.+)$)");
        std::smatch match;

        for (const std::string& line : eventLines) {
            if (line.empty() || line.front() != '[') {
                continue;
            }

            ReplayEvent replayEvent;
            if (std::regex_match(line, match, otherRegex)) {
                replayEvent.timestamp = match[1].str();
                replayEvent.teamName = match[2].str();
                replayEvent.fromZone = match[3].str();
                replayEvent.toZone = match[4].str();
                replayEvent.summary = line;
            }
            else if (std::regex_match(line, match, normalRegex)) {
                replayEvent.timestamp = match[1].str();
                replayEvent.teamName = match[2].str();
                replayEvent.fromZone = match[3].str();
                replayEvent.toZone = match[4].str();
                replayEvent.summary = line;
                replayEvent.isGoal = trim(match[7].str()) == "Goal";
            }
            else {
                continue;
            }

            replayMatch.events.push_back(replayEvent);
        }

        for (std::size_t index = 0; index < replayMatch.events.size() && index < commentaryEventLines.size(); ++index) {
            replayMatch.events[index].summary = commentaryEventLines[index];
        }

        return replayMatch;
    }

    std::pair<int, int> zoneToGridPosition(const std::string& zone) {
        if (zone == "H1") return { 0, 0 };
        if (zone == "H2") return { 0, 1 };
        if (zone == "H3") return { 0, 2 };
        if (zone == "M1") return { 1, 0 };
        if (zone == "M2") return { 1, 1 };
        if (zone == "M3") return { 1, 2 };
        if (zone == "A1") return { 2, 0 };
        if (zone == "A2") return { 2, 1 };
        if (zone == "A3") return { 2, 2 };
        return { 1, 1 };
    }

    std::string padCell(const std::string& text) {
        if (text.size() >= 8) {
            return text.substr(0, 8);
        }

        return text + std::string(8 - text.size(), ' ');
    }

    std::string possessionShortName(const ReplayMatch& replayMatch, const std::string& teamName) {
        if (teamName == replayMatch.homeTeamName) {
            return "H";
        }
        if (teamName == replayMatch.awayTeamName) {
            return "A";
        }
        return "?";
    }

    std::string liveScoreLineAtEvent(const ReplayMatch& replayMatch, std::size_t eventIndex) {
        int homeScore = 0;
        int awayScore = 0;

        for (std::size_t index = 0; index <= eventIndex && index < replayMatch.events.size(); ++index) {
            const ReplayEvent& replayEvent = replayMatch.events[index];
            if (!replayEvent.isGoal) {
                continue;
            }

            if (replayEvent.teamName == replayMatch.homeTeamName) {
                homeScore++;
            }
            else if (replayEvent.teamName == replayMatch.awayTeamName) {
                awayScore++;
            }
        }

        std::ostringstream stream;
        stream << replayMatch.homeTeamName << " " << homeScore
            << " - " << awayScore << " " << replayMatch.awayTeamName;
        return stream.str();
    }

    void printReplayFrame(const ReplayMatch& replayMatch, std::size_t eventIndex) {
        const ReplayEvent& replayEvent = replayMatch.events[eventIndex];
        std::array<std::array<std::string, 3>, 3> cells =
        { {
            { "H1", "H2", "H3" },
            { "M1", "M2", "M3" },
            { "A1", "A2", "A3" }
        } };

        const auto [row, col] = zoneToGridPosition(replayEvent.toZone);
        cells[row][col] += " O(" + possessionShortName(replayMatch, replayEvent.teamName) + ")";

        std::cout << std::string(30, '\n');
        std::cout << "Replay [" << replayEvent.timestamp << "]\n";
        std::cout << "Skor: " << liveScoreLineAtEvent(replayMatch, eventIndex) << '\n';
        std::cout << "Top: " << replayEvent.toZone << " | Akis: " << replayEvent.fromZone << " -> " << replayEvent.toZone << '\n';
        std::cout << "Topa sahip: " << replayEvent.teamName << "\n\n";

        for (int currentRow = 0; currentRow < 3; ++currentRow) {
            std::cout << "+----------+----------+----------+\n";
            std::cout << "| " << padCell(cells[currentRow][0])
                << " | " << padCell(cells[currentRow][1])
                << " | " << padCell(cells[currentRow][2]) << " |\n";
        }
        std::cout << "+----------+----------+----------+\n\n";
        std::cout << replayEvent.summary << "\n\n";
        std::cout << "Devam etmek icin Enter'a basin...";
        std::string discard;
        std::getline(std::cin, discard);
    }

    void runReplay(const ReplayMatch& replayMatch, const std::filesystem::path& eventLogPath) {
        std::cout << "Replay dosyasi: " << eventLogPath.string() << "\n\n";
        std::cout << replayMatch.title << '\n';
        std::cout << replayMatch.finalScoreLine << "\n\n";

        for (const std::string& introLine : replayMatch.introLines) {
            std::cout << introLine << '\n';
        }

        std::cout << "\nReplay basliyor. Her aksiyon icin Enter'a basin.\n";
        std::cout << "Baslamak icin Enter'a basin...";
        std::string discard;
        std::getline(std::cin, discard);

        for (std::size_t index = 0; index < replayMatch.events.size(); ++index) {
            printReplayFrame(replayMatch, index);
        }
    }
}

int main() {
    const std::optional<std::filesystem::path> selectedFile = selectTextFile();
    if (!selectedFile.has_value()) {
        std::cout << "Dosya secimi iptal edildi.\n";
        return 0;
    }

    const std::filesystem::path eventLogPath = selectedFile.value();
    const std::vector<std::string> eventLines = readAllLines(eventLogPath);
    if (eventLines.empty()) {
        std::cerr << "Secilen dosya bos veya okunamadi.\n";
        return 1;
    }

    std::vector<std::string> commentaryLines;
    const std::optional<std::filesystem::path> commentaryPath = findMatchingCommentaryPath(eventLogPath);
    if (commentaryPath.has_value()) {
        commentaryLines = readAllLines(commentaryPath.value());
    }

    const ReplayMatch replayMatch = parseReplayMatch(eventLines, commentaryLines);
    if (replayMatch.events.empty()) {
        std::cerr << "Replay icin okunabilir event bulunamadi.\n";
        return 1;
    }

    runReplay(replayMatch, eventLogPath);
    return 0;
}
