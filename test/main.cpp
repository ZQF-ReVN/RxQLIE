#include <iostream>
#include <print>
#include <ranges>
#include <vector>
#include <Zut/ZxFs.h>
#include <Zut/ZxStr.h>
#include <Zut/ZxSys.h>
#include <Zut/ZxMem.h>
#include <Zut/ZxView.h>
#include <Zut/ZxJson.h>


namespace Zqf::QLIE
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


namespace Zqf::QLIE::Pack::V3
{
	class Cryptor
	{
	public:
		class XMM {
		public:
			static auto punpckldq(uint64_t m0, uint64_t m1) -> uint64_t
			{
				uint64_t v0 = m0 & 0xFFFFFFFF;
				uint64_t v1 = m1 & 0xFFFFFFFF;
				uint64_t ret = v0 | (v1 <<= 32);

				return ret;
			}

			static auto cvtsi32_si64(uint32_t n32) -> uint64_t
			{
				return static_cast<uint64_t>(n32);
			}

			static auto paddd(uint64_t m0, uint64_t m1) -> uint64_t
			{
				auto m0_ptr = reinterpret_cast<uint32_t*>(&m0);
				auto m1_ptr = reinterpret_cast<uint32_t*>(&m1);

				m1_ptr[0] += m0_ptr[0];
				m1_ptr[1] += m0_ptr[1];

				return m1;
			}

			static auto paddw(uint64_t m0, uint64_t m1) -> uint64_t
			{
				auto m0_ptr = reinterpret_cast<uint16_t*>(&m0);
				auto m1_ptr = reinterpret_cast<uint16_t*>(&m1);

				m0_ptr[0] += m1_ptr[0];
				m0_ptr[1] += m1_ptr[1];
				m0_ptr[2] += m1_ptr[2];
				m0_ptr[3] += m1_ptr[3];

				return m0;
			}

			static auto por(uint64_t m0, uint64_t m1) -> uint64_t
			{
				uint64_t ret = m0 | m1;

				return ret;
			}

			static auto pxor(uint64_t m0, uint64_t m1) -> uint64_t
			{
				uint64_t ret = m0 ^ m1;

				return ret;

			}

			static auto pslldi(uint64_t m0, uint32_t nShift) -> uint64_t
			{
				uint64_t l = m0 & 0xFFFFFFFF;
				uint64_t h = (m0 >> 32) & 0xFFFFFFFF;

				l <<= nShift;
				h <<= nShift;
				l &= 0xFFFFFFFF;
				h &= 0xFFFFFFFF;

				uint64_t ret = l | (h << 32);

				return ret;
			}

			static auto psrldi(uint64_t m0, uint32_t nShift) -> uint64_t
			{
				if (nShift >= 32) { return 0; }

				int64_t l = m0 & 0xFFFFFFFF;
				int64_t h = (m0 >> 32) & 0xFFFFFFFF;

				l >>= nShift;
				h >>= nShift;
				l &= 0xFFFFFFFF;
				h &= 0xFFFFFFFF;

				uint64_t ret = (l | (h << 32));

				return ret;
			}

			static auto psrlqi(uint64_t m0, uint32_t nShift) -> uint64_t
			{
				m0 >>= nShift;
				uint64_t ret = m0;

				return ret;
			}

			static auto pmaddwd(uint64_t m0, uint64_t m1) -> uint64_t
			{
				int16_t* m0_ptr = reinterpret_cast<int16_t*>(&m0);
				int16_t* m1_ptr = reinterpret_cast<int16_t*>(&m1);

				int32_t v0 = m0_ptr[0] * m1_ptr[0];
				int32_t v1 = m0_ptr[1] * m1_ptr[1];
				int32_t v2 = m0_ptr[2] * m1_ptr[2];
				int32_t v3 = m0_ptr[3] * m1_ptr[3];

				int64_t vs0 = static_cast<int64_t>(v0) + static_cast<int64_t>(v1);
				int64_t vs1 = static_cast<int64_t>(v2) + static_cast<int64_t>(v3);
				vs0 &= 0xFFFFFFFF;
				vs1 &= 0xFFFFFFFF;

				int64_t ret = vs0 | (vs1 << 32);

				return static_cast<uint64_t>(ret);
			}

		};

