#include <iostream>
#include <string>
#include <NESupport.h>
#include <stb_image_write.h>
#include <json.hpp>

#include <codecvt>
#include <Sprite.h>
#include <wdf.h>

unsigned int string_id(const char *str) {
    int i;
    unsigned int v;
    static unsigned m[70];
    strncpy((char *) m, str, 256);
    for (i = 0; i < 256 / 4 && m[i]; i++);
    m[i++] = 0x9BE74448, m[i++] = 0x66F42C48;
    v = 0xF4FA8928;

    unsigned int cf = 0;
    unsigned int esi = 0x37A8470E;
    unsigned int edi = 0x7758B42B;
    unsigned int eax = 0;
    unsigned int ebx = 0;
    unsigned int ecx = 0;
    unsigned int edx = 0;
    unsigned long long temp = 0;
    while (true) {
        // mov ebx, 0x267B0B11
        ebx = 0x267B0B11;
        // rol v, 1
        cf = (v & 0x80000000) > 0 ? 1 : 0;
        v = ((v << 1) & 0xFFFFFFFF) + cf;
        // xor ebx, v
        ebx = ebx ^ v;
        // mov eax, [eax + ecx * 4]
        eax = m[ecx];
        // mov edx, ebx
        edx = ebx;
        // xor esi, eax
        esi = esi ^ eax;
        // xor edi, eax
        edi = edi ^ eax;
        // add edx, edi
        temp = (unsigned long long) edx + (unsigned long long) edi;
        cf = (temp & 0x100000000) > 0 ? 1 : 0;
        edx = temp & 0xFFFFFFFF;
        // or edx, 0x2040801
        edx = edx | 0x2040801;
        // and edx, 0xBFEF7FDF
        edx = edx & 0xBFEF7FDF;
        // mov eax, esi
        eax = esi;
        // mul edx
        temp = (unsigned long long) eax * (unsigned long long) edx;
        eax = temp & 0xffffffff;
        edx = (temp >> 32) & 0xffffffff;
        cf = edx > 0 ? 1 : 0;
        // adc eax, edx
        temp = (unsigned long long) eax + (unsigned long long) edx + (unsigned long long) cf;
        eax = temp & 0xffffffff;
        cf = (temp & 0x100000000) > 0 ? 1 : 0;
        // mov edx, ebx
        edx = ebx;
        // adc eax, 0
        temp = (unsigned long long) eax + (unsigned long long) cf;
        eax = temp & 0xffffffff;
        cf = (temp & 0x100000000) > 0 ? 1 : 0;
        // add edx, esi
        temp = (unsigned long long) edx + (unsigned long long) esi;
        cf = (temp & 0x100000000) > 0 ? 1 : 0;
        edx = temp & 0xFFFFFFFF;
        // or edx, 0x804021
        edx = edx | 0x804021;
        // and edx, 0x7DFEFBFF
        edx = edx & 0x7DFEFBFF;
        // mov esi, eax
        esi = eax;
        // mov eax, edi
        eax = edi;
        // mul edx
        temp = (unsigned long long) eax * (unsigned long long) edx;
        eax = temp & 0xffffffff;
        edx = (temp >> 32) & 0xffffffff;
        cf = edx > 0 ? 1 : 0;
        // add edx, edx
        temp = (unsigned long long) edx + (unsigned long long) edx;
        cf = (temp & 0x100000000) > 0 ? 1 : 0;
        edx = temp & 0xFFFFFFFF;
        // adc eax, edx
        temp = (unsigned long long) eax + (unsigned long long) edx + (unsigned long long) cf;
        eax = temp & 0xffffffff;
        cf = (temp & 0x100000000) > 0 ? 1 : 0;
        // jnc _skip
        if (cf != 0) {
            // add eax, 2
            temp = (unsigned long long) eax + 2;
            cf = (temp & 0x100000000) > 0 ? 1 : 0;
            eax = temp & 0xFFFFFFFF;
        }
        // inc ecx;
        ecx += 1;
        // mov edi, eax
        edi = eax;
        // cmp ecx, i  jnz _loop
        if (ecx - i == 0) break;
    }
    // xor esi, edi
    esi = esi ^ edi;
    // mov v, esi
    v = esi;
    return v;
}

