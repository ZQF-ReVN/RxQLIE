#pragma once
#include <span>
#include <ranges>
#include <string_view>

#include <Zut/ZxMem.h>


namespace Zqf::Revne::RxQLIE::Cryptor
{
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

	// 004E6328  004E5754
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

	// 004E4F94  004E4E38
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

}