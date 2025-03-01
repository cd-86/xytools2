#include <iostream>
#include <string>
#include <NESupport.h>
#include <stb_image_write.h>
#include <json.hpp>


/**
 * 导出精灵表或单张精灵图
 *
 * @param wdfPath Wdf 路径
 * @param outDir 输出目录
 * @param mode 输出文件模式 \n 0： 合并为一张精灵表保存 \n 1：精灵尺寸保存每一张精灵图 \n 2：原始尺寸保存每一张精灵图
 */
void WdfTest(const std::string& wdfPath, const std::string& outDir, int mode)
{
    nlohmann::json js;
    NE::WDF wdf(wdfPath);
    if (!wdf.m_FileType)
        return;

    if (!std::filesystem::exists(outDir) || !std::filesystem::is_directory(outDir))
    {
        std::filesystem::create_directory(outDir);
    }
    for (auto& ind : wdf.mIndencies)
    {
        auto wasType = wdf.GetWasType(ind.hash);
        std::cout << ind.hash << " " << wasType << std::endl;
        if (wasType != NE::eFILE_TYPE_SPRITE) {
            std::string suffix;
            switch (wdf.GetWasType(ind.hash)) {
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
                    suffix = ".text";
                break;
                case NE::eFILE_TYPE_RAR: // rar
                    suffix = ".rar";
                break;
                case NE::eFILE_TYPE_UNKNOWN:
                    default: ;
            }
            uint8_t *data;
            size_t size;
            wdf.LoadFileData(ind.hash, data, size);
            std::string fileName = outDir + "/" + std::to_string(ind.hash) + suffix;
            std::fstream fs(fileName, std::ios::out | std::ios::binary);
            fs.write((char*)data, size);
            continue;
        }
        auto sp = wdf.LoadSprite(ind.hash);
        if (!sp)
        {
            continue;
        }
        js["WasList"][sp->ID] = {
            {"ID", sp->ID},
            {"GroupCount", sp->GroupCount},
            {"GroupFrameCount", sp->GroupFrameCount},
            {"Times", sp->Times},
            {"Width", sp->Width},
            {"Height", sp->Height},
            {"KeyX", sp->KeyX},
            {"KeyY", sp->KeyY},
        };
        switch (mode)
        {
        case 0: // 合并
            {
                int sheetWidth = sp->Width * sp->GroupFrameCount;
                int sheetHeight = sp->Height * sp->GroupCount;
                std::vector<uint32_t> pixels(sheetWidth * sheetHeight);

                for (int i = 0; i < sp->GroupCount; ++i)
                {
                    for (int j = 0; j < sp->GroupFrameCount; ++j)
                    {
                        auto& frame = sp->Frames[i * sp->GroupFrameCount + j];
                        int frameWidth = frame.Width;
                        int frameHeight = frame.Height;
                        if (int w = sp->KeyX - frame.KeyX + frame.Width - sp->Width; w > 0)
                        {
                            frameWidth -= w;
                        }
                        if (int h = sp->KeyY - frame.KeyY + frame.Height - sp->Height; h > 0)
                        {
                            frameHeight -= h;
                        }
                        for (int row = 0; row < frameHeight; ++row)
                        {
                            int yoff = sp->KeyY - frame.KeyY + row;
                            if (yoff < 0)
                            {
                                // 裁剪
                                continue;
                            }
                            int xoff = sp->KeyX - frame.KeyX;
                            if (xoff < 0)
                            {
                                // 裁剪
                                int index = (i * sp->Height + yoff) * sheetWidth + j * sp->Width;
                                memcpy(&pixels[index], &frame.Src[row * frame.Width - xoff], (frameWidth + xoff) * sizeof(uint32_t));
                            }
                            else
                            {
                                int index = (i * sp->Height + yoff) * sheetWidth + j * sp->Width + xoff;
                                memcpy(&pixels[index], &frame.Src[row * frame.Width], frameWidth * sizeof(uint32_t));
                            }
                        }
                    }
                }
                std::string fileName = outDir + "/" + sp->ID + ".tga";
                stbi_write_tga(fileName.c_str(), sheetWidth, sheetHeight, 4, pixels.data());
                break;
            }
        case 1: // 单张 sprite 尺寸
            {
                for (int i = 0; i < sp->Frames.size(); i++)
                {
                    std::vector<uint32_t> pixels(sp->Width * sp->Height);
                    auto& frame = sp->Frames[i];
                    int frameWidth = frame.Width;
                    int frameHeight = frame.Height;
                    if (int w = sp->KeyX - frame.KeyX + frame.Width - sp->Width; w > 0)
                    {
                        // std::cout << "Cutting width: " << ind.hash << ", " << w << std::endl;
                        frameWidth -= w;
                    }
                    if (int h = sp->KeyY - frame.KeyY + frame.Height - sp->Height; h > 0)
                    {
                        // std::cout << "Cutting height: " << ind.hash << ", " << h << std::endl;
                        frameHeight -= h;
                    }
                    for (int row = 0; row < frameHeight; ++row)
                    {
                        int yoff = sp->KeyY - frame.KeyY + row;
                        if (yoff < 0)
                        {
                            // 裁剪
                            // std::cout << "Cutting yoff: " << ind.hash << ", " << yoff << std::endl;
                            continue;
                        }
                        int xoff = sp->KeyX - frame.KeyX;
                        if (xoff < 0)
                        {
                            // 裁剪
                            // std::cout << "Cutting xoff: " << ind.hash << ", " << xoff << std::endl;
                            memcpy(&pixels[yoff * sp->Width], &frame.Src[row * frame.Width - xoff],
                                   (frameWidth + xoff) * sizeof(uint32_t));
                        }
                        else
                        {
                            memcpy(&pixels[yoff * sp->Width + xoff], &frame.Src[row * frame.Width],
                                   frameWidth * sizeof(uint32_t));
                        }
                    }
                    char fileName[200];
                    sprintf(fileName, "%s/%s_%03d.tga", outDir.c_str(), sp->ID.c_str(), i);
                    stbi_write_tga(fileName, sp->Width, sp->Height, 4, pixels.data());
                }
                break;
            }
        case 2: // 单张 frame 尺寸
            {
                for (int i = 0; i < sp->Frames.size(); i++)
                {
                    auto& frame = sp->Frames[i];
                    js["WasList"][sp->ID]["Frames"].emplace_back(nlohmann::json::object({
                        {"Width", sp->Width}, {"Height", sp->Height}, {"KeyX", sp->KeyX}, {"KeyY", sp->KeyY}
                    }));
                    char fileName[200];
                    sprintf(fileName, "%s/%s_%03d.tga", outDir.c_str(), sp->ID.c_str(), i);
                    stbi_write_tga(fileName, frame.Width, frame.Height, 4, frame.Src.data());
                }
                break;
            }
        default: break;
        }
        wdf.UnLoadSprite(ind.hash);
    }
    std::fstream fs(outDir + "/list.json", std::ios::out);
    fs << js.dump();
    fs.close();
}

int main(int argc, char* argv[])
{
    WdfTest("C:/games/dhxy/gires.wdf", "C:/games/dhxy/gires", 0);
}
