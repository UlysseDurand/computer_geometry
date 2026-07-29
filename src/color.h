#ifndef color_h
#define color_h

#include <array>
#include <algorithm>

// Colormap simple bleu → rouge
inline std::array<unsigned char, 4> colormap(float t)
{
    // t est dans [0,1] après normalisation
    t = std::min(std::max(t, 0.0f), 1.0f);
    // Remap t dans [-1,1]
    float x = 2.0f * t - 1.0f;
    // Échelle log symétrique
    float y = (x == 0.0f) ? 0.0f : std::copysign(std::log10(1.0f + 9.0f * std::fabs(x)), x);
    // y est dans [-1,1], on remappe dans [0,1]
    float c = 0.5f * (y + 1.0f);
    // Bleu → Blanc → Rouge
    unsigned char r = (unsigned char)(255 * c);
    unsigned char g = (unsigned char)(255 * (1.0f - std::fabs(c - 0.5f) * 2.0f));
    unsigned char b = (unsigned char)(255 * (1.0f - c));
    unsigned char a = 255;

    return {r, g, b, a};
}
#endif