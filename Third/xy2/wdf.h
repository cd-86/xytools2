#ifndef WDF_H
#define WDF_H
#include <string>
#include <unordered_map>
#include <vector>

namespace XY2
{
    enum FileType
    {
        FT_UNKNOWN,
        FT_SP, // Sprite
        FT_PK,

        FT_MP3,
        FT_WAV,
        FT_OGGS,
        FT_FSB4, // fmod

        FT_JPEG,
        FT_TGA,
        FT_PNG,
        FT_BMP,

        FT_ACON, // ani

        FT_TEXT,

        FT_RAR, // rar
    };

    struct WdfHeader
    {
        uint32_t flag;
        uint32_t wasCount;
        uint32_t offset;
    };

    struct WasInfo
    {
        uint32_t hash;
        uint32_t offset;
        uint32_t size;
        uint32_t spaces;
    };


    class Wdf
    {
    public:
        Wdf();
        Wdf(const std::string& path);
        ~Wdf();
        bool load(const std::string& path);

        FileType check_file_type(char* data, size_t size);

        char* getWas(int index, int *size);
        char* getWas(uint32_t hash, int *size);

    public:
        std::string m_path;
        std::vector<char> m_wdfData;
        WdfHeader* m_header{nullptr};
        WasInfo* m_wasInfos;
        std::unordered_map<uint32_t, int> m_wasMap;
        bool m_isValid{false};
    };
}
#endif //WDF_H
