namespace FMSimTools.Models
{
    public sealed class PlayerAttributes
    {
        public int Defense { get; set; }
        public int Attack { get; set; }
        public int Pace { get; set; }
        public int Stamina { get; set; }
        public int Passing { get; set; }
        public int Tackle { get; set; }
        public int Dribbling { get; set; }
        public int Shooting { get; set; }
        public int Physicality { get; set; }
        public int Jumping { get; set; }
        public int ReactionTime { get; set; }

        public int GoalKeeping => (int)Math.Round((Jumping + ReactionTime) / 2.0);
    }
}
