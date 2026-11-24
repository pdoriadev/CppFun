#include "SceneIO.h"
#include "Keyframe.h"

#include "ShapeManager.h"
#include "Shape.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Settings.h"
#include "FileImporter.h"

#include "Cube.h"
#include "Sphere.h"
#include "Pyramid.h"
#include "Teapot.h"
#include "Icosahedron.h"
#include "Mobius.h"
#include "Torus.h"
#include "Custom.h"
#include "Light.h"
#include "SimpleSystem.h"
#include "SimplePendulum.h"
#include "SimpleChain.h"
#include "SimpleCloth.h"

#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #include <sys/types.h>
  #define MKDIR(p) mkdir(p, 0755)
#endif

namespace {

std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// Shortest decimal string that reads back as the identical float.
//
// %.6g is not enough. A float needs up to 9 significant digits to survive a
// text round trip, and 6 silently rounds: 1/3 writes as 0.333333 and reloads as
// a different float. Nothing looks broken until the editor compares the
// reloaded scene against what it wrote, finds a different content hash, and
// tells you there are unsaved changes in a file you just opened.
//
// Going straight to %.9g would fix that and make the file unreadable - 1.7f
// prints as 1.70000005. So try increasing precision and stop at the first one
// that round-trips: tidy values stay tidy, awkward ones get the digits they
// need.
std::string floatToString(float f) {
    char buf[64];
    for (int precision = 6; precision < 9; ++precision) {
        std::snprintf(buf, sizeof(buf), "%.*g", precision, f);
        if (static_cast<float>(std::atof(buf)) == f) return buf;
    }
    std::snprintf(buf, sizeof(buf), "%.9g", f);
    return buf;
}

std::string vec3ToString(float a, float b, float c) {
    return floatToString(a) + " " + floatToString(b) + " " + floatToString(c);
}

void parseVec3(const std::string& v, float& a, float& b, float& c) {
    std::istringstream in(v);
    in >> a >> b >> c;
}

// A parsed [section] plus its key=value pairs.
struct Section {
    std::string name;
    std::vector<std::pair<std::string, std::string> > kv;

    std::string get(const std::string& key, const std::string& fallback = "") const {
        for (size_t i = 0; i < kv.size(); ++i) {
            if (kv[i].first == key) return kv[i].second;
        }
        return fallback;
    }
    float getFloat(const std::string& key, float fallback) const {
        std::string v = get(key);
        return v.empty() ? fallback : static_cast<float>(std::atof(v.c_str()));
    }
    int getInt(const std::string& key, int fallback) const {
        std::string v = get(key);
        return v.empty() ? fallback : std::atoi(v.c_str());
    }
    bool has(const std::string& key) const { return !get(key).empty(); }

    // Repeated keys are legal and order-preserving: a [track] section carries
    // one key= line per keyframe. get() returns the first; this returns all.
    std::vector<std::string> getAll(const std::string& key) const {
        std::vector<std::string> v;
        for (size_t i = 0; i < kv.size(); ++i) {
            if (kv[i].first == key) v.push_back(kv[i].second);
        }
        return v;
    }
};

std::string directoryOf(const std::string& path) {
    size_t slash = path.find_last_of("/\\");
    return (slash == std::string::npos) ? std::string(".") : path.substr(0, slash);
}

} // namespace

