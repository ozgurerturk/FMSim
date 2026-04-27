using System.Collections.Generic;

namespace FMSimTools.Models
{
    public static class FormationCatalog
    {
        public static IReadOnlyList<Formation> All { get; } =
        [
            new Formation("4-4-2", [4, 0, 4, 2]),
            new Formation("4-3-3", [4, 1, 2, 3]),
            new Formation("4-2-3-1", [4, 2, 3, 1]),
            new Formation("3-5-2", [3, 0, 5, 2]),
            new Formation("5-3-2", [5, 0, 3, 2]),
            new Formation("4-1-4-1", [4, 1, 4, 1])
        ];
    }
}
