#ifndef COLORPRESETS_H
#define COLORPRESETS_H

struct ImColorPreset {
    const char* name;
    float color[3];
};

extern ImColorPreset colorPresets[32];

#endif // COLORPRESETS_H
