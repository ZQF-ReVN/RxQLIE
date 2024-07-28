#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <ZxMem/ZxMem.h>
#include <ZxJson/JValue.h>


namespace ZQF::RxQLIE
{
    enum class ABMPSoundDataType : uint8_t
    {
        WAV = 0,
        OGG = 1
    };


    class ABMPSoundData12
    {
    private:
        size_t m_nFlag{};
        std::u16string m_u16FileName;
        std::string m_msHashName;
        ABMPSoundDataType m_eType{};
        ZxMem m_zmData;

    public:
        ABMPSoundData12();
        ABMPSoundData12(ZxMem& zmReader);
        ABMPSoundData12(const std::string_view msDir, ZxJson::JObject_t& rfJObject);

    public:
        auto Load(ZxMem& zmReader) -> void;
        auto Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void;
        auto Save(const std::string_view msSaveDir) const->ZxJson::JObject_t;
        auto Make(ZxMem& zmWriter) const -> void;

    public:
        auto SizeBytes() const->size_t;
        auto GetSuffix() const->std::string_view;
    };


    class ABMPSound10
    {
    private:
        std::vector<ABMPSoundData12> m_vcData;

    public:
        ABMPSound10();
        ABMPSound10(ZxMem& zmReader);

    public:
        auto Load(ZxMem& zmReader) -> void;
        auto Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void;
        auto Save(const std::string_view msSaveDir) const->ZxJson::JObject_t;
        auto Make(ZxMem& zmWriter) const -> void;

    public:
        auto SizeBytes() const->size_t;
    };
}
