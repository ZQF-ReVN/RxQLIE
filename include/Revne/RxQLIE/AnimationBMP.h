#pragma once
#include <span>
#include <ranges>
#include <vector>
#include <Zut/ZxStr.h>
#include <Zut/ZxMem.h>
#include <Zut/ZxView.h>
#include <Zut/ZxJson.h>

#include <Revne/RxQLIE/Cryptor.h>


namespace Zqf::Revne::RxQLIE
{
	static auto CheckSignature(Zut::ZxView::Reader& zReader, std::array<char, 16> aSignature) -> void
	{
		if (zReader.Get<std::array<char, 16>>() != aSignature)
		{
			throw std::runtime_error("erro signatrue");
		}
	}

	template<class T>
	static auto DelphiStrView(Zut::ZxView::Reader& zReader) -> std::basic_string_view<T>
	{
		const auto str_size = zReader.Get<uint16_t>();
		const auto str_ptr = zReader.CurPtr<T*>();
		zReader.IncPos(str_size * sizeof(T));
		return { str_ptr, str_size };
	}
}

namespace Zqf::Revne::RxQLIE
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
		Zut::ZxMem m_zmData;

	public:
		ABMPData15()
		{

		}

		ABMPData15(Zut::ZxView::Reader& zReader)
		{
			this->Load(zReader);
		}

		auto Load(Zut::ZxView::Reader& zReader) -> void
		{
			RxQLIE::CheckSignature(zReader, { "abdata15" });

			const auto data_size = zReader.Get<uint32_t>();
			const auto data_ptr = zReader.CurPtr<uint8_t*>();
			m_zmData.Copy(std::span{ data_ptr, data_size });
			zReader.IncPos(data_size);
		}

		auto Store(const std::string_view msDir) const -> Zut::ZxJson::JObject_t
		{
			Zut::ZxFile::SaveDataViaPath(std::string{ msDir } + "abdata15.bin", std::span{ m_zmData }, true);

			Zut::ZxJson::JObject_t json;
			json["version"] = 15;
			json["data_size"] = m_zmData.SizeBytes();

			return json;
		}

		auto SizeBytes() const -> size_t
		{
			size_t size = 16; // signature
			size += sizeof(uint8_t); // snddat_count
			size += m_zmData.SizeBytes();
			return size;
		}
	};

}

