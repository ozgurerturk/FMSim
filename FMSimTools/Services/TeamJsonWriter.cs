using FMSimTools.ViewModels;

namespace FMSimTools.Services
{
    public static class TeamJsonWriter
    {
        public static string SaveTeam(
            string teamsDirectory,
            string teamName,
            string attackTactic,
            string defenseTactic,
            IEnumerable<TeamPlayerSlotViewModel> playerSlots)
        {
            return TeamJsonStore.SaveTeam(
                teamsDirectory,
                teamName,
                attackTactic,
                defenseTactic,
                playerSlots.Select(slot => slot.Player!));
        }
    }
}
