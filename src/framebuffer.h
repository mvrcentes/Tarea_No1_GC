#pragma once

#include <vector>
#include "color.h"

class Framebuffer {
public:
    Framebuffer(int width, int height) : width_(width), height_(height) {
        framebuffer_.resize(width_ * height_);
    }

    void clear(const Color& clearColor) {
        for (size_t i = 0; i < framebuffer_.size(); ++i) {
            framebuffer_[i] = clearColor;
        }
    }

    void set(int x, int y, const Color& color) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return;  // Clamping para valores fuera del rango del framebuffer
        }
        framebuffer_[y * width_ + x] = color;
    }

    Color& getPixel(int x, int y) {
        return framebuffer_[y * width_ + x];
    }

    const std::vector<Color>& getFramebuffer() const {
        return framebuffer_;
    }

    int getWidth() const {
        return width_;
    }

    int getHeight() const {
        return height_;
    }

private:
    std::vector<Color> framebuffer_;
    int width_;
    int height_;
};