namespace FMSimTools.Models
{
    public sealed record PitchZone(string Name, int Row, int Column)
    {
        public static IReadOnlyList<PitchZone> All { get; } =
            [
                new("H1", 0, 0),
                new("H2", 0, 1),
                new("H3", 0, 2),
                new("M1", 1, 0),
                new("M2", 1, 1),
                new("M3", 1, 2),
                new("A1", 2, 0),
                new("A2", 2, 1),
                new("A3", 2, 2),
            ];
    }
}
