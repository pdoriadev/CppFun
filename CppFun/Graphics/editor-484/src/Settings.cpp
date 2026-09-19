#include "Settings.h"

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

AppSettings g_settings;

namespace {

void setEnvVar(const char* name, const char* value) {
#ifdef _WIN32
    _putenv_s(name, value);
#else
    setenv(name, value, 1);
#endif
}

std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

} // namespace

namespace SettingsIO {

const size_t kMaxRecent = 10;

void addRecentScene(AppSettings& settings, const std::string& path) {
    if (path.empty()) return;
    for (size_t i = 0; i < settings.recentScenes.size(); ++i) {
        if (settings.recentScenes[i] == path) {
            settings.recentScenes.erase(settings.recentScenes.begin() + i);
            break;
        }
    }
    settings.recentScenes.insert(settings.recentScenes.begin(), path);
    if (settings.recentScenes.size() > kMaxRecent) {
        settings.recentScenes.resize(kMaxRecent);
    }
}

const char* defaultPath() {
    return "editor.cfg";
}

bool isRunningUnderWSL() {
    if (std::getenv("WSL_DISTRO_NAME") != 0) return true;
    if (std::getenv("WSL_INTEROP") != 0) return true;

    // Fallback: the kernel version string carries "microsoft" under WSL.
    std::ifstream version("/proc/version");
    if (version) {
        std::string line;
        std::getline(version, line);
        for (size_t i = 0; i + 8 < line.size(); ++i) {
            if (line.compare(i, 9, "microsoft") == 0 || line.compare(i, 9, "Microsoft") == 0) {
                return true;
            }
        }
    }
    return false;
}

bool platformSupported(int platformPreference) {
    if (platformPreference == AppSettings::Platform_Auto) return true;

#if defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11) && defined(GLFW_PLATFORM_WAYLAND)
    if (platformPreference == AppSettings::Platform_X11) {
        return glfwPlatformSupported(GLFW_PLATFORM_X11) == GLFW_TRUE;
    }
    if (platformPreference == AppSettings::Platform_Wayland) {
        return glfwPlatformSupported(GLFW_PLATFORM_WAYLAND) == GLFW_TRUE;
    }
#else
    (void)platformPreference;   // GLFW < 3.4 has no platform selection at all
#endif
    return false;
}

const char* activePlatformName() {
#if defined(GLFW_PLATFORM)
    switch (glfwGetPlatform()) {
        case GLFW_PLATFORM_WIN32:   return "Win32";
        case GLFW_PLATFORM_COCOA:   return "Cocoa";
        case GLFW_PLATFORM_X11:     return "X11";
        case GLFW_PLATFORM_WAYLAND: return "Wayland";
        case GLFW_PLATFORM_NULL:    return "Null";
        default:                    return "unknown";
    }
#else
    return "unknown (GLFW < 3.4)";
#endif
}

void applyStartupSettings(const AppSettings& settings) {

    if (settings.softwareRendering) {
        // Mesa-only; harmless elsewhere. Forces llvmpipe instead of the
        // hardware driver, which is useful for isolating driver bugs.
        setEnvVar("LIBGL_ALWAYS_SOFTWARE", "1");
    }

    // Decide whether we want X11, independently of the GLFW version, because
    // the fallback below works even on builds with no platform selection.
    const bool wantX11 =
        (settings.platform == AppSettings::Platform_X11) ||
        (settings.platform == AppSettings::Platform_Auto && isRunningUnderWSL());

    if (wantX11) {
        // Clearing WAYLAND_DISPLAY makes GLFW fall back to X11/XWayland. This
        // is the crude lever, but it is the one that works on GLFW builds
        // compiled for a single backend, where the init hint below does not
        // exist. On GLFW 3.4+ both mechanisms agree.
        setEnvVar("WAYLAND_DISPLAY", "");
    }

#if defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11) && defined(GLFW_PLATFORM_WAYLAND)
    int wanted = 0;

    if (settings.platform == AppSettings::Platform_X11) {
        wanted = GLFW_PLATFORM_X11;
    } else if (settings.platform == AppSettings::Platform_Wayland) {
        wanted = GLFW_PLATFORM_WAYLAND;
    } else {
        // Automatic. Under WSL prefer X11: WSLg's Wayland compositor does not
        // resize the EGL surface when the window is maximized, so the app keeps
        // drawing into a pre-maximize buffer and everything outside it is left
        // uncleared. XWayland resizes server-side and is unaffected.
        if (isRunningUnderWSL() && glfwPlatformSupported(GLFW_PLATFORM_X11) == GLFW_TRUE) {
            wanted = GLFW_PLATFORM_X11;
        }
    }

    if (wanted != 0 && glfwPlatformSupported(wanted) == GLFW_TRUE) {
        glfwInitHint(GLFW_PLATFORM, wanted);
    }
#endif
}

bool load(AppSettings& settings, const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in) return false;

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key   = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));

        if (key == "recent_scene_reset") {
            settings.recentScenes.clear();
        } else if (key == "platform") {
            if (value == "x11")          settings.platform = AppSettings::Platform_X11;
            else if (value == "wayland") settings.platform = AppSettings::Platform_Wayland;
            else                         settings.platform = AppSettings::Platform_Auto;
        } else if (key == "software_rendering") {
            settings.softwareRendering = (value == "1" || value == "true");
        } else if (key == "auto_enter_vr") {
            settings.autoEnterVR = (value == "1" || value == "true");
        } else if (key == "student_name") {
            settings.studentName = value;
        } else if (key == "custom_shape_name") {
            if (!value.empty()) settings.customShapeName = value;
        } else if (key == "scene_folder") {
            if (!value.empty()) settings.sceneFolder = value;
        } else if (key == "recent_scene") {
            // One line per entry, in order, so the file stays line-based.
            if (!value.empty() && settings.recentScenes.size() < kMaxRecent) {
                settings.recentScenes.push_back(value);
            }
        }
    }
    return true;
}

