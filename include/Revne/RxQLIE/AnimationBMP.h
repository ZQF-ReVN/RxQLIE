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
		ABMPData15();
		ABMPData15(Zut::ZxView::Reader& zvReader);

	public:
		auto Load(Zut::ZxView::Reader& zvReader) -> void;
		auto Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void;
		auto Save(const std::string_view msSaveDir) const->Zut::ZxJson::JObject_t;

	public:
		auto SizeBytes() const->size_t;
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
		Zut::ZxMem m_zmData;

	public:
		ABMPImageData15();
		ABMPImageData15(Zut::ZxView::Reader& zvReader);
		ABMPImageData15(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject);

	public:
		auto Load(Zut::ZxView::Reader& zvReader) -> void;
		auto Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void;
		auto Save(const std::string_view msSaveDir) const->Zut::ZxJson::JObject_t;

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
		ABMPImage10(Zut::ZxView::Reader& zvReader);

	public:
		auto Load(Zut::ZxView::Reader& zvReader) -> void;
		auto Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void;
		auto Save(const std::string_view msSaveDir) const->Zut::ZxJson::JObject_t;

	public:
		auto SizeBytes() const->size_t;
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
		std::u16string m_u16FileName;
		std::string m_msHashName;
		ABMPSoundDataType m_eType{};
		Zut::ZxMem m_zmData;

	public:
		ABMPSoundData12();
		ABMPSoundData12(Zut::ZxView::Reader& zvReader);
		ABMPSoundData12(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject);

	public:
		auto Load(Zut::ZxView::Reader& zvReader) -> void;
		auto Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void;
		auto Save(const std::string_view msSaveDir) const->Zut::ZxJson::JObject_t;

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
		ABMPSound10(Zut::ZxView::Reader& zvReader);

	public:
		auto Load(Zut::ZxView::Reader& zvReader) -> void;
		auto Load(const std::string_view msDir, Zut::ZxJson::JObject_t& rfJObject) -> void;
		auto Save(const std::string_view msSaveDir) const->Zut::ZxJson::JObject_t;

	public:
		auto SizeBytes() const->size_t;
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

	public:
		AnimationBMP();
		AnimationBMP(Zut::ZxView::Reader zvReader);
		AnimationBMP(const std::string_view msDir);

	public:
		auto Load(Zut::ZxView::Reader zvReader) -> void;
		auto Load(const std::string_view msDir) -> void;
		auto Save(const std::string_view msSaveDir) -> void;
		auto Make(const std::string_view msMakePath) -> void;
	};
}