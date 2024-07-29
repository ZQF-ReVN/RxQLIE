#pragma once
#include <array>
#include <cstdint>
#include <string_view>
#include <ZxMem/ZxMem.h>


namespace ZQF::RxQLIE
{
    auto CheckSignature(std::array<char, 16>& aSig, const std::string_view msSigTarget) -> void;

    template<class T>
    static auto DelphiStrView(ZxMem& zReader) -> std::basic_string_view<T>
    {
        const auto str_size = zReader.Get<std::uint16_t>();
        const auto str_ptr = zReader.PtrCur<T*>();
        zReader.PosInc(str_size * sizeof(T));
        return { str_ptr, str_size };
    }
}
