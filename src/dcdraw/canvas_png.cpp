#include "canvas_png.h"
#include "dcdraw.h"

#include <png.h>
#include <iostream>

namespace DCDraw {

    using std::cout;
    using std::endl;

    // static
    Canvas PNG::Load(const std::string &fileName, AlphaMask mask) {
        std::string fn = GetPathPrefix() + fileName;

        // Open file
        FILE *f = fopen(fn.c_str(), "rb");

        if (!f) {
            cout << "Could not open file " << fn << " for reading." << endl;
            return Canvas();
        }

        // Check if f is a valid png file
        unsigned char header[8];

        fread(header, 1, 8, f);
        bool is_png = !png_sig_cmp(header, 0, 8);

        if (!is_png) {
            cout << fn << " is not a valid .png file." << endl;
            fclose(f);
            return Canvas();
        }

        // Initialize png library (png_ptr and info_ptr)
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

        if (!png_ptr) {
            cout << "Could not initialize libpng (step 1)." << endl;
            fclose(f);
            return Canvas();
        }

        png_infop png_info = png_create_info_struct(png_ptr);
        if (!png_info) {
            cout << "Could not initialize libpng (step 2)." << endl;
            png_destroy_read_struct(&png_ptr, (png_infopp) nullptr, (png_infopp) nullptr);
            fclose(f);
            return Canvas();
        }

        png_infop end_info = png_create_info_struct(png_ptr);
        if (!end_info) {
            cout << "Could not initialize libpng (step 3)." << endl;
            png_destroy_read_struct(&png_ptr, &png_info, (png_infopp) nullptr);
            fclose(f);
            return Canvas();
        }

        // setjmp..
        if (setjmp(png_jmpbuf(png_ptr))) {
            cout << "Could not initialize libpng (step 4)." << endl;
            png_destroy_read_struct(&png_ptr, &png_info, &end_info);
            fclose(f);
            return Canvas();
        }

        // Set file io
        png_init_io(png_ptr, f);
        png_set_sig_bytes(png_ptr, 8);

        // Read png info
        png_uint_32 width, height;
        int bit_depth;
        int color_type, interlace_type, compression_type, filter_method;

        png_read_info(png_ptr, png_info);
        png_get_IHDR(png_ptr, png_info, &width, &height,
                     &bit_depth, &color_type, &interlace_type,
                     &compression_type, &filter_method);

        // Filters:
        png_set_strip_16(png_ptr);    // 16-bit => 8 Bit
        png_set_packing(png_ptr);    // <8bit => 8bit ???
        png_set_gray_to_rgb(png_ptr);

        // Update image info
        png_read_update_info(png_ptr, png_info);
        png_get_IHDR(png_ptr, png_info, &width, &height,
                     &bit_depth, &color_type, &interlace_type,
                     &compression_type, &filter_method);

        Canvas::Format colorType;

        switch (color_type) {
            default:
            case PNG_COLOR_TYPE_GRAY:
            case PNG_COLOR_TYPE_GRAY_ALPHA:
            case PNG_COLOR_TYPE_PALETTE:
                cout << "Unsupported color type." << endl;
                png_destroy_read_struct(&png_ptr, &png_info, &end_info);
                fclose(f);
                return Canvas();

            case PNG_COLOR_TYPE_RGB:
                colorType = Canvas::RGB;
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                colorType = Canvas::RGBA;
                break;
        }

        // Create row pointers and pixel data
        int pitch = png_get_rowbytes(png_ptr, png_info);
        auto *rgba = new unsigned char[pitch * height];
        auto **row_ptrs = new png_byte *[height]; // array of pointers

        for (size_t i = 0; i < height; ++i) row_ptrs[i] = rgba + i * pitch;

        // Read image in one go
        png_read_image(png_ptr, row_ptrs);

        delete[] row_ptrs;

        // Read end info
        png_read_end(png_ptr, end_info);

        // Clean up..
        png_destroy_read_struct(&png_ptr, &png_info, &end_info);
        fclose(f);

        return Canvas(colorType, rgba, width, height, pitch);
    }

} // ns DCDraw

