namespace FMSimTools.Models
{
    public sealed class MatchHistoryItem
    {
        public string DisplayName { get; init; } = string.Empty;
        public string CommentaryPath { get; init; } = string.Empty;
        public DateTime PlayedAt { get; init; }
        public long Length { get; init; }
        public string SourceFolder { get; init; } = string.Empty;
    }
}