namespace Zqf::Revne::RxQLIE
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
		std::u16string_view m_u16FileName;
		std::string_view m_msHashName;
		ABMPImageDataType m_eType{};
		size_t m_nOffsetX{};
		size_t m_nOffsetY{};
		size_t m_nOffsetZ{};
		size_t m_nVirtualWidth{};
		size_t m_nVirtualHeigh{};
		size_t m_nVirtualDepth{};
		size_t m_nRenderingTextureMode{};
		size_t m_nRenderingTextureBGColor{};
		std::span<uint8_t> m_spData;

	public:
		ABMPImageData15()
		{

		}

		ABMPImageData15(Zut::ZxView::Reader& zReader)
		{
			this->Load(zReader);
		}

		auto Load(Zut::ZxView::Reader& zReader) -> void
		{
			RxQLIE::CheckSignature(zReader, { "abimgdat15" });
			m_nVirtualFlag = zReader.Get<uint32_t>();
			m_u16FileName = RxQLIE::DelphiStrView<char16_t>(zReader);
			m_msHashName = RxQLIE::DelphiStrView<char>(zReader);
			m_eType = static_cast<ABMPImageDataType>(zReader.Get<uint8_t>());
			m_nOffsetX = zReader.Get<uint32_t>();
			m_nOffsetY = zReader.Get<uint32_t>();
			m_nOffsetZ = zReader.Get<uint32_t>();

			if (m_nVirtualFlag > 2)
			{
				throw std::runtime_error("ABMPImageData15::Load: error virtual flag");
			}

			if (m_nVirtualFlag >= 2)
			{
				m_nVirtualWidth = zReader.Get<uint32_t>();
				m_nVirtualHeigh = zReader.Get<uint32_t>();
				m_nVirtualDepth = zReader.Get<uint32_t>();
			}

			m_nRenderingTextureMode = zReader.Get<uint8_t>();
			m_nRenderingTextureBGColor = zReader.Get<uint32_t>();

			const auto data_size = zReader.Get<uint32_t>();
			const auto data_ptr = zReader.CurPtr<uint8_t*>();
			m_spData = std::span{ data_ptr, data_size };
			zReader.IncPos(data_size);
		}

		auto Store(const std::string_view msDir) const -> Zut::ZxJson::JObject_t
		{
			auto file_suffix = this->GetSuffix();
			auto file_name_u8 = Zut::ZxStr::CvtSafe(m_u16FileName, Zut::CodePage::UTF8);

			if (!m_spData.empty())
			{
				std::string save_path{ msDir };
				save_path.append(file_name_u8.first).append(file_suffix);
				Zut::ZxFile::SaveDataViaPath(save_path, m_spData, true);
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
			json["data_size"] = m_spData.size_bytes();
			return json;
		}

		auto SizeBytes() const -> size_t
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
			size += m_spData.size_bytes(); // data binary size
			return size;
		}

		auto GetSuffix() const -> std::string_view
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
	};


	class ABMPImage10
	{
	private:
		std::vector<ABMPImageData15> m_vcData;

	public:
		ABMPImage10()
		{

		}


		ABMPImage10(Zut::ZxView::Reader& zReader)
		{
			this->Load(zReader);
		}

		auto Load(Zut::ZxView::Reader& zReader) -> void
		{
			RxQLIE::CheckSignature(zReader, { "abimage10" });

			for (auto imgdat_cnt = zReader.Get<uint8_t>(); auto _ : std::views::iota(0u, imgdat_cnt))
			{
				m_vcData.emplace_back(zReader);
			}
		}

		auto Store(const std::string_view msDir) const -> Zut::ZxJson::JObject_t
		{
			Zut::ZxJson::JObject_t json;
			json["version"] = 10;
			json["conut"] = m_vcData.size();
			auto& array = json["abimgdat_list"].Sure<Zut::ZxJson::JArray_t>();

			for (const auto& img_dat : m_vcData)
			{
				array.emplace_back(img_dat.Store(msDir));
			}

			return json;
		}

		auto SizeBytes() const -> size_t
		{
			size_t size = 16; // signature
			size += sizeof(uint8_t); // imgdat_count
			for (const auto& img_dat : m_vcData)
			{
				size += img_dat.SizeBytes();
			}
			return size;
		}
	};

}

