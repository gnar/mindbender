#ifndef DCDRAW_CANVAS_H
#define DCDRAW_CANVAS_H

#include <cstddef>

#include "shared_ptr.h"

#include <algorithm>

namespace DCDraw {
    class Canvas {
    public:
        enum Format {
            // packed 16bit
                    RGB565,   // RGB565
            RGBA4444, // RGBA4444
            ARGB4444, // ARGB4444

            // packed with 8-bit/channel
                    RGB,      // RGB888
            RGBA,     // RGB8888
            ARGB,     // ARGB8888
        };

        int GetWidth() { return bitmap->width; }

        int GetHeight() { return bitmap->height; }

        int GetPitch() { return bitmap->pitch; }

        unsigned char *GetData() { return bitmap->data; }

        Format GetFormat() { return bitmap->fmt; }

        bool HasAlpha();

        void GetPixel(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a);

        void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

        Canvas();

        Canvas(Format fmt, int width, int height);

        Canvas(Format fmt, unsigned char *data, int width, int height, int pitch = -1);

        ~Canvas();

        Canvas Clone();

        Canvas Convert(Format fmt);

    private:
        struct Bitmap {
            Format fmt;
            int width, height, pitch;
            unsigned char *data;

            Bitmap(Format fmt, int width, int height, int pitch, unsigned char *data)
                    : fmt(fmt), width(width), height(height), pitch(pitch), data(data) {}

            ~Bitmap() {
                delete[] data;
            }

            Bitmap *Clone() {
                const size_t size = height * pitch;
                auto *b = new Bitmap(fmt, width, height, pitch, new unsigned char[size]);
                std::copy(data, data + size, b->data);

                return b;
            }

        public: // prevent copying
            Bitmap(const Bitmap &other) = delete;
            Bitmap &operator=(const Bitmap &other) = delete;
        };

        internal::shared_ptr<Bitmap> bitmap;
    };
}

#endif
