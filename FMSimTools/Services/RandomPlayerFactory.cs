using FMSimTools.Models;

namespace FMSimTools.Services
{
    public static class RandomPlayerFactory
    {
        private static readonly string[] FirstNames =
        [
            "Arda", "Kerem", "Mert", "Emir", "Deniz", "Kaan", "Efe", "Baran",
            "Ozan", "Yigit", "Can", "Tuna", "Bora", "Alp", "Doruk", "Eren"
        ];

        private static readonly string[] LastNames =
        [
            "Yilmaz", "Kaya", "Demir", "Celik", "Sahin", "Aydin", "Arslan", "Kilic",
            "Aslan", "Polat", "Kaplan", "Aksoy", "Erdem", "Turan", "Ozturk", "Keskin"
        ];

        public static TeamPlayer Create(PlayerPosition position)
        {
            return new TeamPlayer
            {
                Name = $"{FirstNames[Random.Shared.Next(FirstNames.Length)]} {LastNames[Random.Shared.Next(LastNames.Length)]}",
                Position = position,
                Attributes = CreateAttributes(position)
            };
        }

        private static PlayerAttributes CreateAttributes(PlayerPosition position)
        {
            return position switch
            {
                PlayerPosition.Goalkeeper => new PlayerAttributes
                {
                    Defense = Range(10, 35),
                    Attack = Range(5, 25),
                    Pace = Range(35, 65),
                    Stamina = Range(55, 85),
                    Passing = Range(35, 65),
                    Tackle = Range(10, 35),
                    Dribbling = Range(10, 35),
                    Shooting = Range(5, 25),
                    Physicality = Range(55, 85),
                    Jumping = Range(70, 95),
                    ReactionTime = Range(72, 98)
                },
                PlayerPosition.Defender => new PlayerAttributes
                {
                    Defense = Range(68, 94),
                    Attack = Range(25, 55),
                    Pace = Range(45, 78),
                    Stamina = Range(62, 90),
                    Passing = Range(42, 72),
                    Tackle = Range(68, 95),
                    Dribbling = Range(25, 58),
                    Shooting = Range(18, 48),
                    Physicality = Range(65, 94),
                    Jumping = Range(58, 88),
                    ReactionTime = Range(52, 82)
                },
                PlayerPosition.Midfielder => new PlayerAttributes
                {
                    Defense = Range(45, 78),
                    Attack = Range(48, 78),
                    Pace = Range(52, 82),
                    Stamina = Range(70, 96),
                    Passing = Range(68, 95),
                    Tackle = Range(42, 76),
                    Dribbling = Range(60, 90),
                    Shooting = Range(42, 76),
                    Physicality = Range(50, 82),
                    Jumping = Range(40, 72),
                    ReactionTime = Range(56, 86)
                },
                PlayerPosition.Attacker => new PlayerAttributes
                {
                    Defense = Range(15, 45),
                    Attack = Range(70, 96),
                    Pace = Range(65, 96),
                    Stamina = Range(58, 88),
                    Passing = Range(45, 78),
                    Tackle = Range(15, 45),
                    Dribbling = Range(68, 96),
                    Shooting = Range(70, 98),
                    Physicality = Range(52, 88),
                    Jumping = Range(48, 84),
                    ReactionTime = Range(58, 90)
                },
                _ => new PlayerAttributes()
            };
        }

        private static int Range(int minInclusive, int maxInclusive)
        {
            return Random.Shared.Next(minInclusive, maxInclusive + 1);
        }
    }
}
