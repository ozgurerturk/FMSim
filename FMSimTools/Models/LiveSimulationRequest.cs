namespace FMSimTools.Models
{
    public sealed record LiveSimulationRequest(
        SavedTeam HomeTeam,
        SavedTeam AwayTeam,
        int SimulationMinutes,
        int SimulationCount);
}
