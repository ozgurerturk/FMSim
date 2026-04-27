using FMSimTools.ViewModels;
using Wpf.Ui.Controls;

namespace FMSimTools.Views
{
    /// <summary>
    /// Interaction logic for PlayerEditorForm.xaml
    /// </summary>
    public partial class PlayerEditorForm : FluentWindow
    {
        public PlayerEditorForm(PlayerEditorViewModel viewModel)
        {
            InitializeComponent();

            DataContext = viewModel;
            viewModel.CloseRequested += ViewModel_CloseRequested;
        }

        private void ViewModel_CloseRequested(object? sender, bool result)
        {
            DialogResult = result;
        }
    }
}
