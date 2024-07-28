#include <RxQLIE/ABMPImage.h>
#include <RxQLIE/ABMPUtility.h>
#include <ZxCvt/ZxCvt.h>
#include <ZxFile/ZxFile.h>
#include <ranges>


namespace ZQF::RxQLIE
{
    ABMPImageData15::ABMPImageData15()
    {

    }

    ABMPImageData15::ABMPImageData15(ZxMem& zmReader)
    {
        this->Load(zmReader);
    }

    ABMPImageData15::ABMPImageData15(const std::string_view msDir, ZxJson::JObject_t& rfJObject)
    {
        this->Load(msDir, rfJObject);
    }

    auto ABMPImageData15::Load(ZxMem& zmReader) -> void
    {
        auto sig = zmReader.Get<std::array<char, 16>>();
        RxQLIE::CheckSignature(sig, "abimgdat15");

        m_nVirtualFlag = zmReader.Get<uint32_t>();
        m_u16FileName = RxQLIE::DelphiStrView<char16_t>(zmReader);
        m_msHashName = RxQLIE::DelphiStrView<char>(zmReader);
        m_eType = static_cast<ABMPImageDataType>(zmReader.Get<uint8_t>());
        m_nOffsetX = zmReader.Get<uint32_t>();
        m_nOffsetY = zmReader.Get<uint32_t>();
        m_nOffsetZ = zmReader.Get<uint32_t>();

        if (m_nVirtualFlag > 2)
        {
            throw std::runtime_error("ABMPImageData15::Load: error virtual flag");
        }

        if (m_nVirtualFlag >= 2)
        {
            m_nVirtualWidth = zmReader.Get<uint32_t>();
            m_nVirtualHeigh = zmReader.Get<uint32_t>();
            m_nVirtualDepth = zmReader.Get<uint32_t>();
        }

        m_nRenderingTextureMode = zmReader.Get<uint8_t>();
        m_nRenderingTextureBGColor = zmReader.Get<uint32_t>();

        const auto data_size = zmReader.Get<uint32_t>();
        const auto data_ptr = zmReader.PtrCur<uint8_t*>();
        m_zmData.Resize(data_size);
        m_zmData.Put(std::span{ data_ptr, data_size });
        zmReader.PosInc(data_size);
    }

    auto ABMPImageData15::Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void
    {
        if (rfJObject["version"].Get<size_t>() != 15)
        {
            throw std::runtime_error("ABMPImageData15::Load(): error version");
        }

        const auto file_name = rfJObject["file_name"].Get<std::string_view>();

        ZxCvt cvt;
        m_nVirtualFlag = rfJObject["virtual_flag"].Get<decltype(m_nVirtualFlag)>();
        m_u16FileName = cvt.UTF8ToUTF16LE(file_name);
        m_msHashName = cvt.UTF8ToMBCS(rfJObject["hash_name"].Get<std::string_view>(), 932);
        m_eType = static_cast<ABMPImageDataType>(rfJObject["data_type"].Get<std::underlying_type_t<ABMPImageDataType>>());

        m_nOffsetX = rfJObject["offset_x"].Get<decltype(m_nOffsetX)>();
        m_nOffsetY = rfJObject["offset_y"].Get<decltype(m_nOffsetY)>();
        m_nOffsetZ = rfJObject["offset_z"].Get<decltype(m_nOffsetZ)>();

        if (m_nVirtualFlag > 2)
        {
            throw std::runtime_error("ABMPImageData15::Load: error virtual flag");
        }

        if (m_nVirtualFlag >= 2)
        {
            m_nVirtualWidth = rfJObject["virtual_width"].Get<decltype(m_nVirtualWidth)>();
            m_nVirtualHeigh = rfJObject["virtual_heigh"].Get<decltype(m_nVirtualHeigh)>();
            m_nVirtualDepth = rfJObject["virtual_depth"].Get<decltype(m_nVirtualDepth)>();
        }

        m_nRenderingTextureMode = rfJObject["rendering_texture_mode"].Get<decltype(m_nRenderingTextureMode)>();
        m_nRenderingTextureBGColor = rfJObject["rendering_texture_bg_color"].Get<decltype(m_nRenderingTextureBGColor)>();

        if (rfJObject["data_size"].Get<size_t>())
        {
            std::string save_path{ msDir };
            if (file_name.starts_with('*'))
            {
                save_path.append(1, '_');
                save_path.append(file_name.substr(1));
            }
            else
            {
                save_path.append(file_name);
            }

            save_path.append(this->GetSuffix());
            m_zmData.Load(save_path);
        }
    }

    auto ABMPImageData15::Save(const std::string_view msSaveDir) const -> ZxJson::JObject_t
    {
        ZxCvt cvt;

        auto file_suffix = this->GetSuffix();
        auto file_name_u8 = cvt.UTF16LEToUTF8(m_u16FileName);

        if (m_zmData.SizeBytes())
        {
            std::string save_path{ msSaveDir };
            if (file_name_u8.starts_with('*'))
            {
                save_path.append(1, '_');
                save_path.append(file_name_u8.substr(1));
            }
            else
            {
                save_path.append(file_name_u8);
            }

            save_path.append(file_suffix);
            ZxFile::SaveDataViaPath(save_path, m_zmData.Span(), true);
        }

        ZxJson::JObject_t json;
        json["version"] = 15;
        json["virtual_flag"] = m_nVirtualFlag;
        json["file_name"] = file_name_u8;
        json["hash_name"] = cvt.MBCSToUTF8(m_msHashName, 932);
        json["data_type"] = static_cast<uint8_t>(m_eType);
        json["offset_x"] = m_nOffsetX;
        json["offset_y"] = m_nOffsetY;
        json["offset_z"] = m_nOffsetZ;
        json["virtual_width"] = m_nVirtualWidth;
        json["virtual_heigh"] = m_nVirtualHeigh;
        json["virtual_depth"] = m_nVirtualDepth;
        json["rendering_texture_mode"] = m_nRenderingTextureMode;
        json["rendering_texture_bg_color"] = m_nRenderingTextureBGColor;
        json["data_size"] = m_zmData.SizeBytes();
        return json;
    }

