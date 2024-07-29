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

    ABMPImageData15::ABMPImageData15(const std::string_view msDir, const ZxJson::JObject_t& rfJObject)
    {
        this->Load(msDir, rfJObject);
    }

    auto ABMPImageData15::Load(ZxMem& zmReader) -> void
    {
        auto sig = zmReader.Get<std::array<char, 16>>();
        RxQLIE::CheckSignature(sig, "abimgdat15");

        m_nVirtualFlag = zmReader.Get<std::uint32_t>();
        m_u16FileName = RxQLIE::DelphiStrView<char16_t>(zmReader);
        m_msHashName = RxQLIE::DelphiStrView<char>(zmReader);
        m_eType = static_cast<ABMPImageDataType>(zmReader.Get<std::uint8_t>());
        m_nOffsetX = zmReader.Get<std::uint32_t>();
        m_nOffsetY = zmReader.Get<std::uint32_t>();
        m_nOffsetZ = zmReader.Get<std::uint32_t>();

        if (m_nVirtualFlag > 2)
        {
            throw std::runtime_error("ABMPImageData15::Load: error virtual flag");
        }

        if (m_nVirtualFlag >= 2)
        {
            m_nVirtualWidth = zmReader.Get<std::uint32_t>();
            m_nVirtualHeigh = zmReader.Get<std::uint32_t>();
            m_nVirtualDepth = zmReader.Get<std::uint32_t>();
        }

        m_nRenderingTextureMode = zmReader.Get<std::uint8_t>();
        m_nRenderingTextureBGColor = zmReader.Get<std::uint32_t>();

        const auto data_size = zmReader.Get<std::uint32_t>();
        const auto data_ptr = zmReader.PtrCur<std::uint8_t*>();
        m_zmData.Resize(data_size);
        m_zmData.Put(std::span{ data_ptr, data_size });
        zmReader.PosInc(data_size);
    }

    auto ABMPImageData15::Load(const std::string_view msDir, const ZxJson::JObject_t& rfJObject) -> void
    {
        if (rfJObject.at("version").GetNum<std::size_t>() != 15)
        {
            throw std::runtime_error("ABMPImageData15::Load(): error version");
        }

        const auto file_name = rfJObject.at("file_name").GetStrView();

        ZxCvt cvt;
        m_nVirtualFlag = rfJObject.at("virtual_flag").GetNum<decltype(m_nVirtualFlag)>();
        m_u16FileName = cvt.UTF8ToUTF16LE(file_name);
        m_msHashName = cvt.UTF8ToMBCS(rfJObject.at("hash_name").GetStrView(), 932);
        m_eType = static_cast<ABMPImageDataType>(rfJObject.at("data_type").GetNum<std::underlying_type_t<ABMPImageDataType>>());

        m_nOffsetX = rfJObject.at("offset_x").GetNum<decltype(m_nOffsetX)>();
        m_nOffsetY = rfJObject.at("offset_y").GetNum<decltype(m_nOffsetY)>();
        m_nOffsetZ = rfJObject.at("offset_z").GetNum<decltype(m_nOffsetZ)>();

        if (m_nVirtualFlag > 2)
        {
            throw std::runtime_error("ABMPImageData15::Load: error virtual flag");
        }

        if (m_nVirtualFlag >= 2)
        {
            m_nVirtualWidth = rfJObject.at("virtual_width").GetNum<decltype(m_nVirtualWidth)>();
            m_nVirtualHeigh = rfJObject.at("virtual_heigh").GetNum<decltype(m_nVirtualHeigh)>();
            m_nVirtualDepth = rfJObject.at("virtual_depth").GetNum<decltype(m_nVirtualDepth)>();
        }

        m_nRenderingTextureMode = rfJObject.at("rendering_texture_mode").GetNum<decltype(m_nRenderingTextureMode)>();
        m_nRenderingTextureBGColor = rfJObject.at("rendering_texture_bg_color").GetNum<decltype(m_nRenderingTextureBGColor)>();

        if (rfJObject.at("data_size").GetNum<std::size_t>())
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

        const auto file_suffix = this->GetSuffix();
        const auto file_name_u8 = cvt.UTF16LEToUTF8(m_u16FileName);

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

        return ZxJson::JObject_t
        {
            { "version", 15 },
            { "virtual_flag", m_nVirtualFlag },
            { "file_name", file_name_u8 },
            { "hash_name", cvt.MBCSToUTF8(m_msHashName, 932) },
            { "data_type", static_cast<std::uint8_t>(m_eType) },
            { "offset_x", m_nOffsetX },
            { "offset_y", m_nOffsetY },
            { "offset_z", m_nOffsetZ },
            { "virtual_width", m_nVirtualWidth},
            { "virtual_heigh", m_nVirtualHeigh },
            { "virtual_depth", m_nVirtualDepth },
            { "rendering_texture_mode", m_nRenderingTextureMode },
            { "rendering_texture_bg_color", m_nRenderingTextureBGColor },
            { "data_size", m_zmData.SizeBytes() }
        };
    }

    auto ABMPImageData15::Make(ZxMem& zmWriter) const -> void
    {
        zmWriter
            .Put(std::array<char, 16>{ "abimgdat15" })
            .Put(static_cast<std::uint32_t>(m_nVirtualFlag))
            .Put(static_cast<std::uint16_t>(m_u16FileName.size()))
            .Put(std::span{ m_u16FileName })
            .Put(static_cast<std::uint16_t>(m_msHashName.size()))
            .Put(std::span{ m_msHashName })
            .Put(static_cast<std::uint8_t>(m_eType))
            .Put(static_cast<std::uint32_t>(m_nOffsetX))
            .Put(static_cast<std::uint32_t>(m_nOffsetY))
            .Put(static_cast<std::uint32_t>(m_nOffsetZ));

        if (m_nVirtualFlag >= 2)
        {
            zmWriter
                .Put(static_cast<std::uint32_t>(m_nVirtualWidth))
                .Put(static_cast<std::uint32_t>(m_nVirtualHeigh))
                .Put(static_cast<std::uint32_t>(m_nVirtualDepth));
        }

        zmWriter
            .Put(static_cast<std::uint8_t>(m_nRenderingTextureMode))
            .Put(static_cast<std::uint32_t>(m_nRenderingTextureBGColor))
            .Put(static_cast<std::uint32_t>(m_zmData.SizeBytes()))
            .Put(m_zmData.Span());
    }

    auto ABMPImageData15::SizeBytes() const -> std::size_t
    {
        std::size_t size = 16;
        size += sizeof(std::uint32_t); // virtual_flag
        size += sizeof(std::uint16_t) + m_u16FileName.length() * sizeof(char16_t); // file_name
        size += sizeof(std::uint16_t) + m_msHashName.length() * sizeof(char); // hash_name
        size += sizeof(std::uint8_t); // type
        size += sizeof(std::uint32_t); // offset_x
        size += sizeof(std::uint32_t); // offset_y
        size += sizeof(std::uint32_t); // offset_z

        if (m_nVirtualFlag >= 2)
        {
            size += sizeof(std::uint32_t); // virtual_width
            size += sizeof(std::uint32_t); // virtual_height
            size += sizeof(std::uint32_t); // virtual_depth
        }

        size += sizeof(std::uint8_t); // rendering_texture_mode
        size += sizeof(std::uint32_t); // rendering_texture_bg_color
        size += sizeof(std::uint32_t); // data_size
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

        for (auto imgdat_cnt = zmReader.Get<std::uint8_t>(); [[maybe_unused]] auto _ : std::views::iota(0u, imgdat_cnt))
        {
            m_vcData.emplace_back(zmReader);
        }
    }

    auto ABMPImage10::Load(const std::string_view msDir, const ZxJson::JObject_t& rfJObject) -> void
    {
        if (rfJObject.at("version").GetNum<std::size_t>() != 10)
        {
            throw std::runtime_error("ABMPImage10::Load(): error version");
        }

        for (auto& info : rfJObject.at("abimgdat_list").GetArray())
        {
            m_vcData.emplace_back(msDir, info.GetObject());
        }
    }

    auto ABMPImage10::Save(const std::string_view msSaveDir) const -> ZxJson::JObject_t
    {
        ZxJson::JArray_t data_array;
        for (const auto& img_dat : m_vcData)
        {
            data_array.emplace_back(img_dat.Save(msSaveDir));
        }

        return ZxJson::JObject_t
        {
            { "version", 10 },
            { "conut", m_vcData.size() },
            { "abimgdat_list", std::move(data_array) },
        };
    }

    auto ABMPImage10::Make(ZxMem& zmWriter) const -> void
    {
        zmWriter << std::array<char, 16>{ "abimage10" } << static_cast<std::uint8_t>(m_vcData.size());

        for (const auto& data : m_vcData)
        {
            data.Make(zmWriter);
        }
    }

    auto ABMPImage10::SizeBytes() const -> std::size_t
    {
        std::size_t size = 16; // signature
        size += sizeof(std::uint8_t); // imgdat_count
        for (const auto& img_dat : m_vcData)
        {
            size += img_dat.SizeBytes();
        }
        return size;
    }
}

