using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FMSimTools.Models;
using FMSimTools.Services;

namespace FMSimTools.ViewModels
{
    public partial class MainWindowViewModel : ObservableObject
    {
        private readonly string _teamsDirectory = TeamJsonStore.GetTeamsDirectory();

        [ObservableProperty]
        private SavedTeam? _selectedTeam;

        [ObservableProperty]
        private string _teamListStatusMessage = string.Empty;

        public MainWindowViewModel()
        {
            RefreshTeams();
        }

        public ObservableCollection<SavedTeam> Teams { get; } = [];

        public event EventHandler? CreateTeamRequested;

        public event EventHandler<SavedTeam>? EditTeamRequested;

        public event EventHandler? KickOffRequested;

        public event EventHandler? MatchHistoryRequested;

        [RelayCommand]
        private void OnKickOff()
        {
            KickOffRequested?.Invoke(this, EventArgs.Empty);
        }

        [RelayCommand]
        private void OnMatchHistory()
        {
            MatchHistoryRequested?.Invoke(this, EventArgs.Empty);
        }

        [RelayCommand]
        private void OnCreateTeam()
        {
            CreateTeamRequested?.Invoke(this, EventArgs.Empty);
        }

        [RelayCommand(CanExecute = nameof(CanOpenSelectedTeam))]
        private void OnOpenSelectedTeam()
        {
            if (SelectedTeam is not null)
            {
                EditTeamRequested?.Invoke(this, SelectedTeam);
            }
        }

        [RelayCommand]
        private void OnRefreshTeams()
        {
            RefreshTeams();
        }

        partial void OnSelectedTeamChanged(SavedTeam? value)
        {
            OpenSelectedTeamCommand.NotifyCanExecuteChanged();
        }

        private bool CanOpenSelectedTeam()
        {
            return SelectedTeam is not null;
        }

        private void RefreshTeams()
        {
            try
            {
                Teams.Clear();

                foreach (var team in TeamJsonStore.LoadTeams(_teamsDirectory))
                {
                    Teams.Add(team);
                }

                TeamListStatusMessage = $"{Teams.Count} team(s)";
            }
            catch (Exception exception)
            {
                TeamListStatusMessage = $"Could not load teams: {exception.Message}";
            }

            OpenSelectedTeamCommand.NotifyCanExecuteChanged();
        }
    }
}
