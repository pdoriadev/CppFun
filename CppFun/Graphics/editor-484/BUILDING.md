# Building the Editor

The project builds on Linux, macOS and Windows from the same Makefile.

## Dependencies

| Dependency | Vendored in repo? | Notes |
|---|---|---|
| Dear ImGui | Yes (`src/imgui`, `include/imgui`) | — |
| glad | Yes (`src/glad`, `include/glad`) | — |
| tinyfiledialogs | Yes (`src/tinydialog`, `include/tinydialog`) | — |
| **GLM** | **No** | Header-only, must be installed. The Makefile adds no include path for it, so it has to be somewhere the compiler already looks. |
| **zenity** (Linux) | No | **Runtime only.** `tinyfiledialogs` shells out to it; without it the File menu's dialogs silently never appear. |
| mesa-utils (Linux) | No | Optional. `glxinfo` / `glxgears` for diagnosing GL problems. |
| **GLFW** | **No** | Headers and link library required. |
| OpenGL | System | — |

GLM and GLFW are the only two you must install yourself. The project uses glm
throughout for vector and matrix maths; the old vendored vecmath library was dead
code and has been removed.

A missing GLM shows up as:

```
include/Shape.h:7:10: fatal error: glm/glm.hpp: No such file or directory
```

## Linux (Ubuntu/Debian) — the production target

```bash
sudo apt update
sudo apt install -y build-essential libglfw3-dev libglm-dev libgl1-mesa-dev \
                    zenity mesa-utils
make
./editor
```

## macOS

```bash
brew install glfw glm
make
./editor
```

If you do not have Homebrew, install it from <https://brew.sh> first. Xcode
Command Line Tools (`xcode-select --install`) provides the compiler.

Two things about macOS are worth knowing before they cost you an afternoon,
because both fail in ways that do not name the real cause.

### Homebrew's prefix depends on the chip

| Mac | Prefix | On the compiler's default search path? |
|---|---|---|
| Intel | `/usr/local` | **Yes** |
| Apple Silicon (M1 and later) | `/opt/homebrew` | **No** |

A Makefile that hardcodes `/usr/local` therefore works on an Intel Mac and fails
on every M-series one with:

```
fatal error: 'GLFW/glfw3.h' file not found
```

This Makefile asks `brew --prefix` instead of guessing, so it works on both. If
you hit that error anyway, run:

```bash
make whereis-glfw
```

and paste the output — it prints the detected prefix, the include and library
flags, and whether the compiler can actually find the header.

### The editor needs a forward-compatible context

Apple offers an OpenGL Core Profile **only** through the "forward compatible"
path. Requesting 3.3 Core without `GLFW_OPENGL_FORWARD_COMPAT` gets a null window
back, and the program exits with:

```
Failed to create GLFW window
```

`src/Application.cpp` sets the hint, so the editor is fine. It is documented here
because the same bug bit Assignments 0 and 1, and because it is the first thing
to check if you port any of this code elsewhere. On Windows and Linux the hint is
a harmless no-op.

### File dialogs

`tinyfiledialogs` uses AppleScript on macOS, so the File menu's dialogs work with
no extra install — the `zenity` note in the table above is Linux-only.

## Windows

### Important: Git Bash is not a build environment

Git for Windows ships a shell whose prompt reads `MINGW64`, which looks identical
to MSYS2's MINGW64 shell — but it contains **no compiler and no `make`**. If you
see this:

```
$ make
bash: make: command not found
```

you are in Git Bash. Confirm with:

```bash
which g++ make pacman     # Git Bash: all three missing
```

Git Bash is fine for git. Use MSYS2 to build.

### MSYS2 setup

1. Install MSYS2 from <https://www.msys2.org>.
2. Open **"MSYS2 MINGW64"** from the Start menu — the blue icon. Not "MSYS2 MSYS",
   not "MSYS2 UCRT64": the `mingw-w64-x86_64-*` packages below are only on the
   PATH in the MINGW64 shell.
3. Update, then install the toolchain and libraries:

```bash
pacman -Syu                 # may ask you to close and reopen the shell, then run again
pacman -S --needed \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-glfw \
    mingw-w64-x86_64-glm \
    make
```

4. Build:

```bash
cd /e/path/to/Editor        # your clone; /e is E:\ in MSYS2
make
./editor.exe
```

