using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;
using FMSimTools.Models;

namespace FMSimTools.Converters
{
    public sealed class PlayerPositionBrushConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var position = value is PlayerPosition playerPosition ? playerPosition : PlayerPosition.Midfielder;
            var brushKind = parameter as string;

            return brushKind switch
            {
                "Accent" => CreateAccentBrush(position),
                "Foreground" => CreateForegroundBrush(position),
                _ => CreateBackgroundBrush(position)
            };
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }

        private static SolidColorBrush CreateBackgroundBrush(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => FrozenBrush(Color.FromArgb(40, 196, 142, 0)),
                PlayerPosition.Defender => FrozenBrush(Color.FromArgb(42, 128, 0, 32)),
                PlayerPosition.Midfielder => FrozenBrush(Color.FromArgb(38, 70, 130, 180)),
                PlayerPosition.Attacker => FrozenBrush(Color.FromArgb(45, 128, 128, 0)),
                _ => FrozenBrush(Color.FromArgb(28, 92, 101, 112))
            };
        }

        private static SolidColorBrush CreateAccentBrush(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => FrozenBrush(Color.FromRgb(196, 142, 0)),
                PlayerPosition.Defender => FrozenBrush(Color.FromRgb(128, 0, 32)),
                PlayerPosition.Midfielder => FrozenBrush(Color.FromRgb(70, 130, 180)),
                PlayerPosition.Attacker => FrozenBrush(Color.FromRgb(128, 128, 0)),
                _ => FrozenBrush(Color.FromRgb(92, 101, 112))
            };
        }

        private static SolidColorBrush CreateForegroundBrush(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => FrozenBrush(Color.FromRgb(92, 62, 0)),
                PlayerPosition.Defender => FrozenBrush(Color.FromRgb(92, 0, 23)),
                PlayerPosition.Midfielder => FrozenBrush(Color.FromRgb(27, 75, 116)),
                PlayerPosition.Attacker => FrozenBrush(Color.FromRgb(82, 82, 0)),
                _ => FrozenBrush(Color.FromRgb(35, 42, 52))
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
