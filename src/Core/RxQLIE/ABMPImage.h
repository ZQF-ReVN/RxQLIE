#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <ZxMem/ZxMem.h>
#include <ZxJson/JValue.h>


namespace ZQF::RxQLIE
{
    enum class ABMPImageDataType : uint8_t
    {
        BMP = 0, // .bmp
        JPG = 1, // .jpg
        PNG0 = 2, // .png
        PNG1 = 3, // .png
        M = 4, // .m
        ARGB = 5, // .argb
        B = 6, // .b
        OGV = 7, // .ogg
        MDL = 8 // .mdl
    };


    class ABMPImageData15
    {
    private:
        size_t m_nVirtualFlag{};
        std::u16string m_u16FileName;
        std::string m_msHashName;
        ABMPImageDataType m_eType{};
        size_t m_nOffsetX{};
        size_t m_nOffsetY{};
        size_t m_nOffsetZ{};
        size_t m_nVirtualWidth{};
        size_t m_nVirtualHeigh{};
        size_t m_nVirtualDepth{};
        size_t m_nRenderingTextureMode{};
        size_t m_nRenderingTextureBGColor{};
        ZxMem m_zmData;

    public:
        ABMPImageData15();
        ABMPImageData15(ZxMem& zmReader);
        ABMPImageData15(const std::string_view msDir, ZxJson::JObject_t& rfJObject);

    public:
        auto Load(ZxMem& zmReader) -> void;
        auto Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void;
        auto Save(const std::string_view msSaveDir) const->ZxJson::JObject_t;
        auto Make(ZxMem& zmWriter) const -> void;

    public:
        auto SizeBytes() const->size_t;
        auto GetSuffix() const->std::string_view;
    };


    class ABMPImage10
    {
    private:
        std::vector<ABMPImageData15> m_vcData;

    public:
        ABMPImage10();
        ABMPImage10(ZxMem& zmReader);

    public:
        auto Load(ZxMem& zmReader) -> void;
        auto Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void;
        auto Save(const std::string_view msSaveDir) const->ZxJson::JObject_t;
        auto Make(ZxMem& zmWriter) const -> void;

    public:
        auto SizeBytes() const->size_t;
    };
}