void wdf_export_nes(const std::string &wdfPath, const std::string &outDir, int mode) {
    nlohmann::json js;
    NE::WDF wdf(wdfPath);
    if (!wdf.m_FileType) {
        std::cout << wdfPath << " is not a valid WDF file" << std::endl;
        return;
    }

    if (!std::filesystem::exists(outDir) || !std::filesystem::is_directory(outDir)) {
        try {
            std::filesystem::create_directory(outDir);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            throw;
        }
    }
    std::cout << "File count: " << wdf.mIndencies.size() << std::endl;
    for (int i = 0; i < wdf.mIndencies.size(); i++) {
        uint32_t hash = wdf.mIndencies[i].hash;
        auto wasType = wdf.GetWasType(hash);
        char hashStr[9];
        sprintf(hashStr, "%08X", hash);
        std::cout << i << " " << hashStr << " " << wasType << std::endl;
        if (wasType != NE::eFILE_TYPE_SPRITE) {
            std::string suffix;
            switch (wdf.GetWasType(hash)) {
                case NE::eFILE_TYPE_TGA:
                    suffix = ".tga";
                    break;
                case NE::eFILE_TYPE_PNG:
                    suffix = ".png";
                    break;
                case NE::eFILE_TYPE_JPEG:
                    suffix = ".jpeg";
                    break;
                case NE::eFILE_TYPE_BMP:
                    suffix = ".bmp";
                    break;
                case NE::eFILE_TYPE_FSB4: // fmod
                    suffix = ".fmod";
                    break;
                case NE::eFILE_TYPE_OGGS:
                    suffix = ".oggs";
                    break;
                case NE::eFILE_TYPE_MP3:
                    suffix = ".mp3";
                    break;
                case NE::eFILE_TYPE_WAV: // WAV
                    suffix = ".wav";
                    break;
                case NE::eFILE_TYPE_ACON: // ani
                    suffix = ".ani";
                    break;
                case NE::eFILE_TYPE_TEXT:
                    suffix = ".txt";
                    break;
                case NE::eFILE_TYPE_RAR: // rar
                    suffix = ".rar";
                    break;
                case NE::eFILE_TYPE_UNKNOWN:
                default: ;
            }
            uint8_t *data;
            size_t size;
            wdf.LoadFileData(hash, data, size);
            std::string fileName = outDir + "/" + hashStr + suffix;
            std::fstream fs(fileName, std::ios::out | std::ios::binary);
            fs.write((char *) data, size);
            fs.close();
            continue;
        }
        auto sp = wdf.LoadSprite(hash);
        if (!sp) {
            continue;
        }
        switch (mode) {
            case 0: // 合并
            {
                int left = -sp->Frames[0].KeyX;
                int right = sp->Frames[0].Width - sp->Frames[0].KeyX;
                int top = -sp->Frames[0].KeyY;
                int bottom = sp->Frames[0].Height - sp->Frames[0].KeyY;
                for (int i = 1; i < sp->Frames.size(); i++) {
                    left = std::min(left, -sp->Frames[i].KeyX);
                    right = std::max(right, sp->Frames[i].Width - sp->Frames[i].KeyX);
                    top = std::min(top, -sp->Frames[i].KeyY);
                    bottom = std::max(bottom, sp->Frames[i].Height - sp->Frames[i].KeyY);
                }
                int width = right - left;
                int height = bottom - top;
                int keyX = -left;
                int keyY = -top;

                js[hashStr] = {
                    {"ID", hashStr},
                    {"GroupCount", sp->GroupCount},
                    {"GroupFrameCount", sp->GroupFrameCount},
                    {"Times", sp->Times},
                    {"Width", width},
                    {"Height", height},
                    {"KeyX", keyX},
                    {"KeyY", keyY},
                };
                // 1像素间隙
                int sheetWidth = (width + 1) * sp->GroupFrameCount - 1;
                int sheetHeight = (height + 1) * sp->GroupCount - 1;
                std::vector<uint32_t> pixels(sheetWidth * sheetHeight);

                for (int i = 0; i < sp->GroupCount; ++i) {
                    for (int j = 0; j < sp->GroupFrameCount; ++j) {
                        auto &frame = sp->Frames[i * sp->GroupFrameCount + j];

                        for (int row = 0; row < frame.Height; ++row) {
                            int yoff = keyY - frame.KeyY + row;
                            int xoff = keyX - frame.KeyX;
                            int index = (i * height + i + yoff) * sheetWidth + j * width + xoff + j;
                            memcpy(&pixels[index], &frame.Src[row * frame.Width], frame.Width * sizeof(uint32_t));
                        }
                    }
                }
                std::string fileName = outDir + "/" + hashStr + ".tga";
                stbi_write_tga(fileName.c_str(), sheetWidth, sheetHeight, 4, pixels.data());
                break;
            }
            case 1: // 统一尺寸
            {
                int left = -sp->Frames[0].KeyX;
                int right = sp->Frames[0].Width - sp->Frames[0].KeyX;
                int top = -sp->Frames[0].KeyY;
                int bottom = sp->Frames[0].Height - sp->Frames[0].KeyY;
                for (int i = 1; i < sp->Frames.size(); i++) {
                    left = std::min(left, -sp->Frames[i].KeyX);
                    right = std::max(right, sp->Frames[i].Width - sp->Frames[i].KeyX);
                    top = std::min(top, -sp->Frames[i].KeyY);
                    bottom = std::max(bottom, sp->Frames[i].Height - sp->Frames[i].KeyY);
                }
                int width = right - left;
                int height = bottom - top;
                int keyX = -left;
                int keyY = -top;

                js[hashStr] = {
                    {"ID", hashStr},
                    {"GroupCount", sp->GroupCount},
                    {"GroupFrameCount", sp->GroupFrameCount},
                    {"Times", sp->Times},
                    {"Width", width},
                    {"Height", height},
                    {"KeyX", keyX},
                    {"KeyY", keyY},
                };

                for (int i = 0; i < sp->Frames.size(); i++) {
                    std::vector<uint32_t> pixels(width * height);
                    auto &frame = sp->Frames[i];
                    for (int row = 0; row < frame.Height; ++row) {
                        int yoff = keyY - frame.KeyY + row;
                        int xoff = keyX - frame.KeyX;
                        memcpy(&pixels[yoff * width + xoff], &frame.Src[row * frame.Width],
                               frame.Width * sizeof(uint32_t));
                    }
                    char fileName[200];
                    sprintf(fileName, "%s/%s_%03d.tga", outDir.c_str(), hashStr, i);
                    stbi_write_tga(fileName, sp->Width, sp->Height, 4, pixels.data());
                }
                break;
            }
            case 2: // 单张 frame 尺寸
            {
                js[hashStr] = {
                    {"ID", hashStr},
                    {"GroupCount", sp->GroupCount},
                    {"GroupFrameCount", sp->GroupFrameCount},
                    {"Times", sp->Times},
                    {"Width", sp->Width},
                    {"Height", sp->Height},
                    {"KeyX", sp->KeyX},
                    {"KeyY", sp->KeyY},
                };

                for (int i = 0; i < sp->Frames.size(); i++) {
                    auto &frame = sp->Frames[i];
                    js[hashStr]["Frames"].emplace_back(nlohmann::json::object({
                        {"Width", sp->Width}, {"Height", sp->Height}, {"KeyX", sp->KeyX}, {"KeyY", sp->KeyY}
                    }));
                    char fileName[200];
                    sprintf(fileName, "%s/%s_%03d.tga", outDir.c_str(), hashStr, i);
                    stbi_write_tga(fileName, frame.Width, frame.Height, 4, frame.Src.data());
                }
                break;
            }
            default: break;
        }
        wdf.UnLoadSprite(hash);
    }
    if (js.empty())
        return;
    std::fstream fs(outDir + "/list.json", std::ios::out);
    fs << js.dump();
    fs.close();
}

