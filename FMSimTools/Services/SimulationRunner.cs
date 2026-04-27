using System.Diagnostics;
using System.IO;
using FMSimTools.Models;

namespace FMSimTools.Services
{
    public static class SimulationRunner
    {
        public static async Task<SimulationResult> RunAsync(
            SavedTeam homeTeam,
            SavedTeam awayTeam,
            int simulationMinutes,
            int simulationCount)
        {
            var executablePath = AppPaths.SimulatorExecutablePath;

            if (!File.Exists(executablePath))
            {
                throw new FileNotFoundException("FMSim.exe could not be found.", executablePath);
            }

            var logsDirectory = AppPaths.LogsDirectory;
            Directory.CreateDirectory(logsDirectory);

            var startedAt = DateTime.Now;
            using var process = new Process();
            process.StartInfo = new ProcessStartInfo
            {
                FileName = executablePath,
                WorkingDirectory = AppPaths.SimulatorWorkingDirectory,
                UseShellExecute = false,
                RedirectStandardInput = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true
            };
            process.StartInfo.Environment["FMSIM_HOME_JSON"] = homeTeam.FilePath;
            process.StartInfo.Environment["FMSIM_AWAY_JSON"] = awayTeam.FilePath;
            process.StartInfo.Environment["FMSIM_NON_INTERACTIVE"] = "1";
            process.StartInfo.Environment["FMSIM_SKIP_REPLAY"] = "1";

            process.Start();

            await process.StandardInput.WriteLineAsync(simulationMinutes.ToString());
            await process.StandardInput.WriteLineAsync(simulationCount.ToString());

            process.StandardInput.Close();

            var outputTask = process.StandardOutput.ReadToEndAsync();
            var errorTask = process.StandardError.ReadToEndAsync();

            var exitedTask = process.WaitForExitAsync();
            var timeoutTask = Task.Delay(TimeSpan.FromSeconds(90));
            if (await Task.WhenAny(exitedTask, timeoutTask) == timeoutTask)
            {
                try
                {
                    process.Kill(entireProcessTree: true);
                }
                catch (InvalidOperationException)
                {
                }

                throw new TimeoutException("Simulation did not finish in 90 seconds.");
            }

            await exitedTask;

            var outputText = await outputTask;
            var errorText = await errorTask;

            if (process.ExitCode != 0)
            {
                var details = string.IsNullOrWhiteSpace(errorText) ? outputText : errorText;
                await File.WriteAllTextAsync(
                    Path.Combine(logsDirectory, "simulation_error.txt"),
                    details);
                throw new InvalidOperationException($"Simulation failed with exit code {process.ExitCode}: {details}");
            }

            var commentaryPath = Directory
                .EnumerateFiles(logsDirectory, $"match_commentary_{simulationMinutes}*.txt")
                .Select(path => new FileInfo(path))
                .Where(file => file.LastWriteTime >= startedAt.AddSeconds(-2))
                .OrderByDescending(file => file.LastWriteTime)
                .FirstOrDefault()?.FullName;

            if (commentaryPath is null)
            {
                commentaryPath = Directory
                    .EnumerateFiles(logsDirectory, "match_commentary_*.txt")
                    .Select(path => new FileInfo(path))
                    .OrderByDescending(file => file.LastWriteTime)
                    .FirstOrDefault()?.FullName ?? string.Empty;
            }

            return new SimulationResult
            {
                CommentaryPath = commentaryPath,
                CommentaryText = string.IsNullOrWhiteSpace(commentaryPath) ? string.Empty : await File.ReadAllTextAsync(commentaryPath),
                OutputText = outputText,
                ErrorText = errorText
            };
        }

    }
}
