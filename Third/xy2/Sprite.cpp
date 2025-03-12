#include "Sprite.h"

#include <iostream>

namespace XY2
{
    Sprite::Sprite()
    {
    }

    bool Sprite::load(char* data, size_t size)
    {
        m_times.clear();
        m_frames.clear();
        m_isValid = false;
        if (!data || !size)
            return m_isValid;

        m_header = (SpriteHeader*)data;

        int offset = sizeof(SpriteHeader);

        if (m_header->headSize > 12)
        {
            int timesSize = m_header->headSize - 12;
            m_times.resize(timesSize);

            memcpy(&m_times[0], data + offset, timesSize);
            offset += timesSize;
        }

        std::vector<uint16_t> palette16(256);
        memcpy(&palette16[0], data + offset, 256 * sizeof(uint16_t));
        offset += 256 * sizeof(uint16_t);

        std::vector<RGBA> palette32(256);
        for (int i = 0; i < 256; ++i)
        {
            RGB565ToRGBA8888(palette16[i], 255, palette32[i]);
        }


        int picNum = m_header->directionNum * m_header->frameNum;
        std::vector<uint32_t> picOffsets(picNum);
        memcpy(&picOffsets[0], data + offset, picNum * sizeof(uint32_t));
        offset += picNum * sizeof(uint32_t);
        for (int i = 0; i < picNum; ++i)
        {
            picOffsets[i] += 4 + m_header->headSize;
        }

        m_frames.resize(picNum);

        for (int i = 0; i < picNum; ++i)
        {
            int frameSize = i < picNum - 1
                                ? picOffsets[i + 1] - picOffsets[i]
                                : size - picOffsets[i];
            if (frameSize <= 0) {
                std::cout << "Frame Size Error" << std::endl;
                continue;
            }
            auto& frame = m_frames[i];
            if (!readFrame(data + offset, frameSize, palette16, palette32, frame))
                return m_isValid;
            offset += frameSize;
        }
        m_isValid = true;
        return m_isValid;
    }

