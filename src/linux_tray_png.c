#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

#include <stdint.h>
#include <stdlib.h>

#define ZTRAY_MAX_ICON_DIMENSION 1024
#define ZTRAY_MAX_ICON_BYTES (4u * 1024u * 1024u)

int ztray_linux_icon_pixmap_from_png(
    const unsigned char *data,
    int len,
    int *out_width,
    int *out_height,
    unsigned char **out_argb) {
    int width = 0;
    int height = 0;
    int channels = 0;
    if (data == NULL || len <= 0 || out_width == NULL || out_height == NULL || out_argb == NULL ||
        !stbi_info_from_memory(data, len, &width, &height, &channels) || width <= 0 || height <= 0 ||
        width > ZTRAY_MAX_ICON_DIMENSION || height > ZTRAY_MAX_ICON_DIMENSION ||
        (size_t)width * (size_t)height > ZTRAY_MAX_ICON_BYTES / 4u)
        return 0;

    unsigned char *rgba = stbi_load_from_memory(data, len, &width, &height, &channels, 4);
    if (rgba == NULL)
        return 0;

    size_t pixel_count = (size_t)width * (size_t)height;
    unsigned char *argb = malloc(pixel_count * 4u);
    if (argb == NULL) {
        stbi_image_free(rgba);
        return 0;
    }

    for (size_t i = 0; i < pixel_count; i++) {
        const unsigned char *pixel = rgba + i * 4u;
        unsigned char *output = argb + i * 4u;
        output[0] = pixel[3];
        output[1] = pixel[0];
        output[2] = pixel[1];
        output[3] = pixel[2];
    }

    stbi_image_free(rgba);
    *out_width = width;
    *out_height = height;
    *out_argb = (unsigned char *)argb;
    return 1;
}