namespace SceneIO {

const int kFormatVersion = 1;

std::string defaultSceneFolder() {
    return g_settings.sceneFolder.empty() ? std::string("scenes") : g_settings.sceneFolder;
}

bool ensureFolderExists(const std::string& path) {
    if (path.empty()) return false;
    std::ifstream probe((path + "/.probe").c_str());
    MKDIR(path.c_str());          // fails harmlessly if it already exists
    return true;
}

std::string makeRelative(const std::string& path, const std::string& sceneFile) {
    std::string dir = directoryOf(sceneFile);
    if (dir != "." && path.compare(0, dir.size(), dir) == 0) {
        std::string rest = path.substr(dir.size());
        if (!rest.empty() && (rest[0] == '/' || rest[0] == '\\')) rest = rest.substr(1);
        return rest;
    }
    return path;
}

std::string resolveRelative(const std::string& stored, const std::string& sceneFile) {
    if (stored.empty()) return stored;
    // Absolute paths pass through unchanged.
    if (stored[0] == '/' || stored[0] == '\\') return stored;
    if (stored.size() > 1 && stored[1] == ':') return stored;
    return directoryOf(sceneFile) + "/" + stored;
}

// ---------------------------------------------------------------- save ----

bool save(const std::string& path, ShapeManager& shapeManager, const Camera& camera,
          const RayTraceSettings* render, const AnimationTimeline* timeline) {
    std::ofstream out(path.c_str());
    if (!out) return false;

    out << "editor-scene " << kFormatVersion << "\n";
    out << "# Editor scene file. Plain text on purpose - safe to read and diff.\n\n";

    out << "[scene]\n";
    out << "shapeCounter=" << shapeManager.getShapeCounter() << "\n\n";

    out << "[camera]\n";
    out << "position=" << vec3ToString(camera.getPosition().x,
                                       camera.getPosition().y,
                                       camera.getPosition().z) << "\n";
    out << "target=" << vec3ToString(camera.getTarget().x,
                                     camera.getTarget().y,
                                     camera.getTarget().z) << "\n";
    out << "radius=" << floatToString(camera.getRadius()) << "\n";
    // position above is derived and written for readability only; these two are
    // what actually restore the view.
    out << "orbit=" << floatToString(camera.getTheta()) << " "
                    << floatToString(camera.getPhi()) << "\n\n";

    // Render settings. Not scene geometry, but a scene reopened with the
    // default dark background looks nothing like the one that was saved -
    // especially anything transparent, which is mostly whatever is behind it.
    if (render) {
        out << "[render]\n";
        out << "mode=" << render->mode << "\n";
        out << "background=" << vec3ToString(render->background.r,
                                             render->background.g,
                                             render->background.b) << "\n";
        out << "width=" << render->width << "\n";
        out << "height=" << render->height << "\n";
        out << "maxDepth=" << render->maxDepth << "\n";
        out << "samplesPerPixel=" << render->samplesPerPixel << "\n";
        out << "shadows=" << (render->shadows ? 1 : 0) << "\n";
        out << "reflections=" << (render->reflections ? 1 : 0) << "\n";
        out << "refractions=" << (render->refractions ? 1 : 0) << "\n";
        out << "transparentShadows=" << (render->transparentShadows ? 1 : 0) << "\n";
        out << "shadowBias=" << floatToString(render->shadowBias) << "\n\n";
    }

    // Keyframe tracks. One section per animated shape, written after [render]
    // and before the shapes so the file reads chronologically: settings, then
    // animation, then geometry. Load does not care about order.
    //
    // Each key= line is ten floats -- time, position xyz, rotation xyz
    // (degrees), scale xyz -- followed by one integer, the easing mode.
    // Repeated keys in one section are legal: Section::kv is an ordered vector,
    // not a map, precisely so this works.
    //
    // The easing field is written always and read OPTIONALLY, so a scene saved
    // before easing existed still loads, with every key linear. Appending
    // rather than inserting is what makes that possible -- the first ten fields
    // are in the same places they always were.
    if (timeline) {
        // Timeline settings live in their own section: they are global, while
        // [track] is per shape. Written even with no tracks, so an empty scene
        // still remembers a timeline you widened.
        out << "[timeline]\n";
        out << "length=" << floatToString(timeline->length) << "\n";
        out << "loop=" << (timeline->loopAll ? 1 : 0) << "\n\n";

        for (size_t t = 0; t < timeline->tracks.size(); ++t) {
            const AnimationTrack& tr = timeline->tracks[t];
            if (tr.keys.empty()) continue;      // nothing to restore

            out << "[track]\n";
            out << "shape=" << tr.shapeId << "\n";
            out << "loop=" << (tr.loop ? 1 : 0) << "\n";
            for (size_t k = 0; k < tr.keys.size(); ++k) {
                const Keyframe& kf = tr.keys[k];
                out << "key=" << floatToString(kf.time) << " "
                    << vec3ToString(kf.position.x, kf.position.y, kf.position.z) << " "
                    << vec3ToString(kf.rotation.x, kf.rotation.y, kf.rotation.z) << " "
                    << vec3ToString(kf.scale.x, kf.scale.y, kf.scale.z) << " "
                    << kf.easing << "\n";
            }
            out << "\n";
        }
    }

    std::vector<Shape*>& shapes = shapeManager.getShapes();
    for (size_t i = 0; i < shapes.size(); ++i) {
        Shape* s = shapes[i];
        if (!s) continue;

        out << "[shape]\n";
        out << "type=" << s->serialType() << "\n";
        out << "id=" << s->getId() << "\n";
        out << "name=" << s->getShapeType() << "\n";
        out << "position=" << vec3ToString(s->getX(), s->getY(), s->getZ()) << "\n";
        out << "rotation=" << vec3ToString(s->getAngleX(), s->getAngleY(), s->getAngleZ()) << "\n";
        out << "uniformScaling=" << (s->isUsingUniformScaling() ? 1 : 0) << "\n";
        out << "scale=" << floatToString(s->getScale()) << "\n";
        glm::vec3 nu = s->getNonUniformScale();
        out << "scaleXYZ=" << vec3ToString(nu.x, nu.y, nu.z) << "\n";
        out << "colorIndex=" << s->getColorIndex() << "\n";
        const float* cc = s->getCustomColor();
        out << "customColor=" << vec3ToString(cc[0], cc[1], cc[2]) << "\n";

        const Material& m = s->getMaterial();
        // Note floatToString, not operator<<: the stream's default precision is
        // 6 significant digits and would round these the same way.
        out << "material=" << floatToString(m.ambient)
            << " " << floatToString(m.diffuse)
            << " " << floatToString(m.specular)
            << " " << floatToString(m.shininess)
            << " " << floatToString(m.reflectivity)
            << " " << floatToString(m.transparency)
            << " " << floatToString(m.indexOfRefraction) << "\n";

        // Its own key, not an eighth field on material=: a path can contain
        // spaces, and the material line is parsed with operator>>. Stored
        // relative to the scene file for the same reason mesh paths are, so a
        // scenes/ folder can be moved or shared without breaking references.
        if (!m.texturePath.empty()) {
            out << "texture=" << makeRelative(m.texturePath, path) << "\n";
        }

        if (!s->getSourcePath().empty()) {
            out << "source=" << makeRelative(s->getSourcePath(), path) << "\n";
        }

        // Particle systems need their construction arguments back.
        if (PendulumSystem* ps = dynamic_cast<PendulumSystem*>(s)) {
            out << "length=" << floatToString(ps->getLength()) << "\n";
            out << "mass=" << floatToString(ps->getMass()) << "\n";
        }
        if (SimpleCloth* cloth = dynamic_cast<SimpleCloth*>(s)) {
            out << "clothSize=" << cloth->getClothSize() << "\n";
        }

        if (Light* L = dynamic_cast<Light*>(s)) {
            out << "lightType=" << L->getType() << "\n";
            out << "enabled=" << (L->isEnabled() ? 1 : 0) << "\n";
            out << "lightColor=" << vec3ToString(L->getColor().r, L->getColor().g,
                                                 L->getColor().b) << "\n";
            out << "intensity=" << floatToString(L->getIntensity()) << "\n";
            out << "scales=" << vec3ToString(L->getAmbientScale(), L->getDiffuseScale(),
                                             L->getSpecularScale()) << "\n";
            out << "direction=" << vec3ToString(L->getDirection().x, L->getDirection().y,
                                                L->getDirection().z) << "\n";
            out << "attenuation=" << vec3ToString(L->getConstantAttenuation(),
                                                  L->getLinearAttenuation(),
                                                  L->getQuadraticAttenuation()) << "\n";
        }

        out << "\n";
    }

    return out.good();
}

// ---------------------------------------------------------------- load ----

namespace {

Shape* constructShape(const Section& sec, ShapeManager& sm,
                      const std::string& sceneFile,
                      std::vector<std::string>& warnings) {
    const std::string type = sec.get("type");
    // NOT sec.getInt("id", sm.incrementShapeCounter()). C++ evaluates the
    // default argument unconditionally, so that version bumped the counter once
    // per shape even when every shape had a stored id - loading an N-shape scene
    // inflated the counter by N and made save -> load -> save non-idempotent.
    const int id = sec.has("id") ? sec.getInt("id", 0)
                                 : sm.incrementShapeCounter();

    float px = 0, py = 0, pz = 0;
    parseVec3(sec.get("position", "0 0 0"), px, py, pz);
    const float sc = sec.getFloat("scale", 1.0f);
    const int   ci = sec.getInt("colorIndex", 2);

    if (type == "Cube")        return new Cube(px, py, pz, sc, ci, id);
    if (type == "Sphere")      return new Sphere(px, py, pz, sc, ci, id);
    if (type == "Pyramid")     return new Pyramid(px, py, pz, sc, ci, id);
    if (type == "Teapot")      return new Teapot(px, py, pz, sc, ci, id);
    if (type == "Icosahedron") return new Icosahedron(px, py, pz, sc, ci, id);
    if (type == "Mobius")      return new Mobius(px, py, pz, sc, ci, id);
    if (type == "Torus")       return new Torus(px, py, pz, sc, ci, id);
    if (type == "Custom")      return new Custom(px, py, pz, sc, ci, id);
    if (type == "Light")       return new Light(px, py, pz, id);

    const float length = sec.getFloat("length", 2.0f);
    const float mass   = sec.getFloat("mass", 1.0f);

    if (type == "SimpleSystem")   return new SimpleSystem(px, py, pz, sc, ci, id);
    if (type == "SimplePendulum") return new SimplePendulum(px, py, pz, sc, ci, id, length, mass);
    if (type == "SimpleChain")    return new SimpleChain(px, py, pz, sc, ci, id, length, mass);
    if (type == "SimpleCloth")
        return new SimpleCloth(px, py, pz, sc, ci, id, length, mass, sec.getInt("clothSize", 15));

    // File-backed shapes are re-imported rather than stored inline: embedding
    // bunny.obj would put ~5 MB in the scene file.
    if (type == "ImportShape" || type == "ImportCurve" || type == "ImportCharacter") {
        std::string src = resolveRelative(sec.get("source"), sceneFile);
        if (src.empty()) {
            warnings.push_back(type + " has no source path - skipped");
            return 0;
        }
        std::ifstream probe(src.c_str());
        if (!probe) {
            warnings.push_back("missing file, skipped: " + src);
            return 0;
        }
        probe.close();

        FileImporter importer;
        importer.setPendingPath(src);
        int okImport = 0;
        if (type == "ImportShape")     okImport = importer.importObjFile(sm);
        else if (type == "ImportCurve")     okImport = importer.importSwpFile(sm);
        else                                okImport = importer.importCharacterFile(sm);

        if (!okImport) {
            warnings.push_back("failed to import: " + src);
            return 0;
        }
        // The importer already added and selected it; hand it back so the
        // caller can apply the saved transform and material.
        return sm.getSelectedShape();
    }

    warnings.push_back("unknown shape type, skipped: " + type);
    return 0;
}

void applyCommon(Shape* s, const Section& sec, const std::string& scenePath,
                 std::vector<std::string>& warnings) {
    if (!s) return;

    float rx = 0, ry = 0, rz = 0;
    parseVec3(sec.get("rotation", "0 0 0"), rx, ry, rz);
    s->setRotation(rx, ry, rz);

    float px = 0, py = 0, pz = 0;
    parseVec3(sec.get("position", "0 0 0"), px, py, pz);
    s->setPosition(px, py, pz);

    if (sec.getInt("uniformScaling", 1)) {
        s->setScale(sec.getFloat("scale", 1.0f));
    } else {
        float sx = 1, sy = 1, sz = 1;
        parseVec3(sec.get("scaleXYZ", "1 1 1"), sx, sy, sz);
        s->setScale(sx, sy, sz);
    }

    s->setColor(sec.getInt("colorIndex", 2));
    float cr = 0, cg = 0, cb = 0;
    parseVec3(sec.get("customColor", "0 0 0"), cr, cg, cb);
    s->setCustomColor(cr, cg, cb);

    if (!sec.get("name").empty()) s->setShapeType(sec.get("name"));

    if (sec.has("material")) {
        std::istringstream in(sec.get("material"));
        Material m;
        in >> m.ambient >> m.diffuse >> m.specular >> m.shininess
           >> m.reflectivity >> m.transparency >> m.indexOfRefraction;
        s->setMaterial(m);
    }

    // After setMaterial, deliberately: applyCommon builds a fresh Material,
    // so setting the path first would be overwritten by the line above.
    if (sec.has("texture")) {
        const std::string resolved = resolveRelative(sec.get("texture"), scenePath);
        s->getMaterial().texturePath = resolved;

        // Say so if it is not there. Without this a moved scene file loads
        // reporting no problems at all and then renders every textured
        // surface flat grey, because TextureCache::get() fails quietly on the
        // first sample and the shader's hasTexture gate simply stays off.
        // "Loaded fine, looks wrong" is the hardest kind of bug to be handed;
        // the mesh path a few lines up has warned about this since day one and
        // the texture path should too.
        if (!resolved.empty()) {
            std::ifstream probe(resolved.c_str());
            if (!probe) {
                warnings.push_back("missing texture, surface will be untextured: "
                                   + resolved);
            }
        }
    }

    if (Light* L = dynamic_cast<Light*>(s)) {
        L->setType(sec.getInt("lightType", Light::Point));
        L->setEnabled(sec.getInt("enabled", 1) != 0);

        float r = 1, g = 1, b = 1;
        parseVec3(sec.get("lightColor", "1 1 1"), r, g, b);
        L->setColor(glm::vec3(r, g, b));

        L->setIntensity(sec.getFloat("intensity", 1.0f));

        float a = 0.15f, d = 1.0f, sp = 0.8f;
        parseVec3(sec.get("scales", "0.15 1 0.8"), a, d, sp);
        L->setAmbientScale(a); L->setDiffuseScale(d); L->setSpecularScale(sp);

        float dx = 0, dy = -1, dz = 0;
        parseVec3(sec.get("direction", "0 -1 0"), dx, dy, dz);
        L->setDirection(glm::vec3(dx, dy, dz));

        float c = 1, l = 0, q = 0;
        parseVec3(sec.get("attenuation", "1 0 0"), c, l, q);
        L->setConstantAttenuation(c);
        L->setLinearAttenuation(l);
        L->setQuadraticAttenuation(q);
    }
}

} // namespace

Result load(const std::string& path, ShapeManager& shapeManager, Camera& camera,
            RayTraceSettings* render, AnimationTimeline* timeline) {
    Result result;

    std::ifstream in(path.c_str());
    if (!in) {
        result.error = "Could not open " + path;
        return result;
    }

    std::string header;
    std::getline(in, header);
    if (header.compare(0, 13, "editor-scene ") != 0) {
        result.error = "Not an editor scene file: " + path;
        return result;
    }
    int version = std::atoi(header.substr(13).c_str());
    if (version > kFormatVersion) {
        result.warnings.push_back("File was written by a newer version; "
                                  "unrecognized entries will be ignored.");
    }

    std::vector<Section> sections;
    Section current;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[') {
            if (!current.name.empty()) sections.push_back(current);
            current = Section();
            current.name = line.substr(1, line.find(']') - 1);
            continue;
        }
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        current.kv.push_back(std::make_pair(trim(line.substr(0, eq)),
                                            trim(line.substr(eq + 1))));
    }
    if (!current.name.empty()) sections.push_back(current);

