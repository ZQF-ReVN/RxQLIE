#include <RxQLIE/ABMPSound.h>
#include <RxQLIE/ABMPUtility.h>
#include <ZxCvt/ZxCvt.h>
#include <ZxFile/ZxFile.h>
#include <ranges>


namespace ZQF::RxQLIE
{
    ABMPSoundData12::ABMPSoundData12()
    {

    }

    ABMPSoundData12::ABMPSoundData12(ZxMem& zmReader)
    {
        this->Load(zmReader);
    }

    ABMPSoundData12::ABMPSoundData12(const std::string_view msDir, ZxJson::JObject_t& rfJObject)
    {
        this->Load(msDir, rfJObject);
    }

    auto ABMPSoundData12::Load(ZxMem& zmReader) -> void
    {
        auto sig = zmReader.Get<std::array<char, 16>>();
        RxQLIE::CheckSignature(sig, "absnddat12");

        m_nFlag = zmReader.Get<uint32_t>();

        if (m_nFlag > 1)
        {
            throw std::runtime_error("error");
        }

        m_u16FileName = RxQLIE::DelphiStrView<char16_t>(zmReader);
        m_msHashName = RxQLIE::DelphiStrView<char>(zmReader);
        m_eType = static_cast<ABMPSoundDataType>(zmReader.Get<uint8_t>());

        const auto data_size = zmReader.Get<uint32_t>();
        const auto data_ptr = zmReader.PtrCur<uint8_t*>();
        m_zmData.Resize(data_size);
        m_zmData.Put(std::span{ data_ptr, data_size });
        zmReader.PosInc(data_size);
    }

    auto ABMPSoundData12::Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void
    {
        if (rfJObject["version"].Get<size_t>() != 12)
        {
            throw std::runtime_error("error version");
        }

        auto file_name = rfJObject["file_name"].Get<std::string_view>();

        ZxCvt cvt;
        m_nFlag = rfJObject["flag"].Get<decltype(m_nFlag)>();
        m_u16FileName = cvt.UTF8ToUTF16LE(file_name);
        m_msHashName = cvt.UTF8ToMBCS(rfJObject["hash_name"].Get<std::string_view>(), 932);
        m_eType = static_cast<ABMPSoundDataType>(rfJObject["data_type"].Get<std::underlying_type_t<ABMPSoundDataType>>());

        if (rfJObject["data_size"].Get<size_t>())
        {
            m_zmData.Load(std::string{ msDir }.append(file_name).append(this->GetSuffix()));
        }
    }

    auto ABMPSoundData12::Save(const std::string_view msSaveDir) const -> ZxJson::JObject_t
    {
        ZxCvt cvt;

        auto file_suffix = this->GetSuffix();
        auto file_name_u8 = cvt.UTF16LEToUTF8(m_u16FileName);

        if (m_zmData.SizeBytes())
        {
            ZxFile::SaveDataViaPath(std::string{ msSaveDir }.append(file_name_u8).append(file_suffix), m_zmData.Span(), true);
        }

        ZxJson::JObject_t json;
        json["version"] = 12;
        json["flag"] = m_nFlag;
        json["file_name"] = file_name_u8;
        json["hash_name"] = cvt.MBCSToUTF8(m_msHashName, 932);
        json["data_type"] = static_cast<uint8_t>(m_eType);
        json["data_size"] = m_zmData.SizeBytes();
        return json;
    }

    auto ABMPSoundData12::Make(ZxMem& zmWriter) const -> void
    {
        zmWriter
            .Put(std::array<char, 16>{"absnddat12"})
            .Put(static_cast<uint32_t>(m_nFlag))
            .Put(static_cast<uint16_t>(m_u16FileName.size()))
            .Put(std::span{ m_u16FileName })
            .Put(static_cast<uint16_t>(m_msHashName.size()))
            .Put(std::span{ m_msHashName })
            .Put(static_cast<uint8_t>(m_eType))
            .Put(static_cast<uint32_t>(m_zmData.SizeBytes()))
            .Put(m_zmData.Span());
    }

    auto ABMPSoundData12::SizeBytes() const -> size_t
    {
        size_t size = 16; // signature
        size += sizeof(uint32_t); // flag
        size += sizeof(uint16_t) + m_u16FileName.length() * sizeof(char16_t); // file_name
        size += sizeof(uint16_t) + m_msHashName.length() * sizeof(char); // hash_name
        size += sizeof(uint8_t); // type
        size += sizeof(uint32_t); // data_size
        size += m_zmData.SizeBytes(); // data binary size
        return size;
    }

    auto ABMPSoundData12::GetSuffix() const -> std::string_view
    {
        switch (m_eType)
        {
        case ABMPSoundDataType::WAV: return ".wav";
        case ABMPSoundDataType::OGG: return ".ogg";
        default: throw std::runtime_error("error");
        }
    }

    ABMPSound10::ABMPSound10()
    {

    }

    ABMPSound10::ABMPSound10(ZxMem& zmReader)
    {
        this->Load(zmReader);
    }

    auto ABMPSound10::Load(ZxMem& zmReader) -> void
    {
        auto sig = zmReader.Get<std::array<char, 16>>();
        RxQLIE::CheckSignature(sig, "absound10");

        for (auto snddat_cnt = zmReader.Get<uint8_t>(); [[maybe_unused]] auto _ : std::views::iota(0u, snddat_cnt))
        {
            m_vcData.emplace_back(zmReader);
        }
    }

    auto ABMPSound10::Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void
    {
        if (rfJObject["version"].Get<size_t>() != 10)
        {
            throw std::runtime_error("error version");
        }

        for (auto& info : rfJObject["absnddat_list"].Sure<ZxJson::JArray_t&>())
        {
            m_vcData.emplace_back(msDir, info.Sure<ZxJson::JObject_t&>());
        }
    }

    auto ABMPSound10::Save(const std::string_view msSaveDir) const -> ZxJson::JObject_t
    {
        ZxJson::JObject_t json;
        json["version"] = 10;
        json["conut"] = m_vcData.size();

        auto& array = json["absnddat_list"].Sure<ZxJson::JArray_t&>();
        for (const auto& snd_dat : m_vcData)
        {
            array.emplace_back(snd_dat.Save(msSaveDir));
        }

        return json;
    }

    auto ABMPSound10::Make(ZxMem& zmWriter) const -> void
    {
        zmWriter << std::array<char, 16>{ "absound10" } << static_cast<uint8_t>(m_vcData.size());

        for (const auto& data : m_vcData)
        {
            data.Make(zmWriter);
        }
    }

    auto ABMPSound10::SizeBytes() const -> size_t
    {
        size_t size = 16; // signature
        size += sizeof(uint8_t); // snddat_count
        for (const auto& snd_dat : m_vcData)
        {
            size += snd_dat.SizeBytes();
        }
        return size;
    }
}
