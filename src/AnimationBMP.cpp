#include <Revne/RxQLIE/AnimationBMP.h>


namespace Zqf::Revne::RxQLIE
{
	ABMPData15::ABMPData15()
	{

	}

	ABMPData15::ABMPData15(Zut::ZxView::Reader& zvReader)
	{
		this->Load(zvReader);
	}

	auto ABMPData15::Load(Zut::ZxView::Reader& zvReader) -> void
	{
		RxQLIE::CheckSignature(zvReader, { "abdata15" });

		const auto data_size = zvReader.Get<uint32_t>();
		const auto data_ptr = zvReader.CurPtr<uint8_t*>();
		m_zmData.Copy(std::span{ data_ptr, data_size });
		zvReader.IncPos(data_size);
	}

	auto ABMPData15::Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void
	{
		if (rfJObject["version"].Get<size_t>() != 15)
		{
			throw std::runtime_error("error abmpdata version");
		}

		m_zmData.Load(std::string{ msDir }.append("abdata15.bin"));
	}

	auto ABMPData15::Save(const std::string_view msSaveDir) const -> Zut::ZxJson::JObject_t
	{
		Zut::ZxFile::SaveDataViaPath(std::string{ msSaveDir }.append("abdata15.bin"), std::span{ m_zmData }, true);

		Zut::ZxJson::JObject_t json;
		json["version"] = 15;
		json["data_size"] = m_zmData.SizeBytes();
		return json;
	}

	auto ABMPData15::SizeBytes() const -> size_t
	{
		size_t size = 16; // signature
		size += sizeof(uint8_t); // snddat_count
		size += m_zmData.SizeBytes();
		return size;
	}
}

namespace Zqf::Revne::RxQLIE
{
	ABMPImageData15::ABMPImageData15()
	{

	}

	ABMPImageData15::ABMPImageData15(Zut::ZxView::Reader& zvReader)
	{
		this->Load(zvReader);
	}

	ABMPImageData15::ABMPImageData15(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject)
	{
		this->Load(msDir, rfJObject);
	}

	auto ABMPImageData15::Load(Zut::ZxView::Reader& zvReader) -> void
	{
		RxQLIE::CheckSignature(zvReader, { "abimgdat15" });
		m_nVirtualFlag = zvReader.Get<uint32_t>();
		m_u16FileName = RxQLIE::DelphiStrView<char16_t>(zvReader);
		m_msHashName = RxQLIE::DelphiStrView<char>(zvReader);
		m_eType = static_cast<ABMPImageDataType>(zvReader.Get<uint8_t>());
		m_nOffsetX = zvReader.Get<uint32_t>();
		m_nOffsetY = zvReader.Get<uint32_t>();
		m_nOffsetZ = zvReader.Get<uint32_t>();

		if (m_nVirtualFlag > 2)
		{
			throw std::runtime_error("ABMPImageData15::Load: error virtual flag");
		}

		if (m_nVirtualFlag >= 2)
		{
			m_nVirtualWidth = zvReader.Get<uint32_t>();
			m_nVirtualHeigh = zvReader.Get<uint32_t>();
			m_nVirtualDepth = zvReader.Get<uint32_t>();
		}

		m_nRenderingTextureMode = zvReader.Get<uint8_t>();
		m_nRenderingTextureBGColor = zvReader.Get<uint32_t>();

		const auto data_size = zvReader.Get<uint32_t>();
		const auto data_ptr = zvReader.CurPtr<uint8_t*>();
		m_zmData.Copy(std::span{ data_ptr, data_size });
		zvReader.IncPos(data_size);
	}

