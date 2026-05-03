using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FMSimTools.Models;
using FMSimTools.Services;

namespace FMSimTools.ViewModels
{
    public partial class SimulationSetupViewModel : ObservableObject
    {
        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(StartSimulationCommand))]
        private SavedTeam? _selectedHomeTeam;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(StartSimulationCommand))]
        private SavedTeam? _selectedAwayTeam;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(StartSimulationCommand))]
        private int _selectedSimulationMinutes = 6;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(StartSimulationCommand))]
        private int _simulationCount = 1;

        [ObservableProperty]
        private string _statusMessage = string.Empty;

        [ObservableProperty]
        private bool _isRunning;

        public SimulationSetupViewModel()
        {
            Teams = new ObservableCollection<SavedTeam>(
                TeamJsonStore.LoadTeams(TeamJsonStore.GetTeamsDirectory()));
        }

        public ObservableCollection<SavedTeam> Teams { get; }

        public ObservableCollection<int> AvailableSimulationMinutes { get; } = [6, 12, 24];

        public event EventHandler<LiveSimulationRequest>? LiveSimulationRequested;

        public event EventHandler? CloseRequested;

        [RelayCommand(CanExecute = nameof(CanStartSimulation))]
        private void OnStartSimulation()
        {
            if (SelectedHomeTeam is null || SelectedAwayTeam is null)
            {
                return;
            }

            LiveSimulationRequested?.Invoke(
                this,
                new LiveSimulationRequest(
                    SelectedHomeTeam,
                    SelectedAwayTeam,
                    SelectedSimulationMinutes,
                    SimulationCount));
        }

        [RelayCommand]
        private void OnCancel()
        {
            CloseRequested?.Invoke(this, EventArgs.Empty);
        }

        private bool CanStartSimulation()
        {
            return !IsRunning &&
                   SelectedHomeTeam is not null &&
                   SelectedAwayTeam is not null &&
                   SelectedHomeTeam.FilePath != SelectedAwayTeam.FilePath &&
                   AvailableSimulationMinutes.Contains(SelectedSimulationMinutes) &&
                   SimulationCount > 0;
        }
    }
}
