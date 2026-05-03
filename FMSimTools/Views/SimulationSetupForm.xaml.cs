using FMSimTools.Models;
using FMSimTools.ViewModels;
using Wpf.Ui.Controls;

namespace FMSimTools.Views
{
    /// <summary>
    /// Interaction logic for SimulationSetupForm.xaml
    /// </summary>
    public partial class SimulationSetupForm : FluentWindow
    {
        public SimulationSetupForm(SimulationSetupViewModel viewModel)
        {
            InitializeComponent();

            DataContext = viewModel;
            viewModel.CloseRequested += ViewModel_CloseRequested;
            viewModel.LiveSimulationRequested += ViewModel_LiveSimulationRequested;
        }

        public event EventHandler<LiveSimulationRequest>? LiveSimulationRequested;

        private void ViewModel_CloseRequested(object? sender, EventArgs e)
        {
            Close();
        }

        private void ViewModel_LiveSimulationRequested(object? sender, LiveSimulationRequest request)
        {
            LiveSimulationRequested?.Invoke(this, request);
            Close();
        }
    }
}
