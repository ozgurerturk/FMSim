using System.Diagnostics;
using System.IO;
using System.Text.Json;
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

            TeamJsonStore.ValidateTeamFileForSimulation(homeTeam.FilePath);
            TeamJsonStore.ValidateTeamFileForSimulation(awayTeam.FilePath);

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
                    // Process has already exited.
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

        public static async Task<SimulationResult> RunLiveAsync(
            LiveSimulationRequest request,
            Func<LiveMatchEvent, Task> onEvent,
            CancellationToken cancellationToken = default)
        {
            var executablePath = AppPaths.SimulatorExecutablePath;

            if (!File.Exists(executablePath))
            {
                throw new FileNotFoundException("FMSim.exe could not be found.", executablePath);
            }

            TeamJsonStore.ValidateTeamFileForSimulation(request.HomeTeam.FilePath);
            TeamJsonStore.ValidateTeamFileForSimulation(request.AwayTeam.FilePath);

            var logsDirectory = AppPaths.LogsDirectory;
            _ = Directory.CreateDirectory(logsDirectory);

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
            process.StartInfo.Environment["FMSIM_HOME_JSON"] = request.HomeTeam.FilePath;
            process.StartInfo.Environment["FMSIM_AWAY_JSON"] = request.AwayTeam.FilePath;
            process.StartInfo.Environment["FMSIM_NON_INTERACTIVE"] = "1";
            process.StartInfo.Environment["FMSIM_SKIP_REPLAY"] = "1";
            process.StartInfo.Environment["FMSIM_LIVE_EVENTS"] = "1";

            process.Start();

            await process.StandardInput.WriteLineAsync(request.SimulationMinutes.ToString());
            await process.StandardInput.WriteLineAsync(request.SimulationCount.ToString());

            var errorTask = process.StandardError.ReadToEndAsync(cancellationToken);
            var outputLines = new List<string>();

            while (!process.StandardOutput.EndOfStream)
            {
                cancellationToken.ThrowIfCancellationRequested();

                var line = await process.StandardOutput.ReadLineAsync(cancellationToken);
                if (line is null)
                {
                    break;
                }

                outputLines.Add(line);

                const string livePrefix = "FMSIM_EVENT ";
                var livePrefixIndex = line.IndexOf(livePrefix, StringComparison.Ordinal);
                if (livePrefixIndex < 0)
                {
                    continue;
                }

                var liveEvent = JsonSerializer.Deserialize<LiveMatchEvent>(
                    line[(livePrefixIndex + livePrefix.Length)..],
                    new JsonSerializerOptions { PropertyNameCaseInsensitive = true });
                if (liveEvent is null)
                {
                    continue;
                }

                await onEvent(liveEvent);

                if (liveEvent.IsFullTime)
                {
                    continue;
                }

                await Task.Delay(TimeSpan.FromSeconds(1), cancellationToken);
                await process.StandardInput.WriteLineAsync("next");
            }

            await process.WaitForExitAsync(cancellationToken);
            var errorText = await errorTask;
            var outputText = string.Join(Environment.NewLine, outputLines);

            if (process.ExitCode != 0)
            {
                var details = string.IsNullOrWhiteSpace(errorText) ? outputText : errorText;
                await File.WriteAllTextAsync(
                    Path.Combine(logsDirectory, "simulation_error.txt"),
                    details,
                    cancellationToken);
                throw new InvalidOperationException($"Simulation failed with exit code {process.ExitCode}: {details}");
            }

            var commentaryPath = FindLatestLogFile(logsDirectory, $"match_commentary_{request.SimulationMinutes}*.txt", startedAt);
            var eventLogPath = FindLatestLogFile(logsDirectory, $"eventlog_{request.SimulationMinutes}*.txt", startedAt);

            return new SimulationResult
            {
                CommentaryPath = commentaryPath,
                CommentaryText = string.IsNullOrWhiteSpace(commentaryPath) ? string.Empty : await File.ReadAllTextAsync(commentaryPath, cancellationToken),
                EventLogPath = eventLogPath,
                EventLogText = string.IsNullOrWhiteSpace(eventLogPath) ? string.Empty : await File.ReadAllTextAsync(eventLogPath, cancellationToken),
                OutputText = outputText,
                ErrorText = errorText
            };
        }

        private static string FindLatestLogFile(string logsDirectory, string searchPattern, DateTime startedAt)
        {
            var path = Directory
                .EnumerateFiles(logsDirectory, searchPattern)
                .Select(path => new FileInfo(path))
                .Where(file => file.LastWriteTime >= startedAt.AddSeconds(-2))
                .OrderByDescending(file => file.LastWriteTime)
                .FirstOrDefault()?.FullName;

            if (path is not null)
            {
                return path;
            }

            return Directory
                .EnumerateFiles(logsDirectory, searchPattern.Replace("*", "*"))
                .Select(path => new FileInfo(path))
                .OrderByDescending(file => file.LastWriteTime)
                .FirstOrDefault()?.FullName ?? string.Empty;
        }
    }
}
