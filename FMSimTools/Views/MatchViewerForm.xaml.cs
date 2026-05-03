using System.Windows;
using FMSimTools.ViewModels;
using Wpf.Ui.Controls;

namespace FMSimTools.Views
{
    /// <summary>
    /// Interaction logic for MatchViewerForm.xaml
    /// </summary>
    public partial class MatchViewerForm : FluentWindow
    {
        public MatchViewerForm(MatchViewerViewModel viewModel)
        {
            InitializeComponent();

            DataContext = viewModel;
            Loaded += MatchViewerForm_Loaded;
            Closed += MatchViewerForm_Closed;
        }

        private MatchViewerViewModel ViewModel => (MatchViewerViewModel)DataContext;

        private async void MatchViewerForm_Loaded(object sender, RoutedEventArgs e)
        {
            await ViewModel.StartAsync();
        }

        private void MatchViewerForm_Closed(object? sender, EventArgs e)
        {
            ViewModel.Stop();
        }
    }
}
