/*
    MindBender - The MindBender adventure engine
    Copyright (C) 2006  Gunnar Selke

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "image.h"
#include "manager.h"

#include <iostream>

using namespace std;

///////////////////////////////////////////////////
// PNG loading                                   //
///////////////////////////////////////////////////

#include <png.h>

static void png_read_data(png_structp ctx, png_bytep area, png_size_t size) {
    auto *file = (PHYSFS_File *) png_get_io_ptr(ctx);
    PHYSFS_read(file, area, 1, size);
}

DCDraw::Canvas LoadPNG(PHYSFS_File *file) {
    using std::cout;
    using std::endl;

    // Check if f is a valid png file
    unsigned char header[8];

    PHYSFS_read(file, header, 1, 8);
    bool is_png = !png_sig_cmp(header, 0, 8);

    if (!is_png) {
        cout << "This is not a valid .png file." << endl;
        return DCDraw::Canvas();
    }

    // Initialize png library (png_ptr and info_ptr)
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!png_ptr) {
        cout << "Could not initialize libpng (step 1)." << endl;
        return DCDraw::Canvas();
    }

    png_infop png_info = png_create_info_struct(png_ptr);
    if (!png_info) {
        cout << "Could not initialize libpng (step 2)." << endl;
        png_destroy_read_struct(&png_ptr, (png_infopp) nullptr, (png_infopp) nullptr);
        return DCDraw::Canvas();
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        cout << "Could not initialize libpng (step 3)." << endl;
        png_destroy_read_struct(&png_ptr, &png_info, (png_infopp) nullptr);
        return DCDraw::Canvas();
    }

    // setjmp..
    if (setjmp(png_jmpbuf(png_ptr))) {
        cout << "Could not initialize libpng (step 4)." << endl;
        png_destroy_read_struct(&png_ptr, &png_info, &end_info);
        return DCDraw::Canvas();
    }

    // Set file io
    //png_init_io(png_ptr, f);
    png_set_read_fn(png_ptr, (void *) file, png_read_data);
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

    DCDraw::Canvas::Format colorType;

    switch (color_type) {
        default:
        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
        case PNG_COLOR_TYPE_PALETTE:
            cout << "Unsupported color type." << endl;
            png_destroy_read_struct(&png_ptr, &png_info, &end_info);
            return DCDraw::Canvas();

        case PNG_COLOR_TYPE_RGB:
            colorType = DCDraw::Canvas::RGB;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            colorType = DCDraw::Canvas::RGBA;
            break;
    }

    // Create row pointers and pixel data
    png_size_t pitch = png_get_rowbytes(png_ptr, png_info);
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

    return DCDraw::Canvas(colorType, rgba, width, height, pitch);
}

namespace Res {
    Image::Image(const Resource::ID &id)
            : Resource(Resource::IMAGE, id) {
    }

    Image::~Image() {
        Unload();
    }

    void Image::Load() {
        if (loaded) return;

        PHYSFS_File *handle = Manager.OpenFile(GetID());
        DCDraw::Canvas canvas = LoadPNG(handle);
        PHYSFS_close(handle);

        texture.reset(new DCDraw::Texture(canvas));

        loaded = true;
    }

    void Image::Unload() {
        if (!loaded) return;

        texture.reset(nullptr);

        loaded = false;
    }

    Image *CreateImageResource(const Resource::ID &id) {
        if (!Manager.ExistsFile(id)) return nullptr; // file not found?
        return new Image(id);
    }

} //ns

