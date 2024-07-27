#include <iostream>
#include <print>
#include <ZxFS/ZxFS.h>
#include <RxQLIE/AnimationBMP.h>


static auto TestUnpack()
{
    std::string path_temp;
	ZQF::ZxMem file_data;
	for (ZQF::ZxFS::Walk walk("test_file_/"); walk.NextFile();)
	{
        auto file_path = walk.GetPath();
        auto& save_path = path_temp.append(ZQF::ZxFS::FileSuffixDel(file_path)).append(1, '/');
		ZQF::RxQLIE::AnimationBMP12{ file_data.Load(file_path) }.Save(save_path);
        path_temp.clear();

		std::println("{}", file_path);
	}
}

static auto TestPack()
{
    std::string path_temp;
	for (ZQF::ZxFS::Walk walk("test_file_/"); walk.NextDir();)
	{
        auto dir_path = walk.GetPath();
		auto dir_name = walk.GetName();
        auto& save_path = path_temp.append("makes/").append(dir_name.substr(0, dir_name.size() - 1)).append(".b.new");
		ZQF::RxQLIE::AnimationBMP12{ dir_path }.Make(save_path);
        path_temp.clear();

		std::println("{}", dir_path);
	}
}

auto main() -> int
{
	try
	{
		TestUnpack();
		TestPack();
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}
}
