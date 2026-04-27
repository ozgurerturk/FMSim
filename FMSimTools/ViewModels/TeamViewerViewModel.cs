using System.Collections.ObjectModel;
using System.IO;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FMSimTools.Models;
using FMSimTools.Services;

namespace FMSimTools.ViewModels
{
    public partial class TeamViewerViewModel : ObservableObject
    {
        private const int MaxPlayersPerLine = 6;
        private const int MaxTeamPlayers = 11;

        private readonly int[] _lineCounts = new int[4];

        [ObservableProperty]
        private Formation? _selectedFormation;

        [ObservableProperty]
        private string _currentFormationName = string.Empty;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(SaveTeamCommand))]
        private string _teamName = string.Empty;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(SaveTeamCommand))]
        private string? _selectedAttackTactic;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(SaveTeamCommand))]
        private string? _selectedDefenseTactic;

        [ObservableProperty]
        private string _saveStatusMessage = string.Empty;

        public TeamPlayerSlotViewModel? GoalKeeperSlot =>
            TeamPlayerSlots.FirstOrDefault(slot => slot.DefaultPosition == PlayerPosition.Goalkeeper);

        public ObservableCollection<Formation> Formations { get; }

        public ObservableCollection<FormationLineViewModel> FormationLines { get; } = [];

        public ObservableCollection<TeamPlayerSlotViewModel> TeamPlayerSlots { get; } = [];

        public ObservableCollection<string> AvailableAttackTactics { get; } =
        [
            "Possession",
            "WingPlay",
            "LongBall",
            "TikiTaka",
            "CounterAttack"
        ];

        public ObservableCollection<string> AvailableDefenseTactics { get; } =
        [
            "Pressing",
            "ManMarking",
            "ZonalMarking",
            "CounterPressing"
        ];

        public TeamViewerViewModel()
        {
            Formations = new ObservableCollection<Formation>(FormationCatalog.All);
            SelectedFormation = Formations.FirstOrDefault();
        }

        public event EventHandler<TeamPlayerSlotViewModel>? EditPlayerRequested;

        public event EventHandler? CloseRequested;

        public void LoadTeam(SavedTeam savedTeam)
        {
            TeamName = savedTeam.Name;
            SelectedAttackTactic = savedTeam.AttackTactic;
            SelectedDefenseTactic = savedTeam.DefenseTactic;

            Array.Clear(_lineCounts);

            foreach (var player in savedTeam.Players)
            {
                var lineIndex = GetLineIndex(player.Position);
                if (lineIndex >= 0)
                {
                    _lineCounts[lineIndex]++;
                }
            }

            RebuildFormationLines();
            RebuildTeamPlayerSlots(savedTeam.Players);
            RefreshCurrentFormationName();
            SyncSelectedFormationWithCurrentShape();
            OnPropertyChanged(nameof(GoalKeeperSlot));
            SaveStatusMessage = string.Empty;
            AddPlayerCommand.NotifyCanExecuteChanged();
            SaveTeamCommand.NotifyCanExecuteChanged();
        }

        public bool MovePlayer(PlayerSlotViewModel sourcePlayerSlot, int targetLineIndex)
        {
            if (!CanMovePlayer(sourcePlayerSlot.LineIndex, targetLineIndex))
            {
                return false;
            }

            var sourceLineIndex = sourcePlayerSlot.LineIndex;
            _lineCounts[sourceLineIndex]--;
            _lineCounts[targetLineIndex]++;

            AdjustTeamPlayerSlotForMove(sourcePlayerSlot.TeamPlayerSlot, targetLineIndex);
            RefreshCurrentFormationName();
            SyncSelectedFormationWithCurrentShape();
            RebuildFormationLines();
            AddPlayerCommand.NotifyCanExecuteChanged();
            SaveTeamCommand.NotifyCanExecuteChanged();

            return true;
        }

        public bool CanMovePlayer(int sourceLineIndex, int targetLineIndex)
        {
            return sourceLineIndex != targetLineIndex &&
                   IsValidLineIndex(sourceLineIndex) &&
                   IsValidLineIndex(targetLineIndex) &&
                   _lineCounts[sourceLineIndex] > 0 &&
                   _lineCounts[targetLineIndex] < MaxPlayersPerLine;
        }

        public void NotifyTeamPlayerChanged()
        {
            SaveStatusMessage = string.Empty;
            RebuildFormationLines();
            OnPropertyChanged(nameof(GoalKeeperSlot));
            SaveTeamCommand.NotifyCanExecuteChanged();
        }

        [RelayCommand]
        private void OnCancel()
        {
            CloseRequested?.Invoke(this, EventArgs.Empty);
        }

        [RelayCommand]
        private void OnFillRandomTeam()
        {
            foreach (var playerSlot in TeamPlayerSlots)
            {
                playerSlot.SetPlayer(RandomPlayerFactory.Create(playerSlot.DefaultPosition));
            }

            AddPlayerCommand.NotifyCanExecuteChanged();
            SaveTeamCommand.NotifyCanExecuteChanged();
            RebuildFormationLines();
            OnPropertyChanged(nameof(GoalKeeperSlot));
            SaveStatusMessage = string.Empty;
        }

        [RelayCommand(CanExecute = nameof(CanSaveTeam))]
        private void OnSaveTeam()
        {
            try
            {
                var teamsDirectory = TeamJsonStore.GetTeamsDirectory();
                var filePath = TeamJsonWriter.SaveTeam(
                    teamsDirectory,
                    TeamName.Trim(),
                    SelectedAttackTactic!,
                    SelectedDefenseTactic!,
                    TeamPlayerSlots);

                SaveStatusMessage = $"Saved: {Path.GetFileName(filePath)}";
            }
            catch (Exception exception)
            {
                SaveStatusMessage = $"Save failed: {exception.Message}";
            }
        }

        private bool CanSaveTeam()
        {
            return !string.IsNullOrWhiteSpace(TeamName) &&
                   !string.IsNullOrWhiteSpace(SelectedAttackTactic) &&
                   !string.IsNullOrWhiteSpace(SelectedDefenseTactic) &&
                   TeamPlayerSlots.Count == 11 &&
                   TeamPlayerSlots.All(slot =>
                       slot.Player is not null &&
                       !string.IsNullOrWhiteSpace(slot.Player.Name) &&
                       HasValidAttributes(slot.Player.Attributes));
        }

        [RelayCommand(CanExecute = nameof(CanAddPlayer))]
        private void OnAddPlayer()
        {
            RequestEditPlayer(CreateDetachedPlayerSlot());
        }

        private bool CanAddPlayer()
        {
            return TeamPlayerSlots.Count < MaxTeamPlayers;
        }

        [RelayCommand]
        private void OnEditPlayerSlot(TeamPlayerSlotViewModel? playerSlot)
        {
            RequestEditPlayer(playerSlot);
        }

        partial void OnSelectedFormationChanged(Formation? value)
        {
            ApplyFormation(value);
        }

        private void ApplyFormation(Formation? formation)
        {
            FormationLines.Clear();
            SaveStatusMessage = string.Empty;

            if (formation is null)
            {
                Array.Clear(_lineCounts);
                CurrentFormationName = string.Empty;
                AddPlayerCommand.NotifyCanExecuteChanged();
                SaveTeamCommand.NotifyCanExecuteChanged();
                return;
            }

            for (var lineIndex = 0; lineIndex < _lineCounts.Length; lineIndex++)
            {
                _lineCounts[lineIndex] = lineIndex < formation.Lines.Count ? formation.Lines[lineIndex] : 0;
            }

            RebuildFormationLines();
            RebuildEmptyTeamPlayerSlots();
            SortTeamPlayerSlots();
            OnPropertyChanged(nameof(GoalKeeperSlot));
            RefreshCurrentFormationName();
            AddPlayerCommand.NotifyCanExecuteChanged();
            SaveTeamCommand.NotifyCanExecuteChanged();
        }

        private void RebuildFormationLines()
        {
            FormationLines.Clear();
            var lineSlots = CreateLineSlotMap();

            for (var lineIndex = _lineCounts.Length - 1; lineIndex >= 0; lineIndex--)
            {
                FormationLines.Add(new FormationLineViewModel(lineIndex, lineSlots[lineIndex]));
            }
        }

        private Dictionary<int, IReadOnlyList<TeamPlayerSlotViewModel>> CreateLineSlotMap()
        {
            var slotsByPosition = TeamPlayerSlots
                .GroupBy(slot => slot.DefaultPosition)
                .ToDictionary(group => group.Key, group => group.ToList());

            var positionOffsets = new Dictionary<PlayerPosition, int>();
            var lineSlots = new Dictionary<int, IReadOnlyList<TeamPlayerSlotViewModel>>();

            for (var lineIndex = 0; lineIndex < _lineCounts.Length; lineIndex++)
            {
                var position = GetDefaultPosition(lineIndex);
                var offset = positionOffsets.GetValueOrDefault(position);

                if (!slotsByPosition.TryGetValue(position, out var slots))
                {
                    slots = [];
                }

                lineSlots[lineIndex] = slots.Skip(offset).Take(_lineCounts[lineIndex]).ToArray();
                positionOffsets[position] = offset + _lineCounts[lineIndex];
            }

            return lineSlots;
        }

        private void RefreshCurrentFormationName()
        {
            CurrentFormationName = string.Join("-", _lineCounts.Where(playerCount => playerCount > 0));
        }

        private void RebuildEmptyTeamPlayerSlots()
        {
            TeamPlayerSlots.Clear();
            TeamPlayerSlots.Add(new TeamPlayerSlotViewModel(PlayerPosition.Goalkeeper, "GK"));

            for (var lineIndex = 0; lineIndex < _lineCounts.Length; lineIndex++)
            {
                var defaultPosition = GetDefaultPosition(lineIndex);

                for (var slotIndex = 1; slotIndex <= _lineCounts[lineIndex]; slotIndex++)
                {
                    TeamPlayerSlots.Add(new TeamPlayerSlotViewModel(defaultPosition, $"{defaultPosition} {slotIndex}"));
                }
            }

            OnPropertyChanged(nameof(GoalKeeperSlot));
        }

        private void RebuildTeamPlayerSlots(IReadOnlyList<TeamPlayer> players)
        {
            TeamPlayerSlots.Clear();

            foreach (var player in players)
            {
                var slot = new TeamPlayerSlotViewModel(player.Position, player.Position == PlayerPosition.Goalkeeper ? "GK" : player.Position.ToString());
                slot.SetPlayer(player);
                TeamPlayerSlots.Add(slot);
            }

            SortTeamPlayerSlots();
            RenumberTeamPlayerSlots();
            OnPropertyChanged(nameof(GoalKeeperSlot));
            RebuildFormationLines();
        }

        private void AdjustTeamPlayerSlotForMove(TeamPlayerSlotViewModel playerSlot, int targetLineIndex)
        {
            var targetPosition = GetDefaultPosition(targetLineIndex);
            playerSlot.ChangeDefaultPosition(targetPosition);
            SortTeamPlayerSlots();
            RenumberTeamPlayerSlots();
        }

        private TeamPlayerSlotViewModel CreateDetachedPlayerSlot()
        {
            var playerSlot = new TeamPlayerSlotViewModel(PlayerPosition.Midfielder, "New Player");
            TeamPlayerSlots.Add(playerSlot);
            SortTeamPlayerSlots();
            RenumberTeamPlayerSlots();
            AddPlayerCommand.NotifyCanExecuteChanged();
            SaveTeamCommand.NotifyCanExecuteChanged();
            return playerSlot;
        }

        private void RequestEditPlayer(TeamPlayerSlotViewModel? playerSlot)
        {
            if (playerSlot is null)
            {
                return;
            }

            EditPlayerRequested?.Invoke(this, playerSlot);
        }

        private void SyncSelectedFormationWithCurrentShape()
        {
            var matchingFormation = Formations.FirstOrDefault(formation => formation.Lines.SequenceEqual(_lineCounts));

            if (_selectedFormation == matchingFormation)
            {
                return;
            }

            SetProperty(ref _selectedFormation, matchingFormation, nameof(SelectedFormation));
        }

        private static bool IsValidLineIndex(int lineIndex)
        {
            return lineIndex is >= 0 and < 4;
        }

        private static bool HasValidAttributes(PlayerAttributes attributes)
        {
            return IsValidAttribute(attributes.Defense) &&
                   IsValidAttribute(attributes.Attack) &&
                   IsValidAttribute(attributes.Pace) &&
                   IsValidAttribute(attributes.Stamina) &&
                   IsValidAttribute(attributes.Passing) &&
                   IsValidAttribute(attributes.Tackle) &&
                   IsValidAttribute(attributes.Dribbling) &&
                   IsValidAttribute(attributes.Shooting) &&
                   IsValidAttribute(attributes.Physicality) &&
                   IsValidAttribute(attributes.Jumping) &&
                   IsValidAttribute(attributes.ReactionTime);
        }

        private static bool IsValidAttribute(int value)
        {
            return value is >= 0 and <= 100;
        }

        private void SortTeamPlayerSlots()
        {
            var sortedSlots = TeamPlayerSlots
                .OrderBy(slot => GetPositionOrder(slot.DefaultPosition))
                .ThenBy(slot => slot.SlotName)
                .ToArray();

            TeamPlayerSlots.Clear();

            foreach (var playerSlot in sortedSlots)
            {
                TeamPlayerSlots.Add(playerSlot);
            }
        }

        private void RenumberTeamPlayerSlots()
        {
            var counters = new Dictionary<PlayerPosition, int>();

            foreach (var playerSlot in TeamPlayerSlots)
            {
                var position = playerSlot.DefaultPosition;
                counters[position] = counters.GetValueOrDefault(position) + 1;
                playerSlot.SetSlotName(position == PlayerPosition.Goalkeeper ? "GK" : $"{position} {counters[position]}");
            }
        }

        private static int GetPositionOrder(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => 0,
                PlayerPosition.Defender => 1,
                PlayerPosition.Midfielder => 2,
                PlayerPosition.Attacker => 3,
                _ => 4
            };
        }

        private static PlayerPosition GetDefaultPosition(int lineIndex)
        {
            return lineIndex switch
            {
                0 => PlayerPosition.Defender,
                3 => PlayerPosition.Attacker,
                _ => PlayerPosition.Midfielder
            };
        }

        private static int GetLineIndex(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Defender => 0,
                PlayerPosition.Midfielder => 2,
                PlayerPosition.Attacker => 3,
                _ => -1
            };
        }
    }

    public sealed class FormationLineViewModel
    {
        public FormationLineViewModel(int index, IReadOnlyList<TeamPlayerSlotViewModel> teamPlayerSlots)
        {
            Index = index;
            PlayerSlots = new ObservableCollection<PlayerSlotViewModel>(
                teamPlayerSlots.Select((teamPlayerSlot, slotIndex) => new PlayerSlotViewModel(index, slotIndex + 1, teamPlayerSlot)));
        }

        public int Index { get; }

        public ObservableCollection<PlayerSlotViewModel> PlayerSlots { get; }
    }

    public sealed class PlayerSlotViewModel
    {
        public PlayerSlotViewModel(int lineIndex, int number, TeamPlayerSlotViewModel teamPlayerSlot)
        {
            LineIndex = lineIndex;
            Number = number;
            TeamPlayerSlot = teamPlayerSlot;
        }

        public int LineIndex { get; }

        public int Number { get; }

        public TeamPlayerSlotViewModel TeamPlayerSlot { get; }

        public string DisplayText => Number.ToString();

        public string TooltipText
        {
            get
            {
                if (TeamPlayerSlot.Player is null)
                {
                    return $"{TeamPlayerSlot.DisplayName}\n{TeamPlayerSlot.PositionText}";
                }

                var player = TeamPlayerSlot.Player;
                var lines = new List<string>
                {
                    player.Name,
                    player.Position.ToString()
                };

                lines.AddRange(GetImportantAttributeLines(player));

                return string.Join(Environment.NewLine, lines);
            }
        }

        private static IEnumerable<string> GetImportantAttributeLines(TeamPlayer player)
        {
            var attributes = player.Attributes;

            return player.Position switch
            {
                PlayerPosition.Goalkeeper =>
                [
                    $"Jumping: {attributes.Jumping}",
                    $"Reaction: {attributes.ReactionTime}",
                    $"Physicality: {attributes.Physicality}"
                ],
                PlayerPosition.Defender =>
                [
                    $"Defense: {attributes.Defense}",
                    $"Tackle: {attributes.Tackle}",
                    $"Physicality: {attributes.Physicality}",
                    $"Jumping: {attributes.Jumping}"
                ],
                PlayerPosition.Midfielder =>
                [
                    $"Passing: {attributes.Passing}",
                    $"Stamina: {attributes.Stamina}",
                    $"Dribbling: {attributes.Dribbling}",
                    $"Attack: {attributes.Attack}",
                    $"Defense: {attributes.Defense}"
                ],
                PlayerPosition.Attacker =>
                [
                    $"Attack: {attributes.Attack}",
                    $"Shooting: {attributes.Shooting}",
                    $"Pace: {attributes.Pace}",
                    $"Dribbling: {attributes.Dribbling}"
                ],
                _ => []
            };
        }
    }

    public sealed class TeamPlayerSlotViewModel : ObservableObject
    {
        private TeamPlayer? _player;

        public TeamPlayerSlotViewModel(PlayerPosition defaultPosition, string slotName)
        {
            DefaultPosition = defaultPosition;
            SlotName = slotName;
        }

        public PlayerPosition DefaultPosition { get; private set; }

        public string SlotName { get; private set; }

        public TeamPlayer? Player
        {
            get => _player;
            private set
            {
                if (SetProperty(ref _player, value))
                {
                    OnPropertyChanged(nameof(DisplayName));
                    OnPropertyChanged(nameof(PositionText));
                    OnPropertyChanged(nameof(PlayerOrDefaultPosition));
                    OnPropertyChanged(nameof(TooltipText));
                    OnPropertyChanged(nameof(IsEmpty));
                }
            }
        }

        public string DisplayName => Player?.Name ?? $"Empty {SlotName}";

        public string PositionText => (Player?.Position ?? DefaultPosition).ToString();

        public PlayerPosition PlayerOrDefaultPosition => Player?.Position ?? DefaultPosition;

        public string TooltipText
        {
            get
            {
                if (Player is null)
                {
                    return $"{DisplayName}\n{PositionText}";
                }

                var attributes = Player.Attributes;

                return string.Join(Environment.NewLine,
                [
                    Player.Name,
                    Player.Position.ToString(),
                    $"Jumping: {attributes.Jumping}",
                    $"Reaction: {attributes.ReactionTime}",
                    $"Physicality: {attributes.Physicality}"
                ]);
            }
        }

        public bool IsEmpty => Player is null;

        public void SetPlayer(TeamPlayer player)
        {
            Player = player;
        }

        public void ChangeDefaultPosition(PlayerPosition position)
        {
            if (DefaultPosition == position)
            {
                return;
            }

            DefaultPosition = position;
            if (Player is not null)
            {
                Player = new TeamPlayer
                {
                    Name = Player.Name,
                    Position = position,
                    Attributes = Player.Attributes
                };
            }

            OnPropertyChanged(nameof(DefaultPosition));
            OnPropertyChanged(nameof(PositionText));
            OnPropertyChanged(nameof(PlayerOrDefaultPosition));
            OnPropertyChanged(nameof(TooltipText));
        }

        public void SetSlotName(string slotName)
        {
            if (SlotName == slotName)
            {
                return;
            }

            SlotName = slotName;
            OnPropertyChanged(nameof(SlotName));
            OnPropertyChanged(nameof(DisplayName));
            OnPropertyChanged(nameof(TooltipText));
        }
    }
}