    bool Sprite::readFrame(char* data, size_t size, const std::vector<uint16_t>& palette16, const std::vector<RGBA>& palette32, Frame& frame)
    {
        memcpy(&frame, data, offsetof(Frame, pixels));

        if (frame.height >= (1 << 15) || frame.width >= (1 << 15) || frame.height < 0 || frame.width < 0) {
            std::cerr << "Frame error  W: " << frame.width << " H: "<< frame.height << std::endl;
            return false;
        }

        uint32_t offset = offsetof(Frame, pixels);

        // std::vector<uint32_t> frameLineOffset(frame.height);
        // memcpy(frameLineOffset.data(), data + offset, sizeof(uint32_t) * frame.height);
        uint32_t *frameLineOffset = (uint32_t*)(data + offset);
        // offset += sizeof(uint32_t) * frame.height;

        frame.pixels.resize(frame.height * frame.width);
        // std::vector<RGBA> rgba(frame.height * frame.width);
        uint32_t pos = 0;
        uint16_t alphaPixel = 0;
        for (uint32_t h = 0; h < frame.height; h++)
        {
            uint32_t linePixels = 0;
            bool lineNotOver = true;
            if (frameLineOffset[h] >= size) {
                std::cerr << "Frame error" << std::endl;
                return false;
            }
            uint8_t* pData = (uint8_t*)data + frameLineOffset[h];

            while (*pData != 0 && lineNotOver)
            {
                uint8_t level = 0; // Alpha
                uint8_t repeat = 0; // 重复次数
                RGBA color; //重复颜色
                uint8_t style = (*pData & 0xc0) >> 6; // 取字节的前两个比特
                switch (style)
                {
                case 0: // {00******}
                    if (*pData & 0x20)
                    {
                        // {001*****} 表示带有Alpha通道的单个像素
                        // {001 +5bit Alpha}+{1Byte Index}, 表示带有Alpha通道的单个像素。
                        // {001 +0~31层Alpha通道}+{1~255个调色板引索}
                        level = (*pData) & 0x1f; // 0x1f=(11111) 获得Alpha通道的值
                        if (*(pData - 1) == 0xc0)
                        {
                            //特殊处理
                            //Level = 31;
                            //Pixels--;
                            //pos--;
                            if (linePixels <= frame.width)
                            {
                                frame.pixels[pos] = frame.pixels[pos - 1];
                                linePixels++;
                                pos++;
                                pData += 2;
                                break;
                            }
                            else
                            {
                                lineNotOver = false;
                            }
                        }
                        pData++; // 下一个字节
                        if (linePixels <= frame.width)
                        {
                            alphaPixel = Alpha565(palette16[*pData], 0, level);
                            RGB565ToRGBA8888(alphaPixel, level * 8, frame.pixels[pos]);
                            linePixels++;
                            pos++;
                            pData++;
                        }
                        else
                        {
                            lineNotOver = false;
                        }
                    }
                    else
                    {
                        // {000*****} 表示重复n次带有Alpha通道的像素
                        // {000 +5bit Times}+{1Byte Alpha}+{1Byte Index}, 表示重复n次带有Alpha通道的像素。
                        // {000 +重复1~31次}+{0~255层Alpha通道}+{1~255个调色板引索}
                        // 注: 这里的{00000000} 保留给像素行结束使用，所以只可以重复1~31次。
                        repeat = (*pData) & 0x1f; // 获得重复的次数
                        pData++;
                        level = *pData; // 获得Alpha通道值
                        pData++;
                        alphaPixel = Alpha565(palette16[*pData], 0, level);
                        RGB565ToRGBA8888(alphaPixel, level * 8, color);
                        for (int i = 1; i <= repeat; i++)
                        {
                            if (linePixels <= frame.width)
                            {
                                frame.pixels[pos] = color;
                                pos++;
                                linePixels++;
                            }
                            else
                            {
                                lineNotOver = false;
                            }
                        }
                        pData++;
                    }
                    break;
                case 1: // {01******} 表示不带Alpha通道不重复的n个像素组成的数据段
                    // {01  +6bit Times}+{nByte Datas},表示不带Alpha通道不重复的n个像素组成的数据段。
                        // {01  +1~63个长度}+{n个字节的数据},{01000000}保留。
                            repeat = (*pData) & 0x3f; // 获得数据组中的长度
                    pData++;
                    for (int i = 1; i <= repeat; i++)
                    {
                        if (linePixels <= frame.width)
                        {
                            frame.pixels[pos] = palette32[*pData];
                            pos++;
                            linePixels++;
                            pData++;
                        }
                        else
                        {
                            lineNotOver = false;
                        }
                    }
                    break;
                case 2: // {10******} 表示重复n次像素
                    // {10  +6bit Times}+{1Byte Index}, 表示重复n次像素。
                        // {10  +重复1~63次}+{0~255个调色板引索},{10000000}保留。
                            repeat = (*pData) & 0x3f; // 获得重复的次数
                    pData++;
                    color = palette32[*pData];
                    for (int i = 1; i <= repeat; i++)
                    {
                        if (linePixels <= frame.width)
                        {
                            frame.pixels[pos] = color;
                            pos++;
                            linePixels++;
                        }
                        else
                        {
                            lineNotOver = false;
                        }
                    }
                    pData++;
                    break;
                case 3: // {11******} 表示跳过n个像素，跳过的像素用透明色代替
                    // {11  +6bit Times}, 表示跳过n个像素，跳过的像素用透明色代替。
                        // {11  +跳过1~63个像素},{11000000}保留。
                            repeat = (*pData) & 0x3f; // 获得重复次数
                    if (repeat == 0)
                    {
                        if (linePixels <= frame.width)
                        {
                            //特殊处理
                            pos--;
                            linePixels--;
                        }
                        else
                        {
                            lineNotOver = false;
                        }
                    }
                    else
                    {
                        for (int i = 1; i <= repeat; i++)
                        {
                            if (linePixels <= frame.width)
                            {
                                pos++;
                                linePixels++;
                            }
                            else
                            {
                                lineNotOver = false;
                            }
                        }
                    }
                    pData++;
                    break;
                default: // 一般不存在这种情况
                    printf("Sprite ERROR\n");
                    break;
                }
            }
            if (*pData == 0 || !lineNotOver)
            {
                uint32_t repeat = frame.width - linePixels;
                if (repeat > 0)
                {
                    pos += repeat;
                }
            }
        }
        return true;
    }


    void Sprite::RGB565ToRGBA8888(uint16_t src, uint8_t alpha, RGBA& dst)
    {
        uint8_t r = (src >> 11) & 0x1f;
        uint8_t g = (src >> 5) & 0x3f;
        uint8_t b = (src) & 0x1f;

        dst.R = (r << 3) | (r >> 2);
        dst.G = (g << 2) | (g >> 4);
        dst.B = (b << 3) | (b >> 2);
        dst.A = alpha;
    }

    uint16_t Sprite::Alpha565(uint16_t Src, uint16_t Des, uint8_t Alpha) {
        uint16_t Result;
        // after mix = ( ( A-B ) * Alpha ) >> 5 + B
        // after mix = ( A * Alpha + B * ( 32-Alpha ) ) / 32

        unsigned short R_Src, G_Src, B_Src;
        R_Src = G_Src = B_Src = 0;

        R_Src = Src & 0xF800;
        G_Src = Src & 0x07E0;
        B_Src = Src & 0x001F;

        R_Src = R_Src >> 11;
        G_Src = G_Src >> 5;

        unsigned short R_Des, G_Des, B_Des;
        R_Des = G_Des = B_Des = 0;

        R_Des = Des & 0xF800;
        G_Des = Des & 0x07E0;
        B_Des = Des & 0x001F;

        R_Des = R_Des >> 11;
        G_Des = G_Des >> 5;

        unsigned short R_Res, G_Res, B_Res;
        R_Res = G_Res = B_Res = 0;

        R_Res = (((R_Src - R_Des) * Alpha) >> 5) + R_Des;
        G_Res = (((G_Src - G_Des) * Alpha) >> 5) + G_Des;
        B_Res = (((B_Src - B_Des) * Alpha) >> 5) + B_Des;

        R_Res = R_Res << 11;
        G_Res = G_Res << 5;

        Result = R_Res | G_Res | B_Res;
        return Result;
    }
}
