#include <RxQLIE/AnimationBMP.h>
#include <RxQLIE/ABMPUtility.h>
#include <ZxJson/JIO.h>
#include <ZxJson/JValue.h>
#include <ZxMem/ZxMem.h>
#include <array>
#include <string>


namespace ZQF::RxQLIE
{
	AnimationBMP12::AnimationBMP12()
	{

	}

	AnimationBMP12::AnimationBMP12(ZxMem& zmReader)
	{
		this->Load(zmReader);
	}

	AnimationBMP12::AnimationBMP12(const std::string_view msDir)
	{
		this->Load(msDir);
	}

	auto AnimationBMP12::Load(ZxMem& zmReader) -> void
	{
        auto sig = zmReader.Get<std::array<char, 16>>();
        RxQLIE::CheckSignature(sig, "abmp12");

		m_ABData.Load(zmReader);
		m_ABImage.Load(zmReader);
		m_ABSound.Load(zmReader);
	}

	auto AnimationBMP12::Load(const std::string_view msDir) -> void
	{
		std::string dir{ msDir };

        auto json_doc = ZxJson::LoadViaFile(dir + "info.json");
		auto& info_json = json_doc.Get<ZxJson::JObject_t&>();

		m_ABData.Load(dir + "abdata/", info_json["abdata"].Sure<ZxJson::JObject_t&>());
		m_ABImage.Load(dir + "abimage/", info_json["abimage"].Sure<ZxJson::JObject_t&>());
		m_ABSound.Load(dir + "absound/", info_json["absound"].Sure<ZxJson::JObject_t&>());
	}

	auto AnimationBMP12::Save(const std::string_view msSaveDir) -> void
	{
		std::string save_dir{ msSaveDir };
		ZxJson::JObject_t json;
		json["abdata"] = m_ABData.Save(save_dir + "abdata/");
		json["abimage"] = m_ABImage.Save(save_dir + "abimage/");
		json["absound"] = m_ABSound.Save(save_dir + "absound/");

        ZxJson::JValue jv{ std::move(json) };
        ZxJson::StoreViaFile(save_dir + "info.json", jv, true, true);
	}

	auto AnimationBMP12::Make(const std::string_view msPath) -> void
	{
		ZxMem mem(16 + m_ABData.SizeBytes() + m_ABImage.SizeBytes() + m_ABSound.SizeBytes());
		{
			mem << std::array<char, 16>{ "abmp12" }; // write signature
			m_ABData.Make(mem); // write abdata
			m_ABImage.Make(mem); // write abimg
			m_ABSound.Make(mem); // write absound
		}
		mem.Save(msPath, true);
	}
}
