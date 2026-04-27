namespace FMSimTools.Models
{
    public sealed class SimulationResult
    {
        public string CommentaryPath { get; init; } = string.Empty;
        public string CommentaryText { get; init; } = string.Empty;
        public string OutputText { get; init; } = string.Empty;
        public string ErrorText { get; init; } = string.Empty;
    }
}
