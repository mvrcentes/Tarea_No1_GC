#pragma once

#include <iostream>
#include <cstdint>

class Color {
public:
  uint8_t r, g, b;
    Color() : r(0), g(0), b(0) {}

    Color(uint8_t red, uint8_t green, uint8_t blue) {
        // Clamping para los valores fuera del rango [0, 255]
        r = clamp(red);
        g = clamp(green);
        b = clamp(blue);
    }

    // Función de clamping
    uint8_t clamp(uint8_t value) const {
        if (value < 0) {
            return 0;
        } else if (value > 255) {
            return 255;
        }
        return value;
    }

    // Función para imprimir el color utilizando el operador de sobrecarga <<
    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "RGB(" << static_cast<int>(color.r) << ", " << static_cast<int>(color.g) << ", " << static_cast<int>(color.b) << ")";
        return os;
    }

    // Función para sumar dos colores sin sobrepasar 255
    Color operator+(const Color& other) const {
        uint8_t red = clamp(static_cast<uint8_t>(r + other.r));
        uint8_t green = clamp(static_cast<uint8_t>(g + other.g));
        uint8_t blue = clamp(static_cast<uint8_t>(b + other.b));
        return Color(red, green, blue);
    }

    // Función para multiplicar un color por un valor float
    Color operator*(float value) const {
        uint8_t red = clamp(static_cast<uint8_t>(r * value));
        uint8_t green = clamp(static_cast<uint8_t>(g * value));
        uint8_t blue = clamp(static_cast<uint8_t>(b * value));
        return Color(red, green, blue);
    }
};