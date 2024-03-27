#pragma once
#include <vector>
#include "External/pngLib/include/PngWrapper.h"
#include "vec.h"
struct texture_map
{
    
	std::vector<std::vector<vec4>> color_map;
	void loadPNG(const char* png_file)
	{
        FILE* file = fopen(png_file, "rb");
        if (!file) {
            std::cerr << "Error opening file: " << png_file << std::endl;
        }

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) {
            fclose(file);
            std::cerr << "Error initializing PNG read structure" << std::endl;
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_read_struct(&png, nullptr, nullptr);
            fclose(file);
            std::cerr << "Error initializing PNG info structure" << std::endl;
        }

        if (setjmp(png_jmpbuf(png))) {
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(file);
            std::cerr << "Error reading PNG file" << std::endl;
        }

        png_init_io(png, file);
        png_set_sig_bytes(png, 0);

        png_read_info(png, info);
        int width = png_get_image_width(png, info);
        int height = png_get_image_height(png, info);
        png_byte color_type = png_get_color_type(png, info);
        png_byte bit_depth = png_get_bit_depth(png, info);

        if (color_type != PNG_COLOR_TYPE_RGBA) {
            std::cerr << "Only RGBA PNG images are supported" << std::endl;
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(file);
        }

        color_map.resize(height);
        for (int y = 0; y < height; ++y) {
            color_map[y].resize(width);
        }

        std::vector<png_byte> row(width * 4);
        for (int y = 0; y < height; ++y) {
            png_read_row(png, row.data(), nullptr);
            for (int x = 0; x < width; ++x) {
                color_map[y][x].x = row[x * 4] / 255.0f;
                color_map[y][x].y = row[x * 4 + 1] / 255.0f;
                color_map[y][x].z = row[x * 4 + 2] / 255.0f;
                color_map[y][x].w = row[x * 4 + 3] / 255.0f;
            }
        }

        png_destroy_read_struct(&png, &info, nullptr);
        fclose(file);
	}

    texture_map(const char* png_file) { loadPNG(png_file); }

};
