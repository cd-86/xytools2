#include <iostream>
#include <string>
#include <NESupport.h>
#include <stb_image_write.h>
#include <json.hpp>

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
        auto sp = wdf.LoadSprite(ind.hash);
        if (!sp)
        {
            continue;
        }
        js["WasList"][sp->ID] = {
            {"ID", sp->ID},
            {"GroupCount", sp->GroupCount},
            {"GroupFrameCount", sp->GroupFrameCount},
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
    }
    std::fstream fs(outDir + "/list.json", std::ios::out);
    fs << js.dump();
    fs.close();
}

int main()
{
    WdfTest("C:/Users/chend/Desktop/shape.wdf", "C:/Users/chend/Desktop/tga", 0);
}
