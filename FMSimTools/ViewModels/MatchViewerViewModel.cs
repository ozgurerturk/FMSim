using System.Collections.ObjectModel;
using System.Threading.Tasks;
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
        private Task? _simulationTask;

        [ObservableProperty]
        private bool _isPause = false;

        [ObservableProperty]
        private bool _isStop = false;

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

        [ObservableProperty]
        private string _stopStartButtonText = "Stop Simulation";

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

        public Task StartAsync()
        {
            if (_request is null)
            {
                StatusMessage = "No live simulation request.";
                return Task.CompletedTask;
            }

            if (_simulationTask is { IsCompleted: false })
            {
                return Task.CompletedTask;
            }

            _simulationTask = RunSimulationAsync();
            return Task.CompletedTask;
        }

        private async Task RunSimulationAsync()
        {
            var request = _request;
            if (request is null)
            {
                StatusMessage = "No live simulation request.";
                return;
            }

            using var cancellationTokenSource = new CancellationTokenSource();
            _cancellationTokenSource = cancellationTokenSource;

            try
            {
                StatusMessage = "Match running...";
                _ = await SimulationRunner.RunLiveAsync(request,
                    async liveEvent =>
                    {
                        ApplyLiveEvent(liveEvent);
                        while (IsPause)
                        {
                            await Task.Delay(100, cancellationTokenSource.Token);
                        }
                    }, cancellationTokenSource.Token);

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
            finally
            {
                if (ReferenceEquals(_cancellationTokenSource, cancellationTokenSource))
                {
                    _cancellationTokenSource = null;
                }
            }
        }

        public void Stop()
        {
            _cancellationTokenSource?.Cancel();
        }

        public async Task StopAsync()
        {
            Stop();

            if (_simulationTask is null)
            {
                return;
            }

            await _simulationTask;
            _simulationTask = null;
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

        private void ResetMatchInfo()
        {
            ScoreLine = "0 - 0";
            CurrentTime = "00:00";
            CurrentTeamName = string.Empty;
            CurrentCommentary = "Waiting for kick off...";
            StopStartButtonText = "Start Simulation";
        }

        [RelayCommand]
        private void OnPause()
        {
            IsPause = !IsPause;
            StatusMessage = IsPause ? "Match paused." : "Match running...";
            PauseContinueButtonText = IsPause ? "Continue" : "Pause";
        }

        [RelayCommand]
        private async Task OnStop()
        {
            if (!IsStop)
            {
                await StopAsync();
                ResetMatchInfo();
                IsStop = !IsStop;
            }
            else
            {
                ResetMatchInfo();
                StopStartButtonText = "Stop Simulation";
                IsStop = !IsStop;
                await StartAsync();
            }
        }
    }
}
