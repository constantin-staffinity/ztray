/*
 * Decode embedded PNG tray icons (STB_IMAGE_STATIC keeps symbols private vs dvui's stb_image_impl).
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

#define ZTRAY_MAX_ICON_DIMENSION 1024
#define ZTRAY_MAX_ICON_BYTES (4u * 1024u * 1024u)

void *ztray_win32_icon_from_png(const unsigned char *data, int len) {
    int w = 0, h = 0, ch = 0;
    if (data == NULL || len <= 0 || !stbi_info_from_memory(data, len, &w, &h, &ch) ||
        w <= 0 || h <= 0 || w > ZTRAY_MAX_ICON_DIMENSION || h > ZTRAY_MAX_ICON_DIMENSION ||
        (size_t)w * (size_t)h > ZTRAY_MAX_ICON_BYTES / 4u)
        return NULL;

    unsigned char *rgba = stbi_load_from_memory(data, len, &w, &h, &ch, 4);
    if (rgba == NULL)
        return NULL;

    BITMAPINFOHEADER bi;
    memset(&bi, 0, sizeof(bi));
    bi.biSize = sizeof(bi);
    bi.biWidth = w;
    bi.biHeight = -h;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader = bi;

    void *bits = NULL;
    HDC hdc = GetDC(NULL);
    if (hdc == NULL) {
        stbi_image_free(rgba);
        return NULL;
    }
    HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    ReleaseDC(NULL, hdc);
    if (hbm == NULL || bits == NULL) {
        stbi_image_free(rgba);
        return NULL;
    }

    memcpy(bits, rgba, (size_t)w * (size_t)h * 4u);
    stbi_image_free(rgba);

    ICONINFO ii;
    memset(&ii, 0, sizeof(ii));
    ii.fIcon = TRUE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = NULL;
    ii.hbmColor = hbm;

    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(hbm);
    return icon;
}
