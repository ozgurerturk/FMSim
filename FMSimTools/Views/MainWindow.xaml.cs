using FMSimTools.Models;
using FMSimTools.ViewModels;
using Wpf.Ui.Controls;

namespace FMSimTools.Views
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : FluentWindow
    {
        public MainWindow()
        {
            InitializeComponent();

            var viewModel = new MainWindowViewModel();
            DataContext = viewModel;

            viewModel.CreateTeamRequested += ViewModel_CreateTeamRequested;
            viewModel.EditTeamRequested += ViewModel_EditTeamRequested;
            viewModel.KickOffRequested += ViewModel_KickOffRequested;
            viewModel.MatchHistoryRequested += ViewModel_MatchHistoryRequested;
            viewModel.MatchViewerRequested += ViewModel_MatchViewerRequested;
        }

        private void ViewModel_MatchViewerRequested(object? sender, EventArgs e)
        {
            var matchViewer = new MatchViewerForm(new MatchViewerViewModel())
            {
                Owner = this
            };

            _ = matchViewer.ShowDialog();
        }

        private MainWindowViewModel ViewModel => (MainWindowViewModel)DataContext;

        private void ViewModel_CreateTeamRequested(object? sender, EventArgs e)
        {
            ShowTeamViewer(new TeamViewerViewModel());
        }

        private void ViewModel_KickOffRequested(object? sender, EventArgs e)
        {
            var setup = new SimulationSetupForm(new SimulationSetupViewModel())
            {
                Owner = this
            };

            setup.LiveSimulationRequested += Setup_LiveSimulationRequested;
            _ = setup.ShowDialog();
        }

        private void Setup_LiveSimulationRequested(object? sender, LiveSimulationRequest request)
        {
            var viewer = new MatchViewerForm(new MatchViewerViewModel(request))
            {
                Owner = this
            };

            _ = viewer.ShowDialog();
        }

        private void ViewModel_MatchHistoryRequested(object? sender, EventArgs e)
        {
            var history = new MatchHistoryForm(new MatchHistoryViewModel())
            {
                Owner = this
            };

            history.MatchOpened += Setup_SimulationCompleted;
            _ = history.ShowDialog();
        }

        private void Setup_SimulationCompleted(object? sender, SimulationResult result)
        {
            var summary = new MatchSummaryForm(new MatchSummaryViewModel(result))
            {
                Owner = this
            };
            _ = summary.ShowDialog();
        }

        private void ViewModel_EditTeamRequested(object? sender, SavedTeam savedTeam)
        {
            var viewModel = new TeamViewerViewModel();
            viewModel.LoadTeam(savedTeam);
            ShowTeamViewer(viewModel);
        }

        private void ShowTeamViewer(TeamViewerViewModel viewModel)
        {
            var window = new TeamViewerForm
            {
                Owner = this,
                DataContext = viewModel
            };
            _ = window.ShowDialog();
            ViewModel.RefreshTeamsCommand.Execute(null);
        }
    }
}
