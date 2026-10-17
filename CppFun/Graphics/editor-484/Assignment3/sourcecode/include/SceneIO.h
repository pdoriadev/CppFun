#ifndef SCENEIO_H
#define SCENEIO_H

#include <string>
#include <vector>

class ShapeManager;
class Camera;
struct RayTraceSettings;
struct AnimationTimeline;

// Scene save / load.
//
// The file is plain versioned text rather than JSON, deliberately:
//   - no new dependency on either toolchain (glm already showed what that costs)
//   - a student can open a .scene and read the format straight off
//   - it diffs cleanly in git
//
// Shape:  a flat list of [shape] sections, each a set of key=value lines.
// Unknown keys are ignored and unknown shape types are skipped with a warning,
// so a file written by a newer build still loads what it can.
namespace SceneIO {

// Bumped when the format changes incompatibly.
extern const int kFormatVersion;

struct Result {
    bool ok;
    int  shapesLoaded;
    int  shapesSkipped;
    std::vector<std::string> warnings;   // missing meshes, unknown types...
    std::string error;                   // set only when ok == false

    Result() : ok(false), shapesLoaded(0), shapesSkipped(0) {}
};

// Camera state is saved with the scene so a loaded scene frames itself the way
// it was composed.
// The optional RayTraceSettings round-trips a [render] section. Background in
// particular changes the look of a transparent scene enormously, so a scene
// that reopens without it does not look like the one that was saved. Passing
// null simply omits the section.
// The optional AnimationTimeline round-trips one [track] section per animated
// shape. Passing null omits them, exactly like render.
bool save(const std::string& path, ShapeManager& shapeManager, const Camera& camera,
          const RayTraceSettings* render = 0, const AnimationTimeline* timeline = 0);

// Replaces the current scene contents entirely.
Result load(const std::string& path, ShapeManager& shapeManager, Camera& camera,
            RayTraceSettings* render = 0, AnimationTimeline* timeline = 0);

// Directory the editor defaults to, created on demand. Comes from settings.
std::string defaultSceneFolder();
bool ensureFolderExists(const std::string& path);

// Paths are stored relative to the scene file when possible so a scenes/
// folder can be moved or shared without breaking mesh references.
std::string makeRelative(const std::string& path, const std::string& sceneFile);
std::string resolveRelative(const std::string& stored, const std::string& sceneFile);

} // namespace SceneIO

#endif // SCENEIO_H
