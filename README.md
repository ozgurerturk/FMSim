# FMSim

FMSim is a Football Manager-style match simulation project built around a C++ match engine and a C# WPF tooling frontend.

The current focus is to develop a deterministic and inspectable football simulation engine, then expose it through a clean API layer for tools, visualizers, and future game frontends.

## Current Status

FMSim currently includes:

- C++ match simulation engine
- Team loading from JSON files
- Match event generation
- Commentary and event log output
- C# WPF tooling app
- Team editor
- Simulation setup screen
- Live match viewer using stdout JSON events

At the moment, the WPF app runs the C++ simulator executable as a process, sends input through stdin, and reads live match events from stdout.

## Architecture Direction

The long-term goal is to keep the match engine independent from any specific UI.

```text
C++ Match Engine -> API / Interop Layer -> C# WPF Tools -> Future Game Frontend
```

WPF is currently used as a tooling, debugging, and visualization interface. A future game frontend, such as Godot, may be added later once the engine API and event model are more stable.

## Project Structure

```text
FMSim/
    C++ match engine and simulator executable

FMSimTools/
    C# WPF tooling application

Teams/
    Sample and user-created team JSON files

Logs/
    Generated match logs, commentary, and simulation output
```

## Tech Stack

- C++20
- C# / .NET
- WPF
- JSON-based team data
- Visual Studio

## License

See [LICENSE.txt](LICENSE.txt).
