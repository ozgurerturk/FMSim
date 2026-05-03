using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FMSimTools.Models;
using FMSimTools.Services;

namespace FMSimTools.ViewModels
{
    public sealed partial class MatchViewerZone : ObservableObject
    {
        public MatchViewerZone(PitchZone zone)
        {
            Zone = zone;
        }

        public PitchZone Zone { get; }

        public string Name => Zone.Name;

        [ObservableProperty]
        private bool _isCurrent;
    }

    public partial class MatchViewerViewModel : ObservableObject
    {
        private readonly LiveSimulationRequest? _request;
        private CancellationTokenSource? _cancellationTokenSource;

        [ObservableProperty]
        private bool _isPause = false;

        [ObservableProperty]
        private string _homeTeamName = "Home";

        [ObservableProperty]
        private string _awayTeamName = "Away";

        [ObservableProperty]
        private string _scoreLine = "0 - 0";

        [ObservableProperty]
        private string _currentTime = "00:00";

        [ObservableProperty]
        private string _currentTeamName = string.Empty;

        [ObservableProperty]
        private string _currentCommentary = "Waiting for kick off...";

        [ObservableProperty]
        private string _statusMessage = string.Empty;

        [ObservableProperty]
        private string _pauseContinueButtonText = "Pause";

        public MatchViewerViewModel()
        {
            PitchZones = new ObservableCollection<MatchViewerZone>(
                PitchZone.All.Select(zone => new MatchViewerZone(zone)));
        }

        public MatchViewerViewModel(LiveSimulationRequest request)
            : this()
        {
            _request = request;
            HomeTeamName = request.HomeTeam.Name;
            AwayTeamName = request.AwayTeam.Name;
        }

        public ObservableCollection<MatchViewerZone> PitchZones { get; }

        public async Task StartAsync()
        {
            if (_request is null)
            {
                StatusMessage = "No live simulation request.";
                return;
            }

            _cancellationTokenSource = new CancellationTokenSource();

            try
            {
                StatusMessage = "Match running...";
                _ = await SimulationRunner.RunLiveAsync(_request,
                    async liveEvent =>
                    {
                        ApplyLiveEvent(liveEvent);
                        while (IsPause)
                        {
                            await Task.Delay(100, _cancellationTokenSource.Token);
                        }
                    }, _cancellationTokenSource.Token);

                StatusMessage = "Match finished.";
            }
            catch (OperationCanceledException)
            {
                StatusMessage = "Match stopped.";
            }
            catch (Exception exception)
            {
                StatusMessage = $"Live match failed: {exception.Message}";
            }
            _cancellationTokenSource.Dispose();
            _cancellationTokenSource = null;
        }

        public void Stop()
        {
            _cancellationTokenSource?.Cancel();
        }

        private void ApplyLiveEvent(LiveMatchEvent liveEvent)
        {
            HomeTeamName = liveEvent.HomeTeam;
            AwayTeamName = liveEvent.AwayTeam;
            ScoreLine = $"{liveEvent.HomeScore} - {liveEvent.AwayScore}";
            CurrentTime = liveEvent.Time.Trim('[', ']');
            CurrentTeamName = liveEvent.Team;
            CurrentCommentary = liveEvent.Commentary;

            foreach (var zone in PitchZones)
            {
                zone.IsCurrent = zone.Name == liveEvent.ToZone;
            }
        }

        [RelayCommand]
        private void OnPause()
        {
            IsPause = !IsPause;
            StatusMessage = IsPause ? "Match paused." : "Match running...";
            PauseContinueButtonText = IsPause ? "Continue" : "Pause";
        }
    }
}
