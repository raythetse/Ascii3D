# Ascii3D

A real-time ray-marcher that renders 3D signed-distance-field (SDF) scenes as colored ASCII in the Windows console. Written in C++20, it has two renderers: a CPU ray-marcher, and a Vulkan compute renderer that generates a GLSL shader from the scene and compiles it at load time.

It's a personal project for practicing modern C++ features (see [AI assistance](#ai-assistance)).

<!-- TODO: demo video. Edit this file in GitHub's web editor and drag a short (10-20 s) .mp4
     onto this spot; GitHub uploads it and inserts a link that plays inline. Optionally add a
     link to a longer walkthrough on YouTube below it. -->

> **TODO (demo video):** Suggested built-in scenes to record, on a GPU device:
> - **[2] Combination**: add, subtract and intersect, with the smooth versions in the back row.
> - **[5] Worms**: partial tori combined into continuous moving worms (animation plus combinations).
> - **[7] Jigsaw**: domain repetition.
> - **[9] Color Blend**: smooth color blending between shapes.
>
> A short clip of `Loader_Warnings_Demo.scn` showing the warnings screen would also show off the custom-scene tooling.

## Features

- **Two renderers.** The CPU renderer marches rays on the CPU. The Vulkan renderer turns the scene tree into a GLSL compute shader (`GLSLMarcher`), compiles it to SPIR-V with shaderc at run time, and runs it on any Vulkan device you pick from the startup menu.
- **SDF modeling.** 3D shapes (box, box frame, sphere, ellipsoid, cone, torus, capsule, cylinder and more), 2D shapes that can be extruded or revolved (triangles, polygons, star, pie slice), and combinations: add, subtract, intersect and exclude, each with a smooth version that blends shape and color.
- **Operations.** Translate, rotate, elongate, repetition (grid and circular), mirroring, and surface operations (round-off, onion shells, inside-out).
- **Animation.** Continuous, smooth (sine) and linear cycles that bind to shape, operation and combination parameters. Binding is type-checked: an animation of the wrong dimension, or on a property that can't be animated, is rejected with a warning.
- **Lighting.** Point lights with inverse-square falloff, ambient light, and shadows.
- **Color output.** 24-bit ANSI color escapes, quantized through a precomputed 15-bit color table. The viewport follows the console window and font size.
- **Free camera.** Fly around any scene with WASD and the mouse.
- **Custom scenes.** Scenes are plain-text `.scn` files. A built-in validator checks them, from the command line or before a scene loads, and the current scene can be saved back to a file.
- **Performance readout.** Per-frame update, march and render timings are drawn in the corner.

## Requirements

