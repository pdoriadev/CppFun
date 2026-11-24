#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

// Persistent application settings.
//
// Everything here is applied BEFORE glfwInit() and before the GL context
// exists, because that is the only point at which these choices can be made:
// the windowing platform is fixed by a GLFW init hint, and Mesa reads
// LIBGL_ALWAYS_SOFTWARE when the context is created. Changing them therefore
// requires a restart, which the Options UI states plainly.
struct AppSettings {
    enum PlatformPreference {
        Platform_Auto    = 0,   // let the app choose (X11 when running under WSL)
        Platform_X11     = 1,
        Platform_Wayland = 2
    };

    int  platform;
    bool softwareRendering;
    bool autoEnterVR;

    // Shown in the window title. Blank until the user fills it in.
    std::string studentName;

    // Label used for the Custom shape, in the Insert menu and the shape list.
    std::string customShapeName;

    // Where Save/Open start. Relative paths are resolved next to the executable.
    std::string sceneFolder;

    // Most-recently-opened scenes, newest first, capped at kMaxRecent.
    std::vector<std::string> recentScenes;

    AppSettings()
        : platform(Platform_Auto),
          softwareRendering(false),
          autoEnterVR(false),
          customShapeName("Custom Shape"),
          sceneFolder("scenes") {}
};

extern AppSettings g_settings;

namespace SettingsIO {

// Cap on the Open Recent list.
extern const size_t kMaxRecent;

// Moves `path` to the front, de-duplicating, and trims to kMaxRecent.
void addRecentScene(AppSettings& settings, const std::string& path);

// Simple key=value file kept next to the executable.
const char* defaultPath();

bool load(AppSettings& settings, const std::string& path);
bool save(const AppSettings& settings, const std::string& path);

// Applies the settings. MUST be called before glfwInit().
void applyStartupSettings(const AppSettings& settings);

// True when running inside WSL, which is where the Wayland resize bug lives.
bool isRunningUnderWSL();

// Human-readable name of the platform GLFW actually selected. Returns
// "unknown" on GLFW builds older than 3.4, which have no query for it.
const char* activePlatformName();

// True if this build of GLFW can offer the given preference.
bool platformSupported(int platformPreference);

} // namespace SettingsIO

#endif // SETTINGS_H