namespace Zqf::Revne::RxQLIE
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
		std::u16string_view m_u16FileName;
		std::string_view m_msHashName;
		ABMPSoundDataType m_eType{};
		std::span<uint8_t> m_spData;

	public:
		ABMPSoundData12() 
		{

		}

		ABMPSoundData12(Zut::ZxView::Reader& zReader)
		{
			this->Load(zReader);
		}

		auto Load(Zut::ZxView::Reader& zReader) -> void
		{
			RxQLIE::CheckSignature(zReader, { "absnddat12" });
			m_nFlag = zReader.Get<uint32_t>();

			if (m_nFlag > 1)
			{
				throw std::runtime_error("error");
			}

			m_u16FileName = RxQLIE::DelphiStrView<char16_t>(zReader);
			m_msHashName = RxQLIE::DelphiStrView<char>(zReader);
			m_eType = static_cast<ABMPSoundDataType>(zReader.Get<uint8_t>());

			const auto data_size = zReader.Get<uint32_t>();
			const auto data_ptr = zReader.CurPtr<uint8_t*>();
			m_spData = std::span{ data_ptr, data_size };
			zReader.IncPos(data_size);
		}

		auto Store(const std::string_view msDir) const -> Zut::ZxJson::JObject_t
		{
			auto file_suffix = this->GetSuffix();
			auto file_name_u8 = Zut::ZxStr::CvtSafe(m_u16FileName, Zut::CodePage::UTF8);

			if (!m_spData.empty())
			{
				std::string save_path{ msDir };
				save_path.append(file_name_u8.first).append(file_suffix);
				Zut::ZxFile::SaveDataViaPath(save_path, m_spData, true);
			}

			Zut::ZxJson::JObject_t json;
			json["version"] = 12;
			json["flag"] = m_nFlag;
			json["file_name"] = file_name_u8.first;
			json["hash_name"] = Zut::ZxStr::CvtSafe(Zut::ZxStr::CvtSafe(m_msHashName, Zut::CodePage::SJIS).first, Zut::CodePage::UTF8).first;
			json["data_type"] = static_cast<uint8_t>(m_eType);
			json["data_size"] = m_spData.size();
			return json;
		}

		auto SizeBytes() const -> size_t
		{
			size_t size = 16; // signature
			size += sizeof(uint16_t) + m_u16FileName.length() * sizeof(char16_t); // file_name
			size += sizeof(uint16_t) + m_msHashName.length() * sizeof(char); // hash_name
			size += sizeof(uint8_t); // type
			size += sizeof(uint32_t); // data_size
			size += m_spData.size_bytes(); // data binary size
			return size;
		}

		auto GetSuffix() const -> std::string_view
		{
			switch (m_eType)
			{
			case ABMPSoundDataType::WAV: return ".wav";
			case ABMPSoundDataType::OGG: return ".ogg";
			default: throw std::runtime_error("error");
			}
		}
	};

	class ABMPSound10
	{
	private:
		std::vector<ABMPSoundData12> m_vcData;

	public:
		ABMPSound10()
		{

		}

		ABMPSound10(Zut::ZxView::Reader& zReader)
		{
			this->Load(zReader);
		}

		auto Load(Zut::ZxView::Reader& zReader) -> void
		{
			RxQLIE::CheckSignature(zReader, { "absound10" });

			for (auto snddat_cnt = zReader.Get<uint8_t>(); auto _ : std::views::iota(0u, snddat_cnt))
			{
				m_vcData.emplace_back(zReader);
			}
		}

		auto Store(const std::string_view msDir) const -> Zut::ZxJson::JObject_t
		{
			Zut::ZxJson::JObject_t json;
			json["version"] = 10;
			json["conut"] = m_vcData.size();
			auto& array = json["absnddat_list"].Sure<Zut::ZxJson::JArray_t>();

			for (const auto& snd_dat : m_vcData)
			{
				array.emplace_back(snd_dat.Store(msDir));
			}

			return json;
		}

		auto SizeBytes() const -> size_t
		{
			size_t size = 16; // signature
			size += sizeof(uint8_t); // snddat_count
			for (const auto& snd_dat : m_vcData)
			{
				size += snd_dat.SizeBytes();
			}
			return size;
		}
	};
}

namespace Zqf::Revne::RxQLIE
{
	class AnimationBMP
	{
	private:
		ABMPData15 m_ABData;
		ABMPImage10 m_ABImage;
		ABMPSound10 m_ABSound;
		Zut::ZxMem m_ABMPData;

	public:
		AnimationBMP() {}

		AnimationBMP(const std::string_view msPath) : m_ABMPData{ msPath }
		{
			this->Load(Zut::ZxView::Reader{ m_ABMPData.Ptr() });
		}

		auto Load(Zut::ZxView::Reader zReader) -> void
		{
			RxQLIE::CheckSignature(zReader, { "abmp12" });

			m_ABData.Load(zReader);
			m_ABImage.Load(zReader);
			m_ABSound.Load(zReader);
		}

		auto Store(const std::string_view msDir) -> void
		{
			std::string dir{ msDir };
			Zut::ZxJson::JObject_t json;
			json["abdata"] = m_ABData.Store(dir + "abdata/");
			json["abimage"] = m_ABImage.Store(dir + "abimage/");
			json["absound"] = m_ABSound.Store(dir + "absound/");
			Zut::ZxJson::JDoc{ std::move(json) }.Save(dir + "info.json", true);
		}
	};

}