The Makefile's `Windows_NT` branch links `-lgdi32 -lopengl32 -lglfw3`, which is
what the MSYS2 GLFW package provides.

### WSL alternative

If you would rather build against the same environment as production, WSL works:

```bash
sudo apt install -y build-essential libglfw3-dev libglm-dev libgl1-mesa-dev \
                    zenity mesa-utils
cd /mnt/e/path/to/Editor    # your clone; /mnt/e is E:\ under WSL
make
```

The GUI needs WSLg (Windows 11) to display. The headless tests below need no
display at all and run fine either way.

One caveat on any Windows drive (`/mnt/c`, `/mnt/e`, …): WSL reports every file
on a DrvFs mount as mode `0777`, so git inside WSL records `100755` while Git
for Windows records `100644` — spurious mode changes on files you never touched.

Set this once per clone and you can use git from either side:

```bash
git config core.filemode false
```

It is a **per-repository** setting and does not survive a fresh clone. (This
supersedes the older advice here to build in WSL but commit from Git Bash — that
was a workaround; this fixes the cause.)

## Tests

Most of the tests are headless. Build the objects first, then run the whole
suite:

```bash
make                                    # build the objects first
make -f tests/Makefile.test check       # runs every assertion suite
```

`check` runs every suite in `tests/` that reports pass/fail, then exits non-zero
if any of them failed. Which suites are present depends on your course; in a
484 tree it is:

| Suite | Covers | Assignment |
|---|---|---|
| `transform_test` | translate / rotate / scale and the matrix order | A2 |
| `geometry_test` | the generated shapes | A2 |
| `obj_parser_test` | the `.obj` reader | A3 |
| `camera_test` | orbit / pan / zoom invariants | A3 |
| `keyframe_test` | key storage, easing, interpolation | A4 |
| `texture_test` | UV generation and bilinear sampling | A5 |
| `lighting_test` | Phong, against the GLSL reference | A5 |
| `bvh_test` | slab test, Möller–Trumbore, the three traversals | A6 |
| `raytrace_test` | the shading model, shadows, recursion | A6 |
| `scene_test` | scene file save/load round trip | given |
| `render_section_test` | the `[render]` scene-file section | given |
| `dirty_state_test` | float round-trip and the unsaved-changes flag | given |
| `specular_through_glass` | *measurement, not assertions* — prints highlight brightness through a transparent slab | given |

**The suites for assignments you have not done yet are supposed to fail.** That
is the assignment: each one goes green as you implement its topic. The three
marked "given" cover code you were handed and should pass from day one — if one
of those fails, something is wrong with your build, not with your work.

### Run them with a display

Three suites need a real OpenGL context and quietly do less — or nothing —
without one. **`geometry_test` runs zero checks headless**, so an A2 student on a
machine with no display gets no grading at all on half the assignment:

| Suite | No display | With a display |
|---|---|---|
| `geometry_test` | **0 checks** | 58 |
| `texture_test` | 26 | 67 |
| `keyframe_test` | 110 | 147 |
| everything else | same | same |

If you have a desktop, just run `make -f tests/Makefile.test check` normally.
Over SSH, or on WSL without WSLg, install `xvfb` and wrap the command:

```bash
sudo apt install -y xvfb
xvfb-run -a make -f tests/Makefile.test check
```

Grading yourself on the headless numbers is grading yourself on a fraction of
the suite.

The renderers are excluded from `check` — they take minutes and are judged by
eye. Build and run them individually:

```bash
make -f tests/Makefile.test ice_scene && ./ice_scene    # writes ice_scene.ppm
```

Under WSL, `libEGL warning:` and `MESA: error: ZINK: failed to choose pdev` on
the test output are normal — Mesa probing for a GPU and falling back to
software. The suites still pass; they are noise, not failures.

## Notes

- `obj/` is gitignored; the Makefile creates it automatically. A fresh clone
  builds without any manual setup.
- `make clean` removes the binary and object files; `make distclean` also
  removes the test binaries and their `.ppm` output. Run `distclean` before
  staging a commit.
- `make check-sources` lists any `src/*.cpp` missing from the Makefile's
  `SOURCES`. A file that is not in `SOURCES` is never compiled, which is a
  confusing way to lose an afternoon.
