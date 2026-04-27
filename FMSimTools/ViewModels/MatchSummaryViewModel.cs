using CommunityToolkit.Mvvm.ComponentModel;
using FMSimTools.Models;

namespace FMSimTools.ViewModels
{
    public sealed partial class MatchSummaryViewModel : ObservableObject
    {
        public MatchSummaryViewModel(SimulationResult simulationResult)
        {
            CommentaryPath = simulationResult.CommentaryPath;
            CommentaryText = simulationResult.CommentaryText;
            OutputText = simulationResult.OutputText;
        }

        public string CommentaryPath { get; }

        public string CommentaryText { get; }

        public string OutputText { get; }
    }
}
