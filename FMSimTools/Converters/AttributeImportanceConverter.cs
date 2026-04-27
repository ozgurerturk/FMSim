using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;
using FMSimTools.Models;

namespace FMSimTools.Converters
{
    public sealed class AttributeImportanceConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            if (values.Length < 2 ||
                values[0] is not string attributeName ||
                values[1] is not PlayerPosition position)
            {
                return DependencyProperty.UnsetValue;
            }

            var isImportant = IsImportant(attributeName, position);
            var resultKind = parameter as string;

            return resultKind switch
            {
                "FontWeight" => isImportant ? FontWeights.SemiBold : FontWeights.Normal,
                "Background" => isImportant ? CreateHighlightBrush(position) : Brushes.Transparent,
                "BorderBrush" => isImportant ? CreateAccentBrush(position) : FrozenBrush(Color.FromArgb(24, 0, 0, 0)),
                "Opacity" => isImportant ? 1.0 : 0.82,
                _ => isImportant
            };
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }

        private static bool IsImportant(string attributeName, PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => attributeName is "Jumping" or "ReactionTime" or "Physicality",
                PlayerPosition.Defender => attributeName is "Defense" or "Tackle" or "Physicality" or "Jumping",
                PlayerPosition.Midfielder => attributeName is "Passing" or "Stamina" or "Dribbling" or "Defense" or "Attack",
                PlayerPosition.Attacker => attributeName is "Attack" or "Shooting" or "Pace" or "Dribbling",
                _ => false
            };
        }

        private static SolidColorBrush CreateHighlightBrush(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => FrozenBrush(Color.FromArgb(34, 196, 142, 0)),
                PlayerPosition.Defender => FrozenBrush(Color.FromArgb(32, 128, 0, 32)),
                PlayerPosition.Midfielder => FrozenBrush(Color.FromArgb(30, 70, 130, 180)),
                PlayerPosition.Attacker => FrozenBrush(Color.FromArgb(34, 128, 128, 0)),
                _ => FrozenBrush(Color.FromArgb(22, 92, 101, 112))
            };
        }

        private static SolidColorBrush CreateAccentBrush(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => FrozenBrush(Color.FromArgb(160, 196, 142, 0)),
                PlayerPosition.Defender => FrozenBrush(Color.FromArgb(150, 128, 0, 32)),
                PlayerPosition.Midfielder => FrozenBrush(Color.FromArgb(145, 70, 130, 180)),
                PlayerPosition.Attacker => FrozenBrush(Color.FromArgb(150, 128, 128, 0)),
                _ => FrozenBrush(Color.FromArgb(80, 92, 101, 112))
            };
        }

        private static SolidColorBrush FrozenBrush(Color color)
        {
            var brush = new SolidColorBrush(color);
            brush.Freeze();
            return brush;
        }
    }
}