    // Only clear the existing scene once the file has parsed, so a bad file
    // does not destroy what is open.
    std::vector<Shape*>& existing = shapeManager.getShapes();
    for (size_t i = 0; i < existing.size(); ++i) delete existing[i];
    existing.clear();
    shapeManager.setSelectedShape(0);
    shapeManager.setShapeCounter(0);
    // Tracks reference shapes by id, so they have to go with the shapes. A
    // loaded scene that kept the previous scene's tracks would animate shapes
    // that happened to reuse the same ids.
    if (timeline) timeline->clear();

    for (size_t i = 0; i < sections.size(); ++i) {
        const Section& sec = sections[i];

        if (sec.name == "scene") {
            shapeManager.setShapeCounter(sec.getInt("shapeCounter", 0));
            continue;
        }
        if (sec.name == "camera") {
            float tx = 0, ty = 0, tz = 0;
            parseVec3(sec.get("target", "0 0 0"), tx, ty, tz);
            const float radius = sec.getFloat("radius", 5.0f);

            if (sec.has("orbit")) {
                std::istringstream in2(sec.get("orbit"));
                float th = 0.0f, ph = 0.0f;
                in2 >> th >> ph;
                camera.setView(th, ph, radius, glm::vec3(tx, ty, tz));
            } else {
                // Version 1 files written before orbit was stored. Target and
                // distance come back; the angle falls back to whatever is current.
                camera.setTarget(glm::vec3(tx, ty, tz));
                camera.setRadius(radius);
            }
            continue;
        }
        if (sec.name == "render") {
            // Only touch what the file actually carries: a scene written by an
            // older build has no [render] at all, and one written by a newer
            // build may carry keys this version does not know.
            if (!render) continue;
            render->mode            = sec.getInt("mode", render->mode);
            render->width           = sec.getInt("width", render->width);
            render->height          = sec.getInt("height", render->height);
            render->maxDepth        = sec.getInt("maxDepth", render->maxDepth);
            render->samplesPerPixel = sec.getInt("samplesPerPixel", render->samplesPerPixel);
            render->shadows            = sec.getInt("shadows", render->shadows ? 1 : 0) != 0;
            render->reflections        = sec.getInt("reflections", render->reflections ? 1 : 0) != 0;
            render->refractions        = sec.getInt("refractions", render->refractions ? 1 : 0) != 0;
            render->transparentShadows = sec.getInt("transparentShadows",
                                                    render->transparentShadows ? 1 : 0) != 0;
            render->shadowBias      = sec.getFloat("shadowBias", render->shadowBias);
            if (sec.has("background")) {
                float br = 0, bg = 0, bb = 0;
                parseVec3(sec.get("background"), br, bg, bb);
                render->background = glm::vec3(br, bg, bb);
            }
            continue;
        }
        if (sec.name == "timeline") {
            if (!timeline) continue;
            timeline->length  = sec.getFloat("length", timeline->length);
            timeline->loopAll = sec.getInt("loop", timeline->loopAll ? 1 : 0) != 0;
            if (timeline->length <= 0.0f) {
                timeline->length = AnimationTimeline::kDefaultLength;
            }
            continue;
        }
        if (sec.name == "track") {
            if (!timeline) continue;

            AnimationTrack tr(sec.getInt("shape", -1));
            if (tr.shapeId < 0) {
                result.warnings.push_back("[track] with no shape id, skipped");
                continue;
            }
            tr.loop = sec.getInt("loop", 0) != 0;

            // A `rotationMode=` line may appear in a scene saved by a build
            // that briefly had a per-track quaternion mode. Unknown keys are
            // ignored, so such a file still loads correctly -- rotation is
            // interpolated per axis either way.

            const std::vector<std::string> rows = sec.getAll("key");
            for (size_t r = 0; r < rows.size(); ++r) {
                std::istringstream in2(rows[r]);
                Keyframe kf;
                in2 >> kf.time
                    >> kf.position.x >> kf.position.y >> kf.position.z
                    >> kf.rotation.x >> kf.rotation.y >> kf.rotation.z
                    >> kf.scale.x    >> kf.scale.y    >> kf.scale.z;
                if (in2.fail()) {
                    result.warnings.push_back("malformed key= in [track], skipped");
                    continue;
                }

                // Easing is read only after the ten floats have succeeded, so a
                // ten-field line from an older scene is a complete key rather
                // than a failed one. An unrecognised value falls back to linear
                // instead of indexing off the end of the curve table.
                int easing = EASE_LINEAR;
                if (in2 >> easing) {
                    kf.easing = (easing >= 0 && easing < EASE_COUNT)
                                    ? easing : EASE_LINEAR;
                }
                // addKey rather than push_back: a hand-edited file may list the
                // keys out of order or repeat a time, and evaluate() relies on
                // them being sorted and unique.
                tr.addKey(kf);
            }
            if (!tr.keys.empty()) {
                // A hand-edited file may key past the stored length; widen
                // rather than silently leaving keys unreachable.
                timeline->ensureLength(tr.endTime());
                timeline->tracks.push_back(tr);
            }
            continue;
        }
        if (sec.name != "shape") continue;

        Shape* s = constructShape(sec, shapeManager, path, result.warnings);
        if (!s) { ++result.shapesSkipped; continue; }

        applyCommon(s, sec, path, result.warnings);

        // Imported shapes were already added by the importer.
        bool alreadyAdded = false;
        std::vector<Shape*>& all = shapeManager.getShapes();
        for (size_t k = 0; k < all.size(); ++k) if (all[k] == s) alreadyAdded = true;
        if (!alreadyAdded) shapeManager.addShape(s);

        ++result.shapesLoaded;
    }

    // Keep the counter ahead of every id actually present.
    int maxId = shapeManager.getShapeCounter();
    std::vector<Shape*>& all = shapeManager.getShapes();
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i] && all[i]->getId() > maxId) maxId = all[i]->getId();
    }
    shapeManager.setShapeCounter(maxId);

    result.ok = true;
    return result;
}

} // namespace SceneIO