- Windows (the console, input and window handling use the Win32 API).
- Visual Studio 2026 (version 18) with the **Desktop development with C++** workload (toolset v145).
- The [Vulkan SDK](https://vulkan.lunarg.com/), with the `VULKAN_SDK` environment variable set (the SDK installer sets it). The project uses `$(VULKAN_SDK)\Include` and `$(VULKAN_SDK)\Lib`, and links `vulkan-1.lib` and `shaderc_shared.lib`.
- For the Vulkan renderer, a GPU with a Vulkan driver. Without one, the CPU renderer still works.

## Building

Open `Ascii3D.slnx` in Visual Studio, select the **x64** platform (Debug or Release), and build. Only the x64 configurations are set up with the Vulkan SDK paths.

The solution has two projects: `Ascii3D` (the app) and `Tests/Ascii3DTests` (unit tests, see [Tests](#tests)).

<!-- TODO: CMake build, if added later. -->

## Running

Run from Visual Studio (F5), or run the built executable from the repository root:

```bat
x64\Release\Ascii3D.exe
```

Custom scenes are read from `.\CustomScenes`, relative to the working directory, so start the app from the repository root. `shaderc_shared.dll` is loaded from the Vulkan SDK, so the SDK's `Bin` folder must be on `PATH` (the SDK installer adds it).

The app walks you through three menus:

1. **Rendering device.** `[1] CPU (Slow mode)`, then one entry for each Vulkan device found, then *Controls and Information* and *Exit*. If the chosen Vulkan device can't be set up, the app falls back to the CPU renderer.
2. **Scene type.** *Built-In* (scenes hard-coded in C++) or *Custom* (`.scn` files in `CustomScenes/`). Custom scenes can run very slowly on the CPU renderer.
3. **Scene.** The built-in demo scenes are:

   | # | Scene | Shows |
   |---|---|---|
   | 1 | Shapes | A collection of ray-marched shapes |
   | 2 | Combination | Add, subtract, intersect; smoothing in the back row |
   | 3 | Walking Cubes | Rolling cubes |
   | 4 | Exclusion | Removing overlapping space, coloring interior surfaces |
   | 5 | Worms | Partial tori combined into continuous moving worms |
   | 6 | Rainbow | Colors cycling across several objects |
   | 7 | Jigsaw | Domain repetition tricks |
   | 8 | Revolution | An animated star-shaped ring |
   | 9 | Color Blend | Blending colors in different ways |
   | 10 | Elongation | A sphere elongated along three axes |
   | 11 | Smooth Exclude | Spheres that avoid direct contact |

### Controls

| Key | Action |
|---|---|
| `Esc` | Close the scene (back to the menus) |
| `W` `A` `S` `D` | Move forward / left / back / right |
| `Space` / `Left Shift` | Move up / down |
| Left-click, then move the mouse | Look around (the mouse is released when the window loses focus) |
| `Up` / `Down` | Increase / decrease movement speed |
| `Backspace` | Reset the camera position |
| `P` | Pause / unpause animations |
| `R` | Restart animations |
| `Ctrl` + `M` | Save the current scene to `CustomScenes/CustomScene.scn` (overwrites it) |
| `Ctrl` + `-` / `+` | Decrease / increase the console font size (smaller fonts mean more pixels and lower frame rates) |
| `Ctrl` + `0` | Reset the font size |
| `Alt` + `Enter` | Toggle fullscreen (also affects performance) |

## Custom scenes

A `.scn` file describes the scene as a stack: shapes are added with combinations, operations apply to the shapes added after them, `stepBack` pops a level, and light sources come last. A small example, a box with a sphere on top:

```
SDFObject
	COMBAdd
	SDFBox
		OPTranslate translation 0 1 0
		COMBAdd
		SDFSphere
LightSource
```

Indentation is optional but makes the stack depth easier to follow. Keywords are case sensitive, and lines that don't start with a keyword are ignored, so `//` comments work.

- [`HOWTO.txt`](HOWTO.txt) is a step-by-step tutorial and a reference for every shape, operation, animation and light parameter.
- [`ClaudeNotes/scene_format_spec_v0.4.md`](ClaudeNotes/scene_format_spec_v0.4.md) is the stricter format spec, including the numbered validator rules (section 11).
- [`CustomScenes/`](CustomScenes) has example scenes, among them `Jack_O_Lantern.scn`, `Robot_Arm.scn` and `Bort_Sampson.scn`.

### Validating scenes

Check one or more files from the command line without opening the renderer:

```bat
x64\Release\Ascii3D.exe --validate CustomScenes\Robot_Arm.scn CustomScenes\Loader_Warnings_Demo.scn
```

Each finding is printed as `path:line: error|warning: [rule N] message`. The exit code is `0` when no file has an error, `1` when any file has an error or can't be read, and `2` when no files are given.

### The warnings screen

The loader recovers from malformed lines by skipping them or using defaults. When you pick a custom scene that has problems, the app lists them, with line numbers and rule numbers from the spec, before it renders anything. Press `Enter` to render anyway or `B` to go back to the scene list.

[`CustomScenes/Loader_Warnings_Demo.scn`](CustomScenes/Loader_Warnings_Demo.scn) is malformed on purpose to show this screen.

## Tests

`Tests/Ascii3DTests.vcxproj` is a unit test project using the Microsoft Native Unit Test Framework (CppUnitTest). It compiles the app's sources (everything except `Ascii3D.cpp`) and tests:

- the scene validator, against a set of fixture files in `Tests/fixtures/`, with checks that every rule has a fixture and that CRLF line endings give the same results;
- the scene loader, checking that the example scenes load without problems and that `Loader_Warnings_Demo.scn` reports its problems.

Run the tests from **Test Explorer** in Visual Studio, or build the test project and run, from a Developer Command Prompt at the repository root:

```bat
vstest.console.exe Tests\x64\Debug\Ascii3DTests.dll /Platform:x64
```

## Project layout

All app sources are at the repository root, one class per `.h`/`.cpp` pair.

| Files | Purpose |
|---|---|
| `Ascii3D.cpp` | Entry point; console setup and the `--validate` mode |
| `GameEngine.*` | Menus, main loop, input handling, CPU/GPU dispatch |
| `RayMarcher.*` | CPU ray-marcher |
| `VulkanManager.*`, `GLSLMarcher.*` | Vulkan device setup and compute dispatch; GLSL shader generation from the scene |
| `Renderer.*`, `PixelField.*`, `PixelSample.*` | Converting marched samples to colored console output |
| `SDFObject.*`, `SDFAxisObject.*`, `SDF2DObject.*` | Shapes |
| `SDFOperation.*`, `CombinationOperation.*`, `TransformOperation.*`, `DomainOperation.*`, `ResurfaceOperation.*` | Combinations and operations |
| `Animation.*`, `GameTimer.*` | Animations and scene time |
| `SDFScene.*`, `SDFBuilder.*`, `FileHandler.h` | Loading and saving `.scn` files |
| `SceneValidator.*` | The `.scn` validator |
| `*Scene.*` (e.g. `WormsScene.*`) | The built-in demo scenes |
| `Camera.*`, `InputManager.*`, `ConsoleFunctions.h` | Camera, keyboard and mouse input, console control |
| `Vec2.*`, `Vec3.*`, `Mat2.*`, `Mat3.*`, `MathFunctions.h` | Math |
| `PerfMonitor.*`, `Stopwatch.*` | Timing readout |
| `CustomScenes/` | Example `.scn` scenes |
| `Tests/` | Unit tests and fixtures |
| `ClaudeNotes/` | Design notes and the scene format spec |
| `HOWTO.txt` | Custom scene tutorial and reference |

## AI assistance

Ascii3D is a project for practicing modern C++ (C++20). Learning Vulkan was not a goal, so
the initial Vulkan setup and a starting GLSL compute shader came from Google Gemini.

The rest of the engine was written by hand: the CPU ray-marcher, the SDF shapes and
operations, generating the GLSL shader from the scene, animation, the `.scn` loader and
the built-in scenes.

Later, I deliberately brought in Claude Code (Anthropic) to get real experience working
with an AI coding agent on an existing codebase. I set the scope and reviewed every change
before merging. That work covers the loader's warnings and recovery, type-checked animation
binding, the scene validator and `--validate`, the unit tests, the format spec in
`ClaudeNotes/`, and this README. Those commits carry a `Co-Authored-By` trailer.

## Author

Ray Young · [GitHub](https://github.com/raythetse) · [YouTube](https://www.youtube.com/@raythetse)

## License

MIT, see [LICENSE](LICENSE).
