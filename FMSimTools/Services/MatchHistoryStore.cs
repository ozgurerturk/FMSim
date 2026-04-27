using System.IO;
using FMSimTools.Models;

namespace FMSimTools.Services
{
    public static class MatchHistoryStore
    {
        public static IReadOnlyList<MatchHistoryItem> LoadHistory()
        {
            var directories = new[]
            {
                AppPaths.LogsDirectory,
                Path.Combine(AppPaths.TeamsDirectory, "Logs")
            };

            return directories
                .Where(Directory.Exists)
                .SelectMany(directory => Directory.EnumerateFiles(directory, "match_commentary_*.txt", SearchOption.TopDirectoryOnly))
                .Select(CreateHistoryItem)
                .OrderByDescending(item => item.PlayedAt)
                .ToArray();
        }

        public static string ReadCommentary(MatchHistoryItem item)
        {
            return File.Exists(item.CommentaryPath)
                ? File.ReadAllText(item.CommentaryPath)
                : string.Empty;
        }

        private static MatchHistoryItem CreateHistoryItem(string commentaryPath)
        {
            var fileInfo = new FileInfo(commentaryPath);
            return new MatchHistoryItem
            {
                DisplayName = Path.GetFileNameWithoutExtension(commentaryPath),
                CommentaryPath = commentaryPath,
                PlayedAt = fileInfo.LastWriteTime,
                Length = fileInfo.Length,
                SourceFolder = fileInfo.Directory?.Name ?? string.Empty
            };
        }
    }
}
