#include "wdf.h"
#include <fstream>
#include <iostream>

// // PFDW
// #define WDFP 0x57444650

namespace XY2
{
    Wdf::Wdf() {
    }

    Wdf::Wdf(const std::string &path) {
        load(path);
    }

    Wdf::~Wdf() {
    }

    bool Wdf::load(const std::string &path) {
        m_path = path;
        m_isValid = false;
        m_wasInfos = nullptr;
        m_header = nullptr;
        m_wdfData.clear();
        m_wasMap.clear();

        std::fstream file(path, std::ios::binary | std::ios::in);
        if (!file.is_open()) {
            std::cerr << "Failed to open file " << path << std::endl;
            return m_isValid;
        }

        std::cout << "WDF init: " << path << std::endl;

        file.seekg(0, std::ios::end);
        int fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        m_wdfData.resize(fileSize);
        file.read(m_wdfData.data(), m_wdfData.size());
        file.close();

        m_header = (WdfHeader*)m_wdfData.data();
        if (m_header->flag != 'WDFP') {
            std::cerr << "File is not a WDF file" << std::endl;
            return m_isValid;
        }

        int offset = m_header->offset;
        m_wasInfos = (WasInfo*)(m_wdfData.data() + offset);

        for (int i = 0; i < m_header->wasCount; ++i)
        {
            m_wasMap[m_wasInfos[i].hash] = i;
        }

        // offset += m_header->wasCount * sizeof(WasInfo);

        m_isValid = true;
        std::cout << "WDF init success!" << std::endl;
        return m_isValid;
    }

    FileType Wdf::check_file_type(char* data, size_t size)
    {
        // SP
        if (*(uint16_t *) data == 'PS') { return FT_SP; }

        if (*(uint32_t *) data == 'FFIR') {
            if (*(uint32_t *) (data + 8) == 'EVAW') // Wav
                return FT_WAV;
            if (*(uint32_t *) (data + 8) == 'NOCA') // ani
                return FT_ACON;
        }
        // Oggs
        if (*(uint32_t *) data == 'SggO') { return FT_OGGS; }
        // tga
        const char *tgaFlag = "TRUEVISION-XFILE.";
        int off = size - strlen(tgaFlag) - 1;
        if (strncmp(data + off, tgaFlag, strlen(tgaFlag)) == 0) { return FT_TGA; }
        // PNG
        if (*(uint32_t *) data & 0xFFFFFF == 'GNP') { return FT_PNG; }
        // JPEG
        if (*(uint32_t *) data == 0xE0FFD8FF) { return FT_JPEG; }
        // BMP
        if (*(uint16_t *) data == 'MB') { return FT_BMP; }
        // TEXT
        if (strlen(data) == size) { return FT_TEXT; }
        // Mp3
        if (*(int*)data == 0xff && *(int*)(data + 16) == 0x7) {
            return FT_MP3;
        }
        // fsb4
        if (*(uint32_t *) data == '4BSF') { return FT_FSB4; }
        // rar
        if (*(uint32_t *) data == '!raR') { return FT_RAR; }
        return FT_UNKNOWN;
    }

    char* Wdf::getWas(int index, int* size)
    {
        if (!m_isValid || index < 0 || index >= m_header->wasCount)
        {
            *size = 0;
            return nullptr;
        }
        *size = m_wasInfos[index].size;
        return m_wdfData.data() + m_wasInfos[index].offset;
    }

    char* Wdf::getWas(uint32_t hash, int* size)
    {
        if (!m_isValid || m_wasMap.find(hash) == m_wasMap.end())
        {
            *size = 0;
            return nullptr;
        }
        *size = m_wasInfos[m_wasMap[hash]].size;
        return m_wdfData.data() + m_wasInfos[m_wasMap[hash]].offset;
    }
}
