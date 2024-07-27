#include <iostream>
#include <print>
#include <Zut/ZxFs.h>
#include <Zut/ZxSys.h>
#include <Revne/RxQLIE/AnimationBMP.h>

using namespace Zqf;

static auto TestUnpack()
{
	Zut::ZxMem file_data;
	for (Zut::ZxFs::Walk walk("test_file_/"); walk.NextFile();)
	{
		std::string_view file_path = walk.GetPath();
		std::string save_path{ file_path.substr(0, file_path.rfind('.')) };
		save_path.append(1, '/');

		Revne::QLIE::AnimationBMP12{ file_data.Load(file_path) }.Save(save_path);

		std::println("{}", file_path);
	}
}

static auto TestPack()
{
	for (Zut::ZxFs::Walk walk("test_file_/"); walk.NextDir();)
	{
		std::string_view dir = walk.GetPath();
		std::string_view file_name = walk.GetName();
		std::string save_path{"makes/"};
		save_path.append(file_name.substr(0, file_name.size() - 1));
		save_path.append(".b.new");
		Revne::QLIE::AnimationBMP12{ dir }.Make(save_path);

		std::println("{}", dir);
	}
}

auto main(int argc, char* argv[]) -> int
{
	Zut::ZxSys::InitConsoleEncoding();

	try
	{
		TestUnpack();
		TestPack();
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
		std::ignore = std::getchar();
	}
}
