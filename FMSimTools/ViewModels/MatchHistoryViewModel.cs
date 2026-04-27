using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FMSimTools.Models;
using FMSimTools.Services;

namespace FMSimTools.ViewModels
{
    public partial class MatchHistoryViewModel : ObservableObject
    {
        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(OpenSelectedMatchCommand))]
        private MatchHistoryItem? _selectedMatch;

        [ObservableProperty]
        private string _statusMessage = string.Empty;

        public MatchHistoryViewModel()
        {
            RefreshHistory();
        }

        public ObservableCollection<MatchHistoryItem> Matches { get; } = [];

        public event EventHandler<SimulationResult>? MatchOpened;

        [RelayCommand]
        private void OnRefreshHistory()
        {
            RefreshHistory();
        }

        [RelayCommand(CanExecute = nameof(CanOpenSelectedMatch))]
        private void OnOpenSelectedMatch()
        {
            if (SelectedMatch is null)
            {
                return;
            }

            MatchOpened?.Invoke(this, new SimulationResult
            {
                CommentaryPath = SelectedMatch.CommentaryPath,
                CommentaryText = MatchHistoryStore.ReadCommentary(SelectedMatch)
            });
        }

        private bool CanOpenSelectedMatch()
        {
            return SelectedMatch is not null;
        }

        private void RefreshHistory()
        {
            Matches.Clear();

            foreach (var item in MatchHistoryStore.LoadHistory())
            {
                Matches.Add(item);
            }

            StatusMessage = $"{Matches.Count} match(es)";
            OpenSelectedMatchCommand.NotifyCanExecuteChanged();
        }
    }
}
