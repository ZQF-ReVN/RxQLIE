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
		//Zut::ZxMem file_data{ "back01.b" };
		//Revne::RxQLIE::AnimationBMP abmp{ Zut::ZxView::Reader{ file_data.Ptr() } };
		//abmp.Export("back01/");

		Revne::RxQLIE::AnimationBMP abmpx{ std::string_view{ "back01/" } };
		abmpx.Save("123/");

		int a = 0;
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}
}
