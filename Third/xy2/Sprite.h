#ifndef SPRITE_H
#define SPRITE_H
#include <vector>

namespace XY2
{
    struct SpriteHeader
    {
        uint16_t flag;
        int16_t headSize;
        int16_t directionNum;
        int16_t frameNum;
        int16_t width;
        int16_t height;
        int16_t x;
        int16_t y;
    };

    struct RGBA
    {
        uint8_t R{0};
        uint8_t G{0};
        uint8_t B{0};
        uint8_t A{0};
    };

    struct Frame
    {
        int32_t x{0};
        int32_t y{0};
        int32_t width;
        int32_t height;
        std::vector<RGBA> pixels;
    };

    class Sprite
    {
    public:
        Sprite();

        bool load(char* data, size_t size);

    private:
        bool readFrame(char* data, size_t size, const std::vector<uint16_t>& palette16, const std::vector<RGBA>& palette32, Frame& frame);

        void RGB565ToRGBA8888(uint16_t src, uint8_t alpha, RGBA& dst);

        uint16_t Alpha565(uint16_t Src, uint16_t Des, uint8_t Alpha);

    public:
        SpriteHeader* m_header;
        std::vector<uint8_t> m_times;
        std::vector<Frame> m_frames;
        bool m_isValid{false};
    };
}

#endif // SPRITE_H
