#pragma once
#include <string_view>
#include <ZxMem/ZxMem.h>
#include <RxQLIE/ABMPData.h>
#include <RxQLIE/ABMPImage.h>
#include <RxQLIE/ABMPSound.h>


namespace ZQF::RxQLIE
{
	class AnimationBMP12
	{
	private:
		ABMPData15 m_ABData;
		ABMPImage10 m_ABImage;
		ABMPSound10 m_ABSound;

	public:
		AnimationBMP12();
		AnimationBMP12(ZxMem& zmReader);
		AnimationBMP12(const std::string_view msDir);

	public:
		auto Load(ZxMem& zmReader) -> void;
		auto Load(const std::string_view msDir) -> void;
		auto Save(const std::string_view msSaveDir) -> void;
		auto Make(const std::string_view msMakePath) -> void;
	};
}
