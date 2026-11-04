#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>

#ifndef M_PI
#define M_PI  3.14159265358979
#endif

// Declare the global variables
// Set by the instructor, in code.
extern std::string courseName;   // e.g. "CPSC 566"
extern std::string appName;      // e.g. "3D Graphics Editor"

// These two are per-user and come from Options (persisted in editor.cfg).
// Kept as globals so existing call sites keep working; synced from settings
// at startup and whenever Options changes them.
extern std::string customShapeName;
extern std::string windowTitle;
extern std::vector<std::string> jointName;


#endif // GLOBALS_H