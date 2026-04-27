using System.IO;
using System.Text.Encodings.Web;
using System.Text.Json;
using FMSimTools.Models;

namespace FMSimTools.Services
{
    public static class TeamJsonStore
    {
        private static readonly JsonSerializerOptions SerializerOptions = new()
        {
            Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping,
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            PropertyNameCaseInsensitive = true,
            WriteIndented = true
        };

        public static string GetTeamsDirectory()
        {
            return AppPaths.TeamsDirectory;
        }

        public static IReadOnlyList<SavedTeam> LoadTeams(string teamsDirectory)
        {
            if (!Directory.Exists(teamsDirectory))
            {
                return [];
            }

            return Directory
                .EnumerateFiles(teamsDirectory, "*.json", SearchOption.TopDirectoryOnly)
                .Select(TryLoadTeam)
                .Where(team => team is not null)
                .Select(team => team!)
                .OrderBy(team => team.Name)
                .ToArray();
        }

        private static SavedTeam? TryLoadTeam(string filePath)
        {
            try
            {
                return LoadTeam(filePath);
            }
            catch (Exception exception) when (
                exception is IOException ||
                exception is JsonException ||
                exception is InvalidOperationException ||
                exception is ArgumentException)
            {
                return null;
            }
        }

        public static SavedTeam LoadTeam(string filePath)
        {
            var json = File.ReadAllText(filePath);
            var teamJson = JsonSerializer.Deserialize<TeamJson>(json, SerializerOptions)
                           ?? throw new InvalidOperationException($"Team JSON could not be read: {filePath}");

            ValidateTeamJson(teamJson, filePath);

            return new SavedTeam
            {
                Name = teamJson.Name!,
                FilePath = filePath,
                AttackTactic = teamJson.AttackTactic!,
                DefenseTactic = teamJson.DefenseTactic!,
                Players = teamJson.Players!.Select(CreateTeamPlayer).ToArray()
            };
        }

        public static string SaveTeam(
            string teamsDirectory,
            string teamName,
            string attackTactic,
            string defenseTactic,
            IEnumerable<TeamPlayer> players)
        {
            Directory.CreateDirectory(teamsDirectory);

            var teamJson = new TeamJson
            {
                Name = teamName,
                AttackTactic = attackTactic,
                DefenseTactic = defenseTactic,
                Players = players.Select(CreatePlayerJson).ToArray()
            };

            var filePath = Path.Combine(teamsDirectory, $"{CreateSafeFileName(teamName)}.json");
            var json = JsonSerializer.Serialize(teamJson, SerializerOptions);
            json = json.ReplaceLineEndings("\r\n");
            File.WriteAllText(filePath, json);

            return filePath;
        }

        private static TeamPlayer CreateTeamPlayer(PlayerJson playerJson)
        {
            if (string.IsNullOrWhiteSpace(playerJson.Name) ||
                string.IsNullOrWhiteSpace(playerJson.Position) ||
                playerJson.Attributes is null)
            {
                throw new InvalidOperationException("Team JSON contains an invalid player entry.");
            }

            return new TeamPlayer
            {
                Name = playerJson.Name,
                Position = Enum.Parse<PlayerPosition>(playerJson.Position),
                Attributes = new PlayerAttributes
                {
                    Defense = playerJson.Attributes!.Defense,
                    Attack = playerJson.Attributes.Attack,
                    Pace = playerJson.Attributes.Pace,
                    Stamina = playerJson.Attributes.Stamina,
                    Passing = playerJson.Attributes.Passing,
                    Tackle = playerJson.Attributes.Tackle,
                    Dribbling = playerJson.Attributes.Dribbling,
                    Shooting = playerJson.Attributes.Shooting,
                    Physicality = playerJson.Attributes.Physicality,
                    Jumping = playerJson.Attributes.Jumping,
                    ReactionTime = playerJson.Attributes.ReactionTime
                }
            };
        }

        private static void ValidateTeamJson(TeamJson teamJson, string filePath)
        {
            if (string.IsNullOrWhiteSpace(teamJson.Name) ||
                string.IsNullOrWhiteSpace(teamJson.AttackTactic) ||
                string.IsNullOrWhiteSpace(teamJson.DefenseTactic) ||
                teamJson.Players is null)
            {
                throw new InvalidOperationException($"Not a valid team JSON file: {filePath}");
            }
        }

        private static PlayerJson CreatePlayerJson(TeamPlayer player)
        {
            return new PlayerJson
            {
                Name = player.Name,
                Position = player.Position.ToString(),
                Attributes = new AttributesJson
                {
                    Defense = player.Attributes.Defense,
                    Attack = player.Attributes.Attack,
                    Pace = player.Attributes.Pace,
                    Stamina = player.Attributes.Stamina,
                    Passing = player.Attributes.Passing,
                    Tackle = player.Attributes.Tackle,
                    Dribbling = player.Attributes.Dribbling,
                    Shooting = player.Attributes.Shooting,
                    Physicality = player.Attributes.Physicality,
                    Jumping = player.Attributes.Jumping,
                    ReactionTime = player.Attributes.ReactionTime
                }
            };
        }

        private static string CreateSafeFileName(string teamName)
        {
            var invalidChars = Path.GetInvalidFileNameChars();
            var safeName = new string(teamName
                .Trim()
                .Select(character => invalidChars.Contains(character) ? '_' : character)
                .ToArray());

            safeName = string.Join("_", safeName.Split(' ', StringSplitOptions.RemoveEmptyEntries));

            return string.IsNullOrWhiteSpace(safeName) ? "team" : safeName.ToLowerInvariant();
        }

        private sealed class TeamJson
        {
            public string? Name { get; init; }
            public string? AttackTactic { get; init; }
            public string? DefenseTactic { get; init; }
            public PlayerJson[]? Players { get; init; }
        }

        private sealed class PlayerJson
        {
            public string? Name { get; init; }
            public string? Position { get; init; }
            public AttributesJson? Attributes { get; init; }
        }

        private sealed class AttributesJson
        {
            public int Defense { get; init; }
            public int Attack { get; init; }
            public int Pace { get; init; }
            public int Stamina { get; init; }
            public int Passing { get; init; }
            public int Tackle { get; init; }
            public int Dribbling { get; init; }
            public int Shooting { get; init; }
            public int Physicality { get; init; }
            public int Jumping { get; init; }
            public int ReactionTime { get; init; }
        }
    }
}
