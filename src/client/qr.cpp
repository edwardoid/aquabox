/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qr.h"
#include <cmath>
#include <math.h>
#include <plog/Log.h>
#include <png.h>
#include <qrencode.h>
#include <stdio.h>
#include <vector>

#define INCHES_PER_METER (100.0 / 2.54)

using namespace qr;

static unsigned char fg_color[4] = { 0, 0, 0, 255 };
static unsigned char bg_color[4] = { 255, 255, 255, 255 };

static void fillRow(std::vector<unsigned char>& row, int num,
    const unsigned char color[])
{
    unsigned char* p = row.data();
    for (int i = 0; i < num; i++) {
        memcpy(p, color, 4);
        p += 4;
    }
}

bool Qr::create(std::string data, std::string pngPath, double zoomFactor)
{
    int margin = 1;
    int size = std::ceil(3 * zoomFactor);
    int dpi = 72;
    QRcode* qrcode = QRcode_encodeString(data.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (qrcode == NULL) {
        if (errno == ERANGE) {
            LOG_ERROR << "Failed to encode the input data: Input data too large";
        } else {
            LOG_ERROR << "Failed to encode the input data";
        }
        return false;
    }

    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette = NULL;
    png_byte alpha_values[2];
    unsigned char *p, *q;
    int x, y, xx, yy, bit;
    int realwidth = (qrcode->width + 2 * margin) * size;

    std::vector<unsigned char> row(realwidth * 4);

    FILE* fp = fopen(pngPath.c_str(), "wb");
    if (fp == NULL) {
        LOG_ERROR << "Failed to create file: " << pngPath;
        return false;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        LOG_ERROR << "Failed to initialize PNG writer.";
        fclose(fp);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        LOG_ERROR << "Failed to initialize PNG write.\n";
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        LOG_ERROR << "Failed to write PNG image.";
        fclose(fp);
        return false;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, (unsigned int)realwidth,
        (unsigned int)realwidth, 8, PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_set_pHYs(png_ptr, info_ptr, dpi * INCHES_PER_METER,
        dpi * INCHES_PER_METER, PNG_RESOLUTION_METER);
    png_write_info(png_ptr, info_ptr);

    fillRow(row, realwidth, bg_color);
    for (y = 0; y < margin * size; y++) {
        png_write_row(png_ptr, row.data());
    }

    /* data */
    p = qrcode->data;
    for (y = 0; y < qrcode->width; y++) {
        fillRow(row, realwidth, bg_color);
        for (x = 0; x < qrcode->width; x++) {
            for (xx = 0; xx < size; xx++) {
                if (*p & 1) {
                    memcpy(&row[((margin + x) * size + xx) * 4], fg_color, 4);
                }
            }
            p++;
        }
        for (yy = 0; yy < size; yy++) {
            png_write_row(png_ptr, row.data());
        }
    }
    /* bottom margin */
    fillRow(row, realwidth, bg_color);
    for (y = 0; y < margin * size; y++) {
        png_write_row(png_ptr, row.data());
    }

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
    free(palette);

    return true;
}