		// 004EDA1C
		class LZW
		{
		public:
			auto Dec(const std::span<const uint8_t> spData) -> Zut::ZxMem {}
			auto Enc(const std::span<const uint8_t> spData) -> Zut::ZxMem {}
		};

		// 004E6328、004E5754
		class KeyCryptor
		{
		private:
			uint32_t m_nKey{};

		public:
			KeyCryptor()
			{

			}

			KeyCryptor(const std::span<const uint8_t> spData)
			{
				this->m_nKey = KeyCryptor::MakeKey(spData);
			}

			KeyCryptor(const KeyCryptor& rfOBJ) = default;


			static auto MakeKey(const std::span<const uint8_t> spData) -> uint32_t
			{
				if (spData.size_bytes() < 8) { return 0; }

				const uint64_t magic_v = XMM::cvtsi32_si64(0xA35793A7);
				const uint64_t magic = XMM::punpckldq(magic_v, magic_v);

				uint64_t round_ret{};

				const auto block_cnt = spData.size_bytes() >> 3;
				const auto block_ptr = reinterpret_cast<const uint64_t*>(spData.data());
				for (uint64_t round_tmp{}; const auto block : std::span<const uint64_t>{ block_ptr, block_cnt })
				{
					round_tmp = XMM::paddw(round_tmp, magic);
					const uint64_t block_pxor = XMM::pxor(block, round_tmp);
					const uint64_t block_padd = XMM::paddw(round_ret, block_pxor);
					const uint64_t block_lldi = XMM::pslldi(block_padd, 0x03);
					const uint64_t block_rldi = XMM::psrldi(block_padd, 0x1D);
					round_ret = XMM::por(block_lldi, block_rldi);
				}

				const uint64_t round_ret_rlqi = XMM::psrlqi(round_ret, 0x20);
				const uint64_t ret = XMM::pmaddwd(round_ret, round_ret_rlqi);
				return static_cast<uint32_t>(ret);
			}

			auto GetKey() const -> uint32_t
			{
				return m_nKey;
			}

			auto Dec(const std::span<uint8_t> spData) const -> void
			{
				if (spData.size_bytes() < 8) { return; }

				const auto block_bytes = static_cast<uint32_t>(spData.size_bytes());

				const auto magic_v0 = XMM::cvtsi32_si64(0xA73C5F9D);
				const auto magic_v1 = XMM::cvtsi32_si64(0xCE24F523);
				const auto magic_v2 = XMM::cvtsi32_si64((this->m_nKey + block_bytes) ^ 0xFEC9753E);

				auto magic_0 = XMM::punpckldq(magic_v0, magic_v0);
				auto magic_1 = XMM::punpckldq(magic_v1, magic_v1);
				auto magic_2 = XMM::punpckldq(magic_v2, magic_v2);

				const auto block_cnt = block_bytes >> 3;
				const auto block_ptr = reinterpret_cast<uint64_t*>(spData.data());
				for (auto& block : std::span<uint64_t>{ block_ptr, block_cnt })
				{
					magic_0 = XMM::pxor(XMM::paddd(magic_0, magic_1), magic_2);
					magic_2 = XMM::pxor(block, magic_0);
					block = magic_2;
				}
			}

			auto Enc(const std::span<uint8_t> spData) const -> void
			{
				if (spData.size_bytes() < 8) { return; }

				const auto block_bytes = static_cast<uint32_t>(spData.size_bytes());

				const auto magic_v0 = XMM::cvtsi32_si64(0xA73C5F9D);
				const auto magic_v1 = XMM::cvtsi32_si64(0xCE24F523);
				const auto magic_v2 = XMM::cvtsi32_si64((this->m_nKey + block_bytes) ^ 0xFEC9753E);

				auto magic_0 = XMM::punpckldq(magic_v0, magic_v0);
				auto magic_1 = XMM::punpckldq(magic_v1, magic_v1);
				auto magic_2 = XMM::punpckldq(magic_v2, magic_v2);

				const auto block_cnt = block_bytes >> 3;
				const auto block_ptr = reinterpret_cast<uint64_t*>(spData.data());
				for (auto& block : std::span<uint64_t>{ block_ptr, block_cnt })
				{
					magic_0 = XMM::pxor(XMM::paddd(magic_0, magic_1), magic_2);
					magic_2 = block;
					block = XMM::pxor(block, magic_0);
				}
			}
		};

