using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FMSimTools.Models;
using FMSimTools.Services;

namespace FMSimTools.ViewModels
{
    public sealed partial class PlayerEditorViewModel : ObservableObject
    {
        [ObservableProperty]
        private string _name = string.Empty;

        [ObservableProperty]
        private PlayerPosition _selectedPosition;

        public PlayerEditorViewModel(PlayerPosition defaultPosition, TeamPlayer? player = null)
        {
            AvailablePositions = new ObservableCollection<PlayerPosition>(
                Enum.GetValues<PlayerPosition>());

            Attributes = new ObservableCollection<PlayerAttributeViewModel>(
            [
                new PlayerAttributeViewModel("Defense", player?.Attributes.Defense ?? 50),
                new PlayerAttributeViewModel("Attack", player?.Attributes.Attack ?? 50),
                new PlayerAttributeViewModel("Pace", player?.Attributes.Pace ?? 50),
                new PlayerAttributeViewModel("Stamina", player?.Attributes.Stamina ?? 50),
                new PlayerAttributeViewModel("Passing", player?.Attributes.Passing ?? 50),
                new PlayerAttributeViewModel("Tackle", player?.Attributes.Tackle ?? 50),
                new PlayerAttributeViewModel("Dribbling", player?.Attributes.Dribbling ?? 50),
                new PlayerAttributeViewModel("Shooting", player?.Attributes.Shooting ?? 50),
                new PlayerAttributeViewModel("Physicality", player?.Attributes.Physicality ?? 50),
                new PlayerAttributeViewModel("Jumping", player?.Attributes.Jumping ?? 50),
                new PlayerAttributeViewModel("ReactionTime", player?.Attributes.ReactionTime ?? 50)
            ]);

            _name = player?.Name ?? string.Empty;
            _selectedPosition = player?.Position ?? defaultPosition;
        }

        public ObservableCollection<PlayerPosition> AvailablePositions { get; }

        public ObservableCollection<PlayerAttributeViewModel> Attributes { get; }

        public TeamPlayer? ResultPlayer { get; private set; }

        public event EventHandler<bool>? CloseRequested;

        [RelayCommand]
        private void OnRandomizePlayer()
        {
            var player = RandomPlayerFactory.Create(SelectedPosition);
            Name = player.Name;

            SetAttributeValue("Defense", player.Attributes.Defense);
            SetAttributeValue("Attack", player.Attributes.Attack);
            SetAttributeValue("Pace", player.Attributes.Pace);
            SetAttributeValue("Stamina", player.Attributes.Stamina);
            SetAttributeValue("Passing", player.Attributes.Passing);
            SetAttributeValue("Tackle", player.Attributes.Tackle);
            SetAttributeValue("Dribbling", player.Attributes.Dribbling);
            SetAttributeValue("Shooting", player.Attributes.Shooting);
            SetAttributeValue("Physicality", player.Attributes.Physicality);
            SetAttributeValue("Jumping", player.Attributes.Jumping);
            SetAttributeValue("ReactionTime", player.Attributes.ReactionTime);
        }

        [RelayCommand]
        private void OnSave()
        {
            ResultPlayer = new TeamPlayer
            {
                Name = string.IsNullOrWhiteSpace(Name) ? "Unnamed Player" : Name.Trim(),
                Position = SelectedPosition,
                Attributes = new PlayerAttributes
                {
                    Defense = GetAttributeValue("Defense"),
                    Attack = GetAttributeValue("Attack"),
                    Pace = GetAttributeValue("Pace"),
                    Stamina = GetAttributeValue("Stamina"),
                    Passing = GetAttributeValue("Passing"),
                    Tackle = GetAttributeValue("Tackle"),
                    Dribbling = GetAttributeValue("Dribbling"),
                    Shooting = GetAttributeValue("Shooting"),
                    Physicality = GetAttributeValue("Physicality"),
                    Jumping = GetAttributeValue("Jumping"),
                    ReactionTime = GetAttributeValue("ReactionTime")
                }
            };

            CloseRequested?.Invoke(this, true);
        }

        [RelayCommand]
        private void OnCancel()
        {
            CloseRequested?.Invoke(this, false);
        }

        private int GetAttributeValue(string name)
        {
            return Attributes.First(attribute => attribute.Name == name).Value;
        }

        private void SetAttributeValue(string name, int value)
        {
            Attributes.First(attribute => attribute.Name == name).Value = value;
        }
    }

    public sealed class PlayerAttributeViewModel : ObservableObject
    {
        private int _value;

        public PlayerAttributeViewModel(string name, int value)
        {
            Name = name;
            _value = value;
        }

        public string Name { get; }

        public int Value
        {
            get => _value;
            set => SetProperty(ref _value, Math.Clamp(value, 0, 100));
        }
    }
}
