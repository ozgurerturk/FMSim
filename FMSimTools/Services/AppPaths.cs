using System.IO;

namespace FMSimTools.Services
{
    public static class AppPaths
    {
        public static string AppDirectory => AppContext.BaseDirectory;

        public static string RepositoryRoot =>
            Path.GetFullPath(Path.Combine(AppDirectory, "..", "..", "..", ".."));

        public static string TeamsDirectory => ResolveDirectory("Teams");

        public static string LogsDirectory => ResolveDirectory("Logs");

        public static string SimulatorExecutablePath
        {
            get
            {
                var packagedPath = Path.Combine(AppDirectory, "FMSim.exe");
                if (File.Exists(packagedPath))
                {
                    return packagedPath;
                }

                return Path.Combine(RepositoryRoot, "x64", "Debug", "FMSim.exe");
            }
        }

        public static string SimulatorWorkingDirectory
        {
            get
            {
                var packagedPath = Path.Combine(AppDirectory, "FMSim.exe");
                return File.Exists(packagedPath) ? AppDirectory : RepositoryRoot;
            }
        }

        private static string ResolveDirectory(string name)
        {
            var packagedPath = Path.Combine(AppDirectory, name);
            if (File.Exists(Path.Combine(AppDirectory, "FMSim.exe")) || Directory.Exists(packagedPath))
            {
                return packagedPath;
            }

            return Path.Combine(RepositoryRoot, name);
        }
    }
}
