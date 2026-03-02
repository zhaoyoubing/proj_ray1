#ifndef BMP_WRITER_H
#define BMP_WRITER_H

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

class BMPWriter {
public:
    static void write(const std::string& filename, int width, int height, const std::vector<uint8_t>& pixels) {
        // pixels είναι BGR format (3 bytes per pixel)
        // Each row must be padded to 4-byte boundary

        int row_size = ((width * 3 + 3) / 4) * 4;  // padded row size
        int image_data_size = row_size * height;
        int file_size = 54 + image_data_size;  // header + data

        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return;
        }

        // === BMP File Header (14 bytes) ===
        uint8_t file_header[14] = {
            'B', 'M',                                    // Signature
            (uint8_t)(file_size & 0xFF),               // File size (4 bytes, little-endian)
            (uint8_t)((file_size >> 8) & 0xFF),
            (uint8_t)((file_size >> 16) & 0xFF),
            (uint8_t)((file_size >> 24) & 0xFF),
            0, 0, 0, 0,                                 // Reserved
            54, 0, 0, 0                                 // Offset to pixel data (54 bytes)
        };
        file.write((char*)file_header, 14);

        // === BMP Info Header (40 bytes) ===
        uint8_t info_header[40] = {
            40, 0, 0, 0,                               // Header size
            (uint8_t)(width & 0xFF),                   // Width (4 bytes, little-endian)
            (uint8_t)((width >> 8) & 0xFF),
            (uint8_t)((width >> 16) & 0xFF),
            (uint8_t)((width >> 24) & 0xFF),
            (uint8_t)(height & 0xFF),                  // Height (4 bytes, little-endian)
            (uint8_t)((height >> 8) & 0xFF),
            (uint8_t)((height >> 16) & 0xFF),
            (uint8_t)((height >> 24) & 0xFF),
            1, 0,                                       // Planes (1)
            24, 0,                                      // Bits per pixel (24)
            0, 0, 0, 0,                                 // Compression (0 = none)
            (uint8_t)(image_data_size & 0xFF),         // Image size
            (uint8_t)((image_data_size >> 8) & 0xFF),
            (uint8_t)((image_data_size >> 16) & 0xFF),
            (uint8_t)((image_data_size >> 24) & 0xFF),
            0, 0, 0, 0,                                 // X pixels per meter
            0, 0, 0, 0,                                 // Y pixels per meter
            0, 0, 0, 0,                                 // Colors used
            0, 0, 0, 0                                  // Important colors
        };
        file.write((char*)info_header, 40);

        // === Pixel Data (bottom-up, BGR format) ===
        // BMP stores rows from bottom to top, so reverse iteration
        for (int j = height - 1; j >= 0; j--) {
            for (int i = 0; i < width; i++) {
                int idx = (j * width + i) * 3;
                file.put(pixels[idx]);      // B
                file.put(pixels[idx + 1]);  // G
                file.put(pixels[idx + 2]);  // R
            }
            // Add padding (zeros) to align to 4-byte boundary
            int padding = row_size - (width * 3);
            for (int p = 0; p < padding; p++) {
                file.put(0);
            }
        }

        file.close();
        std::clog << "BMP written to: " << filename << std::endl;
    }
};

#endif