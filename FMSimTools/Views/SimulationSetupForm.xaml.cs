using System.Windows;
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
            viewModel.SimulationCompleted += ViewModel_SimulationCompleted;
            viewModel.SimulationFailed += ViewModel_SimulationFailed;
        }

        public event EventHandler<SimulationResult>? SimulationCompleted;

        private void ViewModel_CloseRequested(object? sender, EventArgs e)
        {
            Close();
        }

        private void ViewModel_SimulationCompleted(object? sender, SimulationResult result)
        {
            SimulationCompleted?.Invoke(this, result);
            Close();
        }

        private void ViewModel_SimulationFailed(object? sender, string message)
        {
            System.Windows.MessageBox.Show(
                this,
                message,
                "Simulation failed",
                System.Windows.MessageBoxButton.OK,
                MessageBoxImage.Error);
        }
    }
}
