#include <iostream>
#include <print>
#include <Zut/ZxSys.h>
#include <Revne/RxQLIE/AnimationBMP.h>


auto main() -> int
{
	using namespace Zqf;
	Zut::ZxSys::InitConsoleEncoding();

	try
	{
		Revne::RxQLIE::AnimationBMP abmp("back01.b");
		abmp.Store("back01/");

	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}
}
