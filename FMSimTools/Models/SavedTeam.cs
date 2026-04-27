namespace FMSimTools.Models
{
    public sealed class SavedTeam
    {
        public string Name { get; init; } = string.Empty;
        public string FilePath { get; init; } = string.Empty;
        public string AttackTactic { get; init; } = string.Empty;
        public string DefenseTactic { get; init; } = string.Empty;
        public IReadOnlyList<TeamPlayer> Players { get; init; } = [];
    }
}