		// 004E4F94、004E4E38
		class StrCryptor
		{
		private:
			Cryptor::KeyCryptor m_Cryptor;

		public:
			StrCryptor(Cryptor::KeyCryptor Cryptor) : m_Cryptor{ Cryptor }
			{

			}

			auto Dec(const std::span<uint16_t> spData) -> void
			{
				if (spData.size() < 1) { return; }

				const uint32_t key = m_Cryptor.GetKey();
				const uint32_t dec_key = ((key >> 16) & 0xFFFF) ^ key;

				const uint32_t str_chars_exp = static_cast<uint32_t>(spData.size());
				uint32_t magic_0 = ((str_chars_exp * str_chars_exp) ^ str_chars_exp ^ 0x3E13 ^ dec_key) & 0xFFFF;
				const uint32_t magic_1 = magic_0;

				for (const auto [seq, ch] : std::views::enumerate(spData))
				{
					magic_0 = 8 * magic_0 + magic_1 + static_cast<uint32_t>(seq);
					ch ^= magic_0;
				}
			}

			auto Enc(const std::span<uint16_t> spData) -> void
			{
				this->Dec(spData);
			}
		};

		// 004ED950
		class BinaryCryptor_V0
		{
		private:
			Cryptor::KeyCryptor m_Cryptor;

		public:
			BinaryCryptor_V0(Cryptor::KeyCryptor Cryptor) : m_Cryptor{ Cryptor }
			{

			}

			auto Dec(const std::span<const uint8_t> spData) -> Zut::ZxMem {}
			auto Enc(const std::span<const uint8_t> spData) -> Zut::ZxMem {}
		};
	};

	class AnimationBMP
	{
	private:
		class ABDat15
		{
		private:
			class Motion
			{
				class Layer
				{
				private:

				};

			private:
				std::vector<Motion::Layer> m_vcData;
			};

		private:
			std::span<uint8_t> m_spData;

		public:
			ABDat15()
			{

			}

			ABDat15(Zut::ZxView::Reader& zReader)
			{
				this->Load(zReader);
			}

			auto Load(Zut::ZxView::Reader& zReader) -> void
			{
				QLIE::CheckSignature(zReader, { "abdata15" });

				const auto data_size = zReader.Get<uint32_t>();
				const auto data_ptr = zReader.CurPtr<uint8_t*>();
				m_spData = std::span{ data_ptr, data_size };
				zReader.IncPos(data_size);
			}

			auto Store(const std::string_view msDir) const -> Zut::ZxJson::JObject_t
			{
				Zut::ZxFile::SaveDataViaPath(std::string{ msDir } + "abdata15.bin", m_spData, true);

				Zut::ZxJson::JObject_t json;
				json["version"] = 15;
				json["data_size"] = m_spData.size_bytes();

				return json;
			}

			auto SizeBytes() const -> size_t
			{
				size_t size = 16; // signature
				size += sizeof(uint8_t); // snddat_count
				size += m_spData.size_bytes();
				return size;
			}
		};

		class ABImage10
		{
		private:
			class ImgDat15
			{
			private:
				enum class ImgDatType : uint8_t
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

			private:
				size_t m_nVirtualFlag{};
				std::u16string_view m_u16FileName;
				std::string_view m_msHashName;
				ImgDatType m_eType{};
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
				ImgDat15()
				{

				}

				ImgDat15(Zut::ZxView::Reader& zReader)
				{
					this->Load(zReader);
				}

