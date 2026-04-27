using System.Windows;
using System.Windows.Input;
using FMSimTools.ViewModels;
using Wpf.Ui.Controls;

namespace FMSimTools.Views
{
    /// <summary>
    /// Interaction logic for TeamViewerForm.xaml
    /// </summary>
    public partial class TeamViewerForm : FluentWindow
    {
        public TeamViewerForm()
        {
            InitializeComponent();
            DataContextChanged += TeamViewerForm_DataContextChanged;
        }

        private TeamViewerViewModel? CurrentViewModel { get; set; }

        private void TeamViewerForm_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (CurrentViewModel is not null)
            {
                CurrentViewModel.EditPlayerRequested -= ViewModel_EditPlayerRequested;
                CurrentViewModel.CloseRequested -= ViewModel_CloseRequested;
            }

            CurrentViewModel = e.NewValue as TeamViewerViewModel;

            if (CurrentViewModel is not null)
            {
                CurrentViewModel.EditPlayerRequested += ViewModel_EditPlayerRequested;
                CurrentViewModel.CloseRequested += ViewModel_CloseRequested;
            }
        }

        private void ViewModel_CloseRequested(object? sender, EventArgs e)
        {
            Close();
        }

        private void ViewModel_EditPlayerRequested(object? sender, TeamPlayerSlotViewModel playerSlot)
        {
            var editorViewModel = new PlayerEditorViewModel(playerSlot.DefaultPosition, playerSlot.Player);
            var editor = new PlayerEditorForm(editorViewModel)
            {
                Owner = this
            };

            if (editor.ShowDialog() == true && editorViewModel.ResultPlayer is not null)
            {
                playerSlot.SetPlayer(editorViewModel.ResultPlayer);
                CurrentViewModel?.NotifyTeamPlayerChanged();
            }
        }

        private void PlayerDot_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton != MouseButtonState.Pressed ||
                sender is not FrameworkElement { DataContext: PlayerSlotViewModel playerSlot } playerDot)
            {
                return;
            }

            DragDrop.DoDragDrop(playerDot, playerSlot, DragDropEffects.Move);
        }

        private void FormationLine_DragOver(object sender, DragEventArgs e)
        {
            e.Effects = CanDropPlayer(sender, e, out _, out _) ? DragDropEffects.Move : DragDropEffects.None;
            e.Handled = true;
        }

        private void FormationLine_Drop(object sender, DragEventArgs e)
        {
            if (CanDropPlayer(sender, e, out var viewModel, out var targetLine))
            {
                var sourcePlayerSlot = (PlayerSlotViewModel)e.Data.GetData(typeof(PlayerSlotViewModel));
                _ = viewModel.MovePlayer(sourcePlayerSlot, targetLine.Index);
            }

            e.Handled = true;
        }

        private bool CanDropPlayer(
            object sender,
            DragEventArgs e,
            out TeamViewerViewModel viewModel,
            out FormationLineViewModel targetLine)
        {
            viewModel = null!;
            targetLine = null!;

            if (!e.Data.GetDataPresent(typeof(PlayerSlotViewModel)) ||
                sender is not FrameworkElement { DataContext: FormationLineViewModel line } ||
                DataContext is not TeamViewerViewModel teamViewerViewModel)
            {
                return false;
            }

            var sourcePlayerSlot = (PlayerSlotViewModel)e.Data.GetData(typeof(PlayerSlotViewModel));

            if (!teamViewerViewModel.CanMovePlayer(sourcePlayerSlot.LineIndex, line.Index))
            {
                return false;
            }

            viewModel = teamViewerViewModel;
            targetLine = line;

            return true;
        }
    }
}
