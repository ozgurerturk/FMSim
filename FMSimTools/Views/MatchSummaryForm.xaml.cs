using FMSimTools.ViewModels;
using Wpf.Ui.Controls;

namespace FMSimTools.Views
{
    /// <summary>
    /// Interaction logic for MatchSummaryForm.xaml
    /// </summary>
    public partial class MatchSummaryForm : FluentWindow
    {
        public MatchSummaryForm(MatchSummaryViewModel viewModel)
        {
            InitializeComponent();
            DataContext = viewModel;
        }
    }
}
