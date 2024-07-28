#include <RxQLIE/ABMPUtility.h>
#include <format>
#include <stdexcept>


namespace ZQF::RxQLIE
{
    auto CheckSignature(std::array<char, 16>& aSig, const std::string_view msSigTarget) -> void
    {
        for (auto& c : aSig) { c = static_cast<char>(std::tolower(c)); }
        const std::string_view sig{ aSig.data() };
        if (sig != msSigTarget)
        {
            throw std::runtime_error(std::format("erro signatrue {}", msSigTarget));
        }
    }
}
