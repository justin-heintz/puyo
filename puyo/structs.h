#pragma once
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

void outputLog(const std::string& msg, bool logThis = true) {
    if (logThis) {
        printf("%s\n", msg.c_str());
    }
}