#include "canvas.h"

#include <iostream>

using namespace std;

namespace DCDraw {
    Canvas::Canvas() : bitmap(nullptr) {
    }

    Canvas::Canvas(Format fmt, int width, int height) {
        int pitch = 0;
        switch (fmt) {
            case ARGB4444:
            case RGBA4444:
            case RGB565:
                pitch = width * 2;
                break;

            case RGB:
                pitch = width * 3;
                break;

            case RGBA:
            case ARGB:
                pitch = width * 4;
                break;
        }

        auto *data = new unsigned char[height * pitch];
        std::fill(data, data + height * pitch, 0);

        bitmap.reset(new Bitmap(fmt, width, height, pitch, data));
    }

    Canvas::Canvas(Format fmt, unsigned char *data, int width, int height, int pitch) {
        if (pitch == -1) {
            switch (fmt) {
                case ARGB4444:
                case RGBA4444:
                case RGB565:
                    pitch = width * 2;
                    break;

                case RGB:
                    pitch = width * 3;
                    break;

                case RGBA:
                case ARGB:
                    pitch = width * 4;
                    break;
            }
        }

        bitmap.reset(new Bitmap(fmt, width, height, pitch, data));
    }

    Canvas::~Canvas() = default;

    Canvas Canvas::Clone() {
        Canvas clone;
        if (bitmap.get()) {
            clone.bitmap.reset(bitmap->Clone());
        }
        return clone;
    }

    bool Canvas::HasAlpha() {
        if (bitmap.get()) {
            switch (bitmap->fmt) {
                case RGB565:
                case RGB:
                    return false;
                case ARGB4444:
                case RGBA4444:
                case RGBA:
                case ARGB:
                    return true;
            }
        }

        return false;
    }

    void Canvas::GetPixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a) {
        unsigned char *row = bitmap->data + y * bitmap->pitch;
        switch (bitmap->fmt) {
            case RGBA4444: {
                auto *pix = (unsigned short *) &row[x * 2]; //FIXME: typedef a 16 bit type
                r = (unsigned char) (255.f * ((*pix & 0xf000) >> 12) / 16.f);
                g = (unsigned char) (255.f * ((*pix & 0x0f00) >> 8) / 16.f);
                b = (unsigned char) (255.f * ((*pix & 0x00f0) >> 4) / 16.f);
                a = (unsigned char) (255.f * ((*pix & 0x000f)) / 16.f);
                break;
            }

            case ARGB4444: {
                auto *pix = (unsigned short *) &row[x * 2]; //FIXME: typedef a 16 bit type
                a = (unsigned char) (255.f * ((*pix & 0xf000) >> 12) / 16.f);
                r = (unsigned char) (255.f * ((*pix & 0x0f00) >> 8) / 16.f);
                g = (unsigned char) (255.f * ((*pix & 0x00f0) >> 4) / 16.f);
                b = (unsigned char) (255.f * ((*pix & 0x000f)) / 16.f);
                break;
            }

            case RGB565: {
                auto *pix = (unsigned short *) &row[x * 2]; //FIXME: typedef a 16 bit type
                r = (unsigned char) (255.f * ((*pix & 0xf800) >> 11) / 31.f);
                g = (unsigned char) (255.f * ((*pix & 0x07e0) >> 5) / 63.f);
                b = (unsigned char) (255.f * ((*pix & 0x001f)) / 31.f);
                a = 255;
                break;
            }

            case RGB:
                r = row[x * 3 + 0];
                g = row[x * 3 + 1];
                b = row[x * 3 + 2];
                a = 255;
                break;
            case RGBA:
                r = row[x * 4 + 0];
                g = row[x * 4 + 1];
                b = row[x * 4 + 2];
                a = row[x * 4 + 3];
                break;
            case ARGB:
                a = row[x * 4 + 0];
                r = row[x * 4 + 1];
                g = row[x * 4 + 2];
                b = row[x * 4 + 3];
                break;
        }
    }

    void Canvas::SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        if (bitmap.use_count() > 1) // Copy on write?
        {
            bitmap.reset(bitmap->Clone());
        }

        unsigned char *row = bitmap->data + y * bitmap->pitch;
        switch (bitmap->fmt) {
            case ARGB4444: {
                auto *pix = (unsigned short *) &row[x * 2]; //FIXME: typedef a 16 bit type
                *pix = ((unsigned short) (16.f * a / 255.f) << 12) & 0xf000
                       | ((unsigned short) (16.f * r / 255.f) << 8) & 0x0f00
                       | ((unsigned short) (16.f * g / 255.f) << 4) & 0x00f0
                       | ((unsigned short) (16.f * b / 255.f)) & 0x000f;
                break;
            }

            case RGBA4444: {
                auto *pix = (unsigned short *) &row[x * 2]; //FIXME: typedef a 16 bit type
                *pix = ((unsigned short) (16.f * r / 255.f) << 12) & 0xf000
                       | ((unsigned short) (16.f * g / 255.f) << 8) & 0x0f00
                       | ((unsigned short) (16.f * b / 255.f) << 4) & 0x00f0
                       | ((unsigned short) (16.f * a / 255.f)) & 0x000f;
                break;
            }

            case RGB565: {
                auto *pix = (unsigned short *) &row[x * 2]; //FIXME: typedef a 16 bit type
                *pix = ((unsigned short) (31.f * r / 255.f) << 11) & 0xf800
                       | ((unsigned short) (63.f * g / 255.f) << 5) & 0x07e0
                       | ((unsigned short) (31.f * b / 255.f)) & 0x001f;
                break;
            }

            case RGB:
                row[x * 3 + 0] = r;
                row[x * 3 + 1] = g;
                row[x * 3 + 2] = b;
                break;
            case RGBA:
                row[x * 4 + 0] = r;
                row[x * 4 + 1] = g;
                row[x * 4 + 2] = b;
                row[x * 4 + 3] = a;
                break;
            case ARGB:
                row[x * 4 + 0] = a;
                row[x * 4 + 1] = r;
                row[x * 4 + 2] = g;
                row[x * 4 + 3] = b;
                break;
        }
    }

    Canvas Canvas::Convert(Format fmt) {
        if (GetFormat() == fmt) return *this;

        const int W = GetWidth();
        const int H = GetHeight();

        Canvas dst(fmt, W, H);
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                unsigned char r, g, b, a;
                GetPixel(x, y, r, g, b, a);
                dst.SetPixel(x, y, r, g, b, a);
            }
        }
        return dst;
    }
}

