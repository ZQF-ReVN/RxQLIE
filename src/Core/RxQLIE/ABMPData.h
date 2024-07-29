#pragma once
#include <vector>
#include <cstdint>
#include <string_view>
#include <ZxMem/ZxMem.h>
#include <ZxJson/JValue.h>


namespace ZQF::RxQLIE
{
    class ABMPLayer
    {
    private:

    };


    class ABMPMotion
    {
    private:
        std::vector<ABMPLayer> m_vcData;
    };


    class ABMPData15
    {
    private:
        std::size_t m_nVersion{};
        ZxMem m_zmData;

    public:
        ABMPData15();
        ABMPData15(ZxMem& zmReader);

    public:
        auto Load(ZxMem& zmReader) -> void;
        auto Load(const std::string_view msDir, const ZxJson::JObject_t& rfJObject) -> void;
        auto Save(const std::string_view msSaveDir) const -> ZxJson::JObject_t;
        auto Make(ZxMem& zmWriter) const -> void;

    public:
        auto SizeBytes() const -> std::size_t;
    };
}
