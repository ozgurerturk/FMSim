namespace FMSimTools.Models
{
    public sealed class LiveMatchEvent
    {
        public string Time { get; init; } = string.Empty;
        public string HomeTeam { get; init; } = string.Empty;
        public string AwayTeam { get; init; } = string.Empty;
        public string Team { get; init; } = string.Empty;
        public string FromZone { get; init; } = string.Empty;
        public string ToZone { get; init; } = string.Empty;
        public int HomeScore { get; init; }
        public int AwayScore { get; init; }
        public bool IsFullTime { get; init; }
        public string Commentary { get; init; } = string.Empty;
    }
}
