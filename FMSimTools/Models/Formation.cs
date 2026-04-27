using System.Collections.Generic;

namespace FMSimTools.Models
{
    public sealed record Formation(string Name, IReadOnlyList<int> Lines);
}