void wdf_export_xy2(const std::string &wdfPath, const std::string &outDir, int mode) {
    nlohmann::json js;
    XY2::Wdf wdf(wdfPath);
    if (!wdf.m_isValid) {
        std::cout << wdfPath << " is not a valid WDF file" << std::endl;
        return;
    }

    if (!std::filesystem::exists(outDir) || !std::filesystem::is_directory(outDir)) {
        try {
            std::filesystem::create_directory(outDir);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            throw;
        }
    }
    std::cout << "File count: " << wdf.m_header->wasCount << std::endl;
    XY2::Sprite sp;
    for (int i = 0; i < wdf.m_header->wasCount; i++) {
        uint32_t hash = wdf.m_wasInfos[i].hash;
        int size = 0;
        char *data = wdf.getWas(i, &size);
        auto wasType = wdf.check_file_type(data, size);
        char hashStr[9];
        sprintf(hashStr, "%08X", hash);
        std::cout << i << " " << hashStr << " " << wasType << std::endl;
        if (wasType != XY2::FT_SP) {
            std::string suffix;
            switch (wasType) {
                case XY2::FT_TGA:
                    suffix = ".tga";
                    break;
                case XY2::FT_PNG:
                    suffix = ".png";
                    break;
                case XY2::FT_JPEG:
                    suffix = ".jpeg";
                    break;
                case XY2::FT_BMP:
                    suffix = ".bmp";
                    break;
                case XY2::FT_FSB4: // fmod
                    suffix = ".fmod";
                    break;
                case XY2::FT_OGGS:
                    suffix = ".oggs";
                    break;
                case XY2::FT_MP3:
                    suffix = ".mp3";
                    break;
                case XY2::FT_WAV: // WAV
                    suffix = ".wav";
                    break;
                case XY2::FT_ACON: // ani
                    suffix = ".ani";
                    break;
                case XY2::FT_TEXT:
                    suffix = ".txt";
                    break;
                case XY2::FT_RAR: // rar
                    suffix = ".rar";
                    break;
                case XY2::FT_UNKNOWN:
                default: ;
            }
            std::string fileName = outDir + "/" + hashStr + suffix;
            std::fstream fs(fileName, std::ios::out | std::ios::binary);
            fs.write((char *) data, size);
            fs.close();
            continue;
        }
        if (!sp.load(data, size)) {
            continue;
        }
        switch (mode) {
            case 0: // 合并
            {
                int left = -sp.m_frames[0].x;
                int right = sp.m_frames[0].width - sp.m_frames[0].x;
                int top = -sp.m_frames[0].y;
                int bottom = sp.m_frames[0].height - sp.m_frames[0].y;
                for (int i = 1; i < sp.m_frames.size(); i++) {
                    left = std::min(left, -sp.m_frames[i].x);
                    right = std::max(right, sp.m_frames[i].width - sp.m_frames[i].x);
                    top = std::min(top, -sp.m_frames[i].y);
                    bottom = std::max(bottom, sp.m_frames[i].height - sp.m_frames[i].y);
                }
                int width = right - left;
                int height = bottom - top;
                int keyX = -left;
                int keyY = -top;

                js[hashStr] = {
                    {"ID", hashStr},
                    {"GroupCount", sp.m_header->directionNum},
                    {"GroupFrameCount", sp.m_header->frameNum},
                    {"Times", sp.m_times},
                    {"Width", width},
                    {"Height", height},
                    {"KeyX", keyX},
                    {"KeyY", keyY},
                };
                // 1像素间隙
                int sheetWidth = (width + 1) * sp.m_header->frameNum - 1;
                int sheetHeight = (height + 1) * sp.m_header->directionNum - 1;
                std::vector<uint32_t> pixels(sheetWidth * sheetHeight);

                for (int i = 0; i < sp.m_header->directionNum; ++i) {
                    for (int j = 0; j < sp.m_header->frameNum; ++j) {
                        auto &frame = sp.m_frames[i * sp.m_header->frameNum + j];

                        for (int row = 0; row < frame.height; ++row) {
                            int yoff = keyY - frame.y + row;
                            int xoff = keyX - frame.x;
                            int index = (i * height + i + yoff) * sheetWidth + j * width + xoff + j;
                            memcpy(&pixels[index], &frame.pixels[row * frame.width], frame.width * sizeof(uint32_t));
                        }
                    }
                }
                std::string fileName = outDir + "/" + hashStr + ".tga";
                stbi_write_tga(fileName.c_str(), sheetWidth, sheetHeight, 4, pixels.data());
                break;
            }
            case 1: // 统一尺寸
            {
                int left = -sp.m_frames[0].x;
                int right = sp.m_frames[0].width - sp.m_frames[0].x;
                int top = -sp.m_frames[0].y;
                int bottom = sp.m_frames[0].height - sp.m_frames[0].y;
                for (int i = 1; i < sp.m_frames.size(); i++) {
                    left = std::min(left, -sp.m_frames[i].x);
                    right = std::max(right, sp.m_frames[i].width - sp.m_frames[i].x);
                    top = std::min(top, -sp.m_frames[i].y);
                    bottom = std::max(bottom, sp.m_frames[i].height - sp.m_frames[i].y);
                }
                int width = right - left;
                int height = bottom - top;
                int keyX = -left;
                int keyY = -top;

                js[hashStr] = {
                    {"ID", hashStr},
                    {"GroupCount", sp.m_header->directionNum},
                    {"GroupFrameCount", sp.m_header->frameNum},
                    {"Times", sp.m_times},
                    {"Width", width},
                    {"Height", height},
                    {"KeyX", keyX},
                    {"KeyY", keyY},
                };

                for (int i = 0; i < sp.m_frames.size(); i++) {
                    std::vector<uint32_t> pixels(width * height);
                    auto &frame = sp.m_frames[i];
                    for (int row = 0; row < frame.height; ++row) {
                        int yoff = keyY - frame.y + row;
                        int xoff = keyX - frame.x;
                        memcpy(&pixels[yoff * width + xoff], &frame.pixels[row * frame.width],
                               frame.width * sizeof(uint32_t));
                    }
                    char fileName[200];
                    sprintf(fileName, "%s/%s_%03d.tga", outDir.c_str(), hashStr, i);
                    stbi_write_tga(fileName, sp.m_header->width, sp.m_header->height, 4, pixels.data());
                }
                break;
            }
            case 2: // 单张 frame 尺寸
            {
                js[hashStr] = {
                    {"ID", hashStr},
                    {"GroupCount", sp.m_header->directionNum},
                    {"GroupFrameCount", sp.m_header->frameNum},
                    {"Times", sp.m_times},
                    {"Width", sp.m_header->width},
                    {"Height", sp.m_header->height},
                    {"KeyX", sp.m_header->x},
                    {"KeyY", sp.m_header->y},
                };

                for (int i = 0; i < sp.m_frames.size(); i++) {
                    auto &frame = sp.m_frames[i];
                    js[hashStr]["Frames"].emplace_back(nlohmann::json::object({
                        {"Width", sp.m_header->width}, {"Height", sp.m_header->height}, {"KeyX", sp.m_header->x},
                        {"KeyY", sp.m_header->y}
                    }));
                    char fileName[200];
                    sprintf(fileName, "%s/%s_%03d.tga", outDir.c_str(), hashStr, i);
                    stbi_write_tga(fileName, frame.width, frame.height, 4, frame.pixels.data());
                }
                break;
            }
            default: break;
        }
    }
    if (js.empty())
        return;
    std::fstream fs(outDir + "/list.json", std::ios::out);
    fs << js.dump();
    fs.close();
}
