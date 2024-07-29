#include <RxQLIE/ABMPData.h>
#include <RxQLIE/ABMPUtility.h>
#include <ZxFile/ZxFile.h>
#include <array>


namespace ZQF::RxQLIE
{
    ABMPData15::ABMPData15()
    {

    }

    ABMPData15::ABMPData15(ZxMem& zmReader)
    {
        this->Load(zmReader);
    }

    auto ABMPData15::Load(ZxMem& zmReader) -> void
    {
        auto cur_sig = zmReader.Get<std::array<char, 16>>();
        for (auto& c : cur_sig) { c = static_cast<char>(std::tolower(c)); }
        if (std::memcmp(cur_sig.data(), "abdata", 6))
        {
            throw std::runtime_error("ABMPData::Load(): error signature");
        }

        char* end_ptr{};
        m_nVersion = static_cast<std::size_t>(std::strtol(&cur_sig[6], &end_ptr, 10));

        constexpr std::array<std::size_t, 3> allow_version{ 15, 14, 13 };
        if (auto it = std::find(std::cbegin(allow_version), std::cend(allow_version), m_nVersion); it == std::cend(allow_version))
        {
            throw std::runtime_error("ABMPData::Load(): error signature");
        }

        const auto data_size = zmReader.Get<std::uint32_t>();
        const auto data_ptr = zmReader.PtrCur<std::uint8_t*>();
        m_zmData.Resize(data_size);
        m_zmData.Put(std::span{ data_ptr, data_size });
        zmReader.PosInc(data_size);
    }

    auto ABMPData15::Load(const std::string_view msDir, const ZxJson::JObject_t& rfJObject) -> void
    {
        m_nVersion = rfJObject.at("version").GetNum<decltype(m_nVersion)>();
        m_zmData.Load(std::string{ msDir }.append("abdata.bin"));
    }

    auto ABMPData15::Save(const std::string_view msSaveDir) const -> ZxJson::JObject_t
    {
        ZxFile::SaveDataViaPath(std::string{ msSaveDir }.append("abdata.bin"), m_zmData.Span(), true, true);

        return ZxJson::JObject_t
        {
            { "version", m_nVersion },
            { "data_size", m_zmData.SizeBytes() }
        };
    }

    auto ABMPData15::Make(ZxMem& zmWriter) const -> void
    {
        auto sig = std::array<char, 16>{ "abdata" };
        auto v = std::to_string(m_nVersion);
        std::memcpy(&sig[6], v.data(), v.size());

        zmWriter
            .Put(sig)
            .Put(static_cast<std::uint32_t>(m_zmData.SizeBytes()))
            .Put(m_zmData.Span());
    }

    auto ABMPData15::SizeBytes() const -> std::size_t
    {
        std::size_t size = 16; // signature
        size += sizeof(std::uint32_t); // data_size
        size += m_zmData.SizeBytes();
        return size;
    }
}