    auto ABMPImageData15::Make(ZxMem& zmWriter) const -> void
    {
        zmWriter
            .Put(std::array<char, 16>{ "abimgdat15" })
            .Put(static_cast<uint32_t>(m_nVirtualFlag))
            .Put(static_cast<uint16_t>(m_u16FileName.size()))
            .Put(std::span{ m_u16FileName })
            .Put(static_cast<uint16_t>(m_msHashName.size()))
            .Put(std::span{ m_msHashName })
            .Put(static_cast<uint8_t>(m_eType))
            .Put(static_cast<uint32_t>(m_nOffsetX))
            .Put(static_cast<uint32_t>(m_nOffsetY))
            .Put(static_cast<uint32_t>(m_nOffsetZ));

        if (m_nVirtualFlag >= 2)
        {
            zmWriter
                .Put(static_cast<uint32_t>(m_nVirtualWidth))
                .Put(static_cast<uint32_t>(m_nVirtualHeigh))
                .Put(static_cast<uint32_t>(m_nVirtualDepth));
        }

        zmWriter
            .Put(static_cast<uint8_t>(m_nRenderingTextureMode))
            .Put(static_cast<uint32_t>(m_nRenderingTextureBGColor))
            .Put(static_cast<uint32_t>(m_zmData.SizeBytes()))
            .Put(m_zmData.Span());
    }

    auto ABMPImageData15::SizeBytes() const -> size_t
    {
        size_t size = 16;
        size += sizeof(uint32_t); // virtual_flag
        size += sizeof(uint16_t) + m_u16FileName.length() * sizeof(char16_t); // file_name
        size += sizeof(uint16_t) + m_msHashName.length() * sizeof(char); // hash_name
        size += sizeof(uint8_t); // type
        size += sizeof(uint32_t); // offset_x
        size += sizeof(uint32_t); // offset_y
        size += sizeof(uint32_t); // offset_z

        if (m_nVirtualFlag >= 2)
        {
            size += sizeof(uint32_t); // virtual_width
            size += sizeof(uint32_t); // virtual_height
            size += sizeof(uint32_t); // virtual_depth
        }

        size += sizeof(uint8_t); // rendering_texture_mode
        size += sizeof(uint32_t); // rendering_texture_bg_color
        size += sizeof(uint32_t); // data_size
        size += m_zmData.SizeBytes(); // data binary size
        return size;
    }

    auto ABMPImageData15::GetSuffix() const -> std::string_view
    {
        switch (m_eType)
        {
        case ABMPImageDataType::BMP: return ".bmp";
        case ABMPImageDataType::JPG: return ".jpg";
        case ABMPImageDataType::PNG0: return ".png";
        case ABMPImageDataType::PNG1: return ".png";
        case ABMPImageDataType::M: return ".m";
        case ABMPImageDataType::ARGB: return ".argb";
        case ABMPImageDataType::B: return ".b";
        case ABMPImageDataType::OGV: return ".ogv";
        case ABMPImageDataType::MDL: return ".mdl";
        default: throw std::runtime_error("error");
        }
    }



    ABMPImage10::ABMPImage10()
    {

    }


    ABMPImage10::ABMPImage10(ZxMem& zmReader)
    {
        this->Load(zmReader);
    }

    auto ABMPImage10::Load(ZxMem& zmReader) -> void
    {
        auto sig = zmReader.Get<std::array<char, 16>>();
        RxQLIE::CheckSignature(sig, "abimage10");

        for (auto imgdat_cnt = zmReader.Get<uint8_t>(); [[maybe_unused]] auto _ : std::views::iota(0u, imgdat_cnt))
        {
            m_vcData.emplace_back(zmReader);
        }
    }

    auto ABMPImage10::Load(const std::string_view msDir, ZxJson::JObject_t& rfJObject) -> void
    {
        if (rfJObject["version"].Get<size_t>() != 10)
        {
            throw std::runtime_error("ABMPImage10::Load(): error version");
        }

        for (auto& info : rfJObject["abimgdat_list"].Sure<ZxJson::JArray_t&>())
        {
            m_vcData.emplace_back(msDir, info.Sure<ZxJson::JObject_t&>());
        }
    }

    auto ABMPImage10::Save(const std::string_view msSaveDir) const -> ZxJson::JObject_t
    {
        ZxJson::JObject_t json;
        json["version"] = 10;
        json["conut"] = m_vcData.size();
        auto& array = json["abimgdat_list"].Sure<ZxJson::JArray_t&>();

        for (const auto& img_dat : m_vcData)
        {
            array.emplace_back(img_dat.Save(msSaveDir));
        }

        return json;
    }

    auto ABMPImage10::Make(ZxMem& zmWriter) const -> void
    {
        zmWriter << std::array<char, 16>{ "abimage10" } << static_cast<uint8_t>(m_vcData.size());

        for (const auto& data : m_vcData)
        {
            data.Make(zmWriter);
        }
    }

    auto ABMPImage10::SizeBytes() const -> size_t
    {
        size_t size = 16; // signature
        size += sizeof(uint8_t); // imgdat_count
        for (const auto& img_dat : m_vcData)
        {
            size += img_dat.SizeBytes();
        }
        return size;
    }
}

