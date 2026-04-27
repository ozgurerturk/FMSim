using FMSimTools.Models;
using FMSimTools.ViewModels;
using Wpf.Ui.Controls;

namespace FMSimTools.Views
{
    /// <summary>
    /// Interaction logic for MatchHistoryForm.xaml
    /// </summary>
    public partial class MatchHistoryForm : FluentWindow
    {
        public MatchHistoryForm(MatchHistoryViewModel viewModel)
        {
            InitializeComponent();

            DataContext = viewModel;
            viewModel.MatchOpened += ViewModel_MatchOpened;
        }

        public event EventHandler<SimulationResult>? MatchOpened;

        private void ViewModel_MatchOpened(object? sender, SimulationResult result)
        {
            MatchOpened?.Invoke(this, result);
        }
    }
}