				auto Load(Zut::ZxView::Reader& zReader) -> void
				{
					QLIE::CheckSignature(zReader, { "abimgdat15" });
					m_nVirtualFlag = zReader.Get<uint32_t>();
					m_u16FileName = QLIE::DelphiStrView<char16_t>(zReader);
					m_msHashName = QLIE::DelphiStrView<char>(zReader);
					m_eType = static_cast<ImgDat15::ImgDatType>(zReader.Get<uint8_t>());
					m_nOffsetX = zReader.Get<uint32_t>();
					m_nOffsetY = zReader.Get<uint32_t>();
					m_nOffsetZ = zReader.Get<uint32_t>();
					
					if (m_nVirtualFlag > 2)
					{
						throw std::runtime_error("error");
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
					case ImgDat15::ImgDatType::BMP: return ".bmp";
					case ImgDat15::ImgDatType::JPG: return ".jpg";
					case ImgDat15::ImgDatType::PNG0: return ".png";
					case ImgDat15::ImgDatType::PNG1: return ".png";
					case ImgDat15::ImgDatType::M: return ".m";
					case ImgDat15::ImgDatType::ARGB: return ".argb";
					case ImgDat15::ImgDatType::B: return ".b";
					case ImgDat15::ImgDatType::OGV: return ".ogv";
					case ImgDat15::ImgDatType::MDL: return ".mdl";
					default: throw std::runtime_error("error");
					}
				}
			};

		private:
			std::vector<ABImage10::ImgDat15> m_vcData;

		public:
			ABImage10()
			{

			}


			ABImage10(Zut::ZxView::Reader& zReader)
			{
				this->Load(zReader);
			}

			auto Load(Zut::ZxView::Reader& zReader) -> void
			{
				QLIE::CheckSignature(zReader, { "abimage10" });

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

		class ABSound10
		{
		private:
			class SndDat12
			{
			private:
				enum class SoundDatType : uint8_t
				{
					WAV = 0,
					OGG = 1
				};

			private:
				size_t m_nFlag{};
				std::u16string_view m_u16FileName;
				std::string_view m_msHashName;
				SoundDatType m_eType{};
				std::span<uint8_t> m_spData;

			public:
				SndDat12() {}

				SndDat12(Zut::ZxView::Reader& zReader)
				{
					this->Load(zReader);
				}

				auto Load(Zut::ZxView::Reader& zReader) -> void
				{
					QLIE::CheckSignature(zReader, { "absnddat12" });
					m_nFlag = zReader.Get<uint32_t>();

					if (m_nFlag > 1) 
					{
						throw std::runtime_error("error");
					}

					m_u16FileName = QLIE::DelphiStrView<char16_t>(zReader);
					m_msHashName = QLIE::DelphiStrView<char>(zReader);
					m_eType = static_cast<SndDat12::SoundDatType>(zReader.Get<uint8_t>());

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
					case SndDat12::SoundDatType::WAV: return ".wav";
					case SndDat12::SoundDatType::OGG: return ".ogg";
					default: throw std::runtime_error("error");
					}
				}
			};

		private:
			std::vector<ABSound10::SndDat12> m_vcData;

		public:
			ABSound10()
			{

			}

			ABSound10(Zut::ZxView::Reader& zReader)
			{
				this->Load(zReader);
			}

			auto Load(Zut::ZxView::Reader& zReader) -> void
			{
				QLIE::CheckSignature(zReader, { "absound10" });

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

	private:
		ABDat15 m_ABData;
		ABImage10 m_ABImage;
		ABSound10 m_ABSound;
		Zut::ZxMem m_ABMPData;

	public:
		AnimationBMP() {}

		AnimationBMP(const std::string_view msPath) : m_ABMPData{ msPath }
		{
			this->Load(Zut::ZxView::Reader{ m_ABMPData.Ptr() });
		}

		auto Load(Zut::ZxView::Reader zReader) -> void
		{
			QLIE::CheckSignature(zReader, { "abmp12" });

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

	class HashFilePack
	{
		class HashTable
		{
		public:
			HashTable() {}

		};

		class FilePack
		{
		private:
			Cryptor::KeyCryptor m_CryptorTable;

		public:
			FilePack() {}

		};

	private:
		FilePack m_FilePack;
		HashTable m_HashTable;
		std::string m_FilePackPath;
		std::string m_HashTablePath;
	public:
		HashFilePack() {};
		~HashFilePack() {};

	};
}



auto main() -> int
{
	using namespace Zqf;
	Zut::ZxSys::InitConsoleEncoding();

	try
	{
		QLIE::Pack::V3::AnimationBMP abmp("back01.b");
		abmp.Store("back01/");

	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}
}