bool save(const AppSettings& settings, const std::string& path) {
    std::ofstream out(path.c_str());
    if (!out) return false;

    const char* platformName = "auto";
    if (settings.platform == AppSettings::Platform_X11)     platformName = "x11";
    if (settings.platform == AppSettings::Platform_Wayland) platformName = "wayland";

    out << "# Editor settings. Applied at startup - restart to take effect.\n";
    out << "#\n";
    out << "# platform: auto | x11 | wayland\n";
    out << "#   Windowing backend. Linux only; ignored on Windows and macOS.\n";
    out << "#   'auto' picks X11 under WSL, where Wayland mishandles maximize.\n";
    out << "platform=" << platformName << "\n";
    out << "\n";
    out << "# software_rendering: 0 | 1\n";
    out << "#   Forces Mesa's llvmpipe software rasterizer (LIBGL_ALWAYS_SOFTWARE).\n";
    out << "software_rendering=" << (settings.softwareRendering ? 1 : 0) << "\n";
    out << "\n";

    out << "# auto_enter_vr: enter automatically only when a real device source is selected.\n";
    out << "auto_enter_vr=" << (settings.autoEnterVR ? 1 : 0) << "\n";
    out << "\n";

    out << "# student_name: shown in the window title.\n";
    out << "student_name=" << settings.studentName << "\n";
    out << "\n";

    out << "# custom_shape_name: label for the Custom shape in the Insert menu.\n";
    out << "custom_shape_name=" << settings.customShapeName << "\n";
    out << "\n";

    out << "# scene_folder: where Open/Save start. Created on first save.\n";
    out << "scene_folder=" << settings.sceneFolder << "\n";
    out << "\n";

    out << "# recent_scene: one line per entry, newest first.\n";
    for (size_t i = 0; i < settings.recentScenes.size(); ++i) {
        out << "recent_scene=" << settings.recentScenes[i] << "\n";
    }

    return out.good();
}

} // namespace SettingsIO
