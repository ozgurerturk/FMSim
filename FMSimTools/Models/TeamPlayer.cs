namespace FMSimTools.Models
{
    public sealed class TeamPlayer
    {
        public string Name { get; init; } = string.Empty;
        public PlayerPosition Position { get; init; }
        public PlayerAttributes Attributes { get; init; } = new();
    }
}
