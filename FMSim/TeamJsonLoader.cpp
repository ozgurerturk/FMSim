#include "TeamJsonLoader.h"

#include <cctype>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace {
    struct JsonValue;

    using JsonObject = std::map<std::string, JsonValue>;
    using JsonArray = std::vector<JsonValue>;

    struct JsonValue {
        using Storage = std::variant<std::nullptr_t, bool, double, std::string, JsonArray, JsonObject>;
        Storage storage;

        bool isString() const { return std::holds_alternative<std::string>(storage); }
        bool isNumber() const { return std::holds_alternative<double>(storage); }
        bool isArray() const { return std::holds_alternative<JsonArray>(storage); }
        bool isObject() const { return std::holds_alternative<JsonObject>(storage); }

        const std::string& asString() const { return std::get<std::string>(storage); }
        double asNumber() const { return std::get<double>(storage); }
        const JsonArray& asArray() const { return std::get<JsonArray>(storage); }
        const JsonObject& asObject() const { return std::get<JsonObject>(storage); }
    };

    class JsonParser {
    public:
        explicit JsonParser(std::string text)
            : _text(std::move(text)) {
        }

        JsonValue parse() {
            consumeUtf8Bom();
            JsonValue value = parseValue();
            skipWhitespace();
            if (!isAtEnd()) {
                throw std::runtime_error("JSON parse error: unexpected trailing characters.");
            }
            return value;
        }

    private:
        JsonValue parseValue() {
            skipWhitespace();
            if (isAtEnd()) {
                throw std::runtime_error("JSON parse error: unexpected end of input.");
            }

            const char current = peek();
            if (current == '{') {
                return JsonValue{ parseObject() };
            }
            if (current == '[') {
                return JsonValue{ parseArray() };
            }
            if (current == '"') {
                return JsonValue{ parseString() };
            }
            if (current == '-' || std::isdigit(static_cast<unsigned char>(current))) {
                return JsonValue{ parseNumber() };
            }
            if (matchLiteral("true")) {
                return JsonValue{ true };
            }
            if (matchLiteral("false")) {
                return JsonValue{ false };
            }
            if (matchLiteral("null")) {
                return JsonValue{ nullptr };
            }

            throw std::runtime_error("JSON parse error: unsupported value.");
        }

        JsonObject parseObject() {
            expect('{');
            JsonObject object;
            skipWhitespace();
            if (consumeIf('}')) {
                return object;
            }

            while (true) {
                skipWhitespace();
                const std::string key = parseString();
                skipWhitespace();
                expect(':');
                object.emplace(key, parseValue());
                skipWhitespace();

                if (consumeIf('}')) {
                    break;
                }

                expect(',');
            }

            return object;
        }

        JsonArray parseArray() {
            expect('[');
            JsonArray array;
            skipWhitespace();
            if (consumeIf(']')) {
                return array;
            }

            while (true) {
                array.push_back(parseValue());
                skipWhitespace();
                if (consumeIf(']')) {
                    break;
                }

                expect(',');
            }

            return array;
        }

        std::string parseString() {
            expect('"');
            std::string result;

            while (!isAtEnd()) {
                const char current = advance();
                if (current == '"') {
                    return result;
                }

                if (current == '\\') {
                    if (isAtEnd()) {
                        throw std::runtime_error("JSON parse error: incomplete escape sequence.");
                    }

                    const char escaped = advance();
                    switch (escaped) {
                    case '"':
                    case '\\':
                    case '/':
                        result.push_back(escaped);
                        break;
                    case 'b':
                        result.push_back('\b');
                        break;
                    case 'f':
                        result.push_back('\f');
                        break;
                    case 'n':
                        result.push_back('\n');
                        break;
                    case 'r':
                        result.push_back('\r');
                        break;
                    case 't':
                        result.push_back('\t');
                        break;
                    case 'u':
                        appendUtf8(result, parseUnicodeCodePoint());
                        break;
                    default:
                        throw std::runtime_error("JSON parse error: unsupported escape sequence.");
                    }
                }
                else {
                    result.push_back(current);
                }
            }

            throw std::runtime_error("JSON parse error: unterminated string.");
        }

        double parseNumber() {
            const std::size_t start = _position;
            if (peek() == '-') {
                advance();
            }

            while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
                advance();
            }

            if (!isAtEnd() && peek() == '.') {
                advance();
                while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
                    advance();
                }
            }

            const std::string numberText = _text.substr(start, _position - start);
            return std::stod(numberText);
        }

        bool matchLiteral(const char* literal) {
            const std::size_t length = std::char_traits<char>::length(literal);
            if (_text.compare(_position, length, literal) != 0) {
                return false;
            }

            _position += length;
            return true;
        }

        void skipWhitespace() {
            while (!isAtEnd() && std::isspace(static_cast<unsigned char>(peek()))) {
                advance();
            }
        }

        void consumeUtf8Bom() {
            if (_text.size() >= 3 &&
                static_cast<unsigned char>(_text[0]) == 0xEF &&
                static_cast<unsigned char>(_text[1]) == 0xBB &&
                static_cast<unsigned char>(_text[2]) == 0xBF) {
                _position = 3;
            }
        }

        void expect(char expected) {
            skipWhitespace();
            if (isAtEnd() || peek() != expected) {
                throw std::runtime_error(std::string("JSON parse error: expected '") + expected + "'.");
            }
            advance();
        }

        bool consumeIf(char expected) {
            skipWhitespace();
            if (!isAtEnd() && peek() == expected) {
                advance();
                return true;
            }
            return false;
        }

        bool isAtEnd() const {
            return _position >= _text.size();
        }

        char peek() const {
            return _text[_position];
        }

        char advance() {
            return _text[_position++];
        }

        int parseHexDigit() {
            if (isAtEnd()) {
                throw std::runtime_error("JSON parse error: incomplete unicode escape sequence.");
            }

            const char current = advance();
            if (current >= '0' && current <= '9') {
                return current - '0';
            }
            if (current >= 'a' && current <= 'f') {
                return 10 + current - 'a';
            }
            if (current >= 'A' && current <= 'F') {
                return 10 + current - 'A';
            }

            throw std::runtime_error("JSON parse error: invalid unicode escape sequence.");
        }

        unsigned int parseFourHexDigits() {
            unsigned int codeUnit = 0;
            for (int index = 0; index < 4; index++) {
                codeUnit = (codeUnit << 4) | static_cast<unsigned int>(parseHexDigit());
            }
            return codeUnit;
        }

        unsigned int parseUnicodeCodePoint() {
            const unsigned int first = parseFourHexDigits();
            if (first < 0xD800 || first > 0xDBFF) {
                return first;
            }

            if (isAtEnd() || advance() != '\\' || isAtEnd() || advance() != 'u') {
                throw std::runtime_error("JSON parse error: incomplete unicode surrogate pair.");
            }

            const unsigned int second = parseFourHexDigits();
            if (second < 0xDC00 || second > 0xDFFF) {
                throw std::runtime_error("JSON parse error: invalid unicode surrogate pair.");
            }

            return 0x10000 + ((first - 0xD800) << 10) + (second - 0xDC00);
        }

        static void appendUtf8(std::string& text, unsigned int codePoint) {
            if (codePoint <= 0x7F) {
                text.push_back(static_cast<char>(codePoint));
                return;
            }
            if (codePoint <= 0x7FF) {
                text.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
                text.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
                return;
            }
            if (codePoint <= 0xFFFF) {
                text.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
                text.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                text.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
                return;
            }
            if (codePoint <= 0x10FFFF) {
                text.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
                text.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
                text.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
                text.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
                return;
            }

            throw std::runtime_error("JSON parse error: unicode code point out of range.");
        }

        std::string _text;
        std::size_t _position = 0;
    };

    const JsonValue& getRequiredField(const JsonObject& object, const std::string& key) {
        const auto iterator = object.find(key);
        if (iterator == object.end()) {
            throw std::runtime_error("Missing required JSON field: " + key);
        }
        return iterator->second;
    }

    std::string getRequiredString(const JsonObject& object, const std::string& key) {
        const JsonValue& value = getRequiredField(object, key);
        if (!value.isString()) {
            throw std::runtime_error("JSON field is not a string: " + key);
        }
        return value.asString();
    }

    int getRequiredInt(const JsonObject& object, const std::string& key) {
        const JsonValue& value = getRequiredField(object, key);
        if (!value.isNumber()) {
            throw std::runtime_error("JSON field is not a number: " + key);
        }
        return static_cast<int>(value.asNumber());
    }

    Position parsePosition(const std::string& text) {
        if (text == "Goalkeeper") return Position::Goalkeeper;
        if (text == "Defender") return Position::Defender;
        if (text == "Midfielder") return Position::Midfielder;
        if (text == "Attacker") return Position::Attacker;
        throw std::runtime_error("Unknown position: " + text);
    }

    TacticType parseAttackTactic(const std::string& text) {
        if (text == "Possession") return TacticType::Possession;
        if (text == "WingPlay") return TacticType::WingPlay;
        if (text == "LongBall") return TacticType::LongBall;
        if (text == "TikiTaka") return TacticType::TikiTaka;
        if (text == "CounterAttack") return TacticType::CounterAttack;
        throw std::runtime_error("Unknown attack tactic: " + text);
    }

    DefenseTacticType parseDefenseTactic(const std::string& text) {
        if (text == "Pressing") return DefenseTacticType::Pressing;
        if (text == "ManMarking") return DefenseTacticType::ManMarking;
        if (text == "ZonalMarking") return DefenseTacticType::ZonalMarking;
        if (text == "CounterPressing") return DefenseTacticType::CounterPressing;
        throw std::runtime_error("Unknown defense tactic: " + text);
    }

    Attributes parseAttributes(const JsonObject& object) {
        return Attributes(
            getRequiredInt(object, "defense"),
            getRequiredInt(object, "attack"),
            getRequiredInt(object, "pace"),
            getRequiredInt(object, "stamina"),
            getRequiredInt(object, "passing"),
            getRequiredInt(object, "tackle"),
            getRequiredInt(object, "dribbling"),
            getRequiredInt(object, "shooting"),
            getRequiredInt(object, "physicality"),
            getRequiredInt(object, "jumping"),
            getRequiredInt(object, "reactionTime"));
    }

    Player parsePlayer(const JsonObject& object) {
        const JsonValue& attributesValue = getRequiredField(object, "attributes");
        if (!attributesValue.isObject()) {
            throw std::runtime_error("Player attributes must be an object.");
        }

        return Player(
            getRequiredString(object, "name"),
            parsePosition(getRequiredString(object, "position")),
            parseAttributes(attributesValue.asObject()));
    }
}

Team loadTeamFromJsonFile(const std::filesystem::path& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile) {
        throw std::runtime_error("Team JSON file could not be opened: " + filePath.string());
    }

    std::ostringstream buffer;
    buffer << inputFile.rdbuf();
    JsonParser parser(buffer.str());
    const JsonValue rootValue = parser.parse();

    if (!rootValue.isObject()) {
        throw std::runtime_error("Team JSON root must be an object.");
    }

    const JsonObject& root = rootValue.asObject();
    const JsonValue& playersValue = getRequiredField(root, "players");
    if (!playersValue.isArray()) {
        throw std::runtime_error("Team JSON 'players' must be an array.");
    }

    std::vector<Player> players;
    for (const JsonValue& playerValue : playersValue.asArray()) {
        if (!playerValue.isObject()) {
            throw std::runtime_error("Each player entry must be an object.");
        }
        players.push_back(parsePlayer(playerValue.asObject()));
    }

    return Team(
        getRequiredString(root, "name"),
        players,
        parseAttackTactic(getRequiredString(root, "attackTactic")),
        parseDefenseTactic(getRequiredString(root, "defenseTactic")));
}