	auto ABMPImageData15::Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void
	{
		if (rfJObject["version"].Get<size_t>() != 15)
		{
			throw std::runtime_error("ABMPImageData15::Load: error version");
		}

		const auto file_name = rfJObject["file_name"].Get<std::string_view>();

		m_nVirtualFlag = rfJObject["virtual_flag"].Get<decltype(m_nVirtualFlag)>();
		m_u16FileName = Zut::ZxStr::CvtSafe(file_name, Zut::CodePage::UTF8).first;
		m_msHashName = Zut::ZxStr::CvtSafe(Zut::ZxStr::CvtSafe(rfJObject["hash_name"].Get<std::string_view>(), Zut::CodePage::UTF8).first, Zut::CodePage::SJIS).first;
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
			m_zmData.Load(std::string{ msDir }.append(file_name).append(this->GetSuffix()));
		}
	}

	auto ABMPImageData15::Save(const std::string_view msSaveDir) const -> Zut::ZxJson::JObject_t
	{
		auto file_suffix = this->GetSuffix();
		auto file_name_u8 = Zut::ZxStr::CvtSafe(m_u16FileName, Zut::CodePage::UTF8);

		if (m_zmData.SizeBytes())
		{
			Zut::ZxFile::SaveDataViaPath(std::string{ msSaveDir }.append(file_name_u8.first).append(file_suffix), std::span{ m_zmData }, true);
		}

		Zut::ZxJson::JObject_t json;
		json["version"] = 15;
		json["virtual_flag"] = m_nVirtualFlag;
		json["file_name"] = file_name_u8.first;
		json["hash_name"] = Zut::ZxStr::CvtSafe(Zut::ZxStr::CvtSafe(m_msHashName, Zut::CodePage::SJIS).first, Zut::CodePage::UTF8).first;
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


	ABMPImage10::ABMPImage10(Zut::ZxView::Reader& zvReader)
	{
		this->Load(zvReader);
	}

	auto ABMPImage10::Load(Zut::ZxView::Reader& zvReader) -> void
	{
		RxQLIE::CheckSignature(zvReader, { "abimage10" });

		for (auto imgdat_cnt = zvReader.Get<uint8_t>(); auto _ : std::views::iota(0u, imgdat_cnt))
		{
			m_vcData.emplace_back(zvReader);
		}
	}

	auto ABMPImage10::Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void
	{
		if (rfJObject["version"].Get<size_t>() != 10)
		{
			throw std::runtime_error("error version");
		}

		for (auto& info : rfJObject["abimgdat_list"].Sure<Zut::ZxJson::JArray_t&>())
		{
			m_vcData.emplace_back(msDir, info.Sure<Zut::ZxJson::JObject_t&>());
		}
	}

	auto ABMPImage10::Save(const std::string_view msSaveDir) const -> Zut::ZxJson::JObject_t
	{
		Zut::ZxJson::JObject_t json;
		json["version"] = 10;
		json["conut"] = m_vcData.size();
		auto& array = json["abimgdat_list"].Sure<Zut::ZxJson::JArray_t&>();

		for (const auto& img_dat : m_vcData)
		{
			array.emplace_back(img_dat.Save(msSaveDir));
		}

		return json;
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


namespace Zqf::Revne::RxQLIE
{
	ABMPSoundData12::ABMPSoundData12()
	{

	}

	ABMPSoundData12::ABMPSoundData12(Zut::ZxView::Reader& zvReader)
	{
		this->Load(zvReader);
	}

	ABMPSoundData12::ABMPSoundData12(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject)
	{
		this->Load(msDir, rfJObject);
	}

	auto ABMPSoundData12::Load(Zut::ZxView::Reader& zvReader) -> void
	{
		RxQLIE::CheckSignature(zvReader, { "absnddat12" });
		m_nFlag = zvReader.Get<uint32_t>();

		if (m_nFlag > 1)
		{
			throw std::runtime_error("error");
		}

		m_u16FileName = RxQLIE::DelphiStrView<char16_t>(zvReader);
		m_msHashName = RxQLIE::DelphiStrView<char>(zvReader);
		m_eType = static_cast<ABMPSoundDataType>(zvReader.Get<uint8_t>());

		const auto data_size = zvReader.Get<uint32_t>();
		const auto data_ptr = zvReader.CurPtr<uint8_t*>();
		m_zmData.Copy(std::span{ data_ptr, data_size });
		zvReader.IncPos(data_size);
	}

	auto ABMPSoundData12::Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void
	{
		if (rfJObject["version"].Get<size_t>() != 12)
		{
			throw std::runtime_error("error version");
		}

		auto file_name = rfJObject["file_name"].Get<std::string_view>();

		m_nFlag = rfJObject["flag"].Get<decltype(m_nFlag)>();
		m_u16FileName = Zut::ZxStr::CvtSafe(file_name, Zut::CodePage::UTF8).first;
		m_msHashName = Zut::ZxStr::CvtSafe(Zut::ZxStr::CvtSafe(rfJObject["hash_name"].Get<std::string_view>(), Zut::CodePage::UTF8).first, Zut::CodePage::SJIS).first;
		m_eType = static_cast<ABMPSoundDataType>(rfJObject["data_type"].Get<std::underlying_type_t<ABMPSoundDataType>>());

		if (rfJObject["data_size"].Get<size_t>())
		{
			m_zmData.Load(std::string{ msDir }.append(file_name).append(this->GetSuffix()));
		}
	}

	auto ABMPSoundData12::Save(const std::string_view msSaveDir) const -> Zut::ZxJson::JObject_t
	{
		auto file_suffix = this->GetSuffix();
		auto file_name_u8 = Zut::ZxStr::CvtSafe(m_u16FileName, Zut::CodePage::UTF8);

		if (m_zmData.SizeBytes())
		{
			Zut::ZxFile::SaveDataViaPath(std::string{ msSaveDir }.append(file_name_u8.first).append(file_suffix), std::span{ m_zmData }, true);
		}

		Zut::ZxJson::JObject_t json;
		json["version"] = 12;
		json["flag"] = m_nFlag;
		json["file_name"] = file_name_u8.first;
		json["hash_name"] = Zut::ZxStr::CvtSafe(Zut::ZxStr::CvtSafe(m_msHashName, Zut::CodePage::SJIS).first, Zut::CodePage::UTF8).first;
		json["data_type"] = static_cast<uint8_t>(m_eType);
		json["data_size"] = m_zmData.SizeBytes();
		return json;
	}

	auto ABMPSoundData12::SizeBytes() const -> size_t
	{
		size_t size = 16; // signature
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

	ABMPSound10::ABMPSound10(Zut::ZxView::Reader& zvReader)
	{
		this->Load(zvReader);
	}

	auto ABMPSound10::Load(Zut::ZxView::Reader& zvReader) -> void
	{
		RxQLIE::CheckSignature(zvReader, { "absound10" });

		for (auto snddat_cnt = zvReader.Get<uint8_t>(); auto _ : std::views::iota(0u, snddat_cnt))
		{
			m_vcData.emplace_back(zvReader);
		}
	}

	auto ABMPSound10::Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void
	{
		if (rfJObject["version"].Get<size_t>() != 10)
		{
			throw std::runtime_error("error version");
		}

		for (auto& info : rfJObject["absnddat_list"].Sure<Zut::ZxJson::JArray_t&>())
		{
			m_vcData.emplace_back(msDir, info.Sure<Zut::ZxJson::JObject_t&>());
		}
	}

	auto ABMPSound10::Save(const std::string_view msSaveDir) const -> Zut::ZxJson::JObject_t
	{
		Zut::ZxJson::JObject_t json;
		json["version"] = 10;
		json["conut"] = m_vcData.size();

		auto& array = json["absnddat_list"].Sure<Zut::ZxJson::JArray_t&>();
		for (const auto& snd_dat : m_vcData)
		{
			array.emplace_back(snd_dat.Save(msSaveDir));
		}

		return json;
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

namespace Zqf::Revne::RxQLIE
{
	AnimationBMP::AnimationBMP()
	{

	}

	AnimationBMP::AnimationBMP(Zut::ZxView::Reader zvReader)
	{
		this->Load(zvReader);
	}

	AnimationBMP::AnimationBMP(const std::string_view msDir)
	{
		this->Load(msDir);
	}

	auto AnimationBMP::Load(Zut::ZxView::Reader zvReader) -> void
	{
		RxQLIE::CheckSignature(zvReader, { "abmp12" });

		m_ABData.Load(zvReader);
		m_ABImage.Load(zvReader);
		m_ABSound.Load(zvReader);
	}

	auto AnimationBMP::Load(const std::string_view msDir) -> void
	{
		std::string dir{ msDir };

		Zut::ZxJson::JDoc jdoc;
		jdoc.Load(dir + "info.json");
		auto& json = jdoc.GetJObject();

		m_ABData.Load(dir + "abdata/", json["abdata"].Sure<Zut::ZxJson::JObject_t&>());
		m_ABImage.Load(dir + "abimage/", json["abimage"].Sure<Zut::ZxJson::JObject_t&>());
		m_ABSound.Load(dir + "absound/", json["absound"].Sure<Zut::ZxJson::JObject_t&>());
	}

	auto AnimationBMP::Save(const std::string_view msSaveDir) -> void
	{
		std::string save_dir{ msSaveDir };
		Zut::ZxJson::JObject_t json;
		json["abdata"] = m_ABData.Save(save_dir + "abdata/");
		json["abimage"] = m_ABImage.Save(save_dir + "abimage/");
		json["absound"] = m_ABSound.Save(save_dir + "absound/");
		Zut::ZxJson::JDoc{ std::move(json) }.Save(save_dir + "info.json", true);
	}

	auto AnimationBMP::Make(const std::string_view msPath) -> void
	{

	}
}