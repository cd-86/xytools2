#include <iostream>
#include <string>

#include <NESupport.h>
#include <stb_image_write.h>
#include <json.hpp>

int main()
{
    nlohmann::json js;
    NE::WDF wdf("C:/Users/chend/Desktop/shape.wdf");
    // std::cout << std::hex;
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
        // 1 合并
        // {
        //     int sheetWidth = sp->Width * sp->GroupFrameCount;
        //     int sheetHeight = sp->Height * sp->GroupCount;
        //     std::vector<uint32_t> pixels(sheetWidth * sheetHeight);
        //
        //     for (int i = 0; i < sp->GroupCount; ++i)
        //     {
        //         for (int j = 0; j < sp->GroupFrameCount; ++j)
        //         {
        //             auto& frame = sp->Frames[i * sp->GroupFrameCount + j];
        //             for (int row = 0; row < frame.Height; ++row)
        //             {
        //                 int index = (i * sp->Height + sp->KeyY - frame.KeyY + row) * sheetWidth + j * sp->Width + sp->
        //                     KeyX - frame.KeyX;
        //                 memcpy(&pixels[index], &frame.Src[row * frame.Width], frame.Width * sizeof(uint32_t));
        //             }
        //         }
        //     }
        //     std::string fileName = "C:/Users/chend/Desktop/tga/" + sp->ID + ".tga";
        //     stbi_write_tga(fileName.c_str(), sheetWidth, sheetHeight, 4, pixels.data());
        // }
        // 2 单张 sprite 尺寸
        // {
        //     for (int i = 0; i < sp->Frames.size(); i++)
        //     {
        //         std::vector<uint32_t> pixels(sp->Width * sp->Height);
        //         auto& frame = sp->Frames[i];
        //         for (int row = 0; row < frame.Height; ++row)
        //         {
        //             int index = (sp->KeyY - frame.KeyY + row) * sp->Width + sp->KeyX - frame.KeyX;
        //             memcpy(&pixels[index], &frame.Src[row * frame.Width], frame.Width * sizeof(uint32_t));
        //         }
        //         char fileName[200];
        //         sprintf(fileName, "C:/Users/chend/Desktop/tga/%s_%03d.tga", sp->ID.c_str(), i);
        //         stbi_write_tga(fileName, sp->Width, sp->Height, 4, pixels.data());
        //     }
        // }
        // 3 单张 frame 尺寸
        {
            for (int i = 0; i < sp->Frames.size(); i++)
            {
                auto& frame = sp->Frames[i];
                js["WasList"][sp->ID]["Frames"].emplace_back(nlohmann::json::object({{"Width", sp->Width}, {"Height", sp->Height}, {"KeyX", sp->KeyX}, {"KeyY", sp->KeyY}}));
                char fileName[200];
                sprintf(fileName, "C:/Users/chend/Desktop/tga/%s_%03d.tga", sp->ID.c_str(), i);
                stbi_write_tga(fileName, frame.Width, frame.Height, 4, frame.Src.data());
            }
        }
    }

    std::fstream fs("C:/Users/chend/Desktop/tga/shape.json", std::ios::out);
    fs << js.dump();
    fs.close();
    return 0;
}
