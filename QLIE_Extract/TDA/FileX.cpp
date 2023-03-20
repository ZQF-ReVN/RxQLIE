#include "FileX.h"

#include <Windows.h>
#include <shlobj.h>


namespace TDA
{
	bool FileX::SaveFileViaPath(wchar_t* pPath, void* pBuffer, size_t nSize)
	{
		std::wstring path = GetCurrentDirectoryFolder_RETW() + pPath;
		std::wstring folder = PathRemoveFileName_RET(path);

		SHCreateDirectoryExW(NULL, folder.c_str(), NULL);

		HANDLE hFile = CreateFileW(pPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) return false;

		bool isWritten = WriteFile(hFile, pBuffer, nSize, NULL, NULL);
		FlushFileBuffers(hFile);
		CloseHandle(hFile);

		return isWritten;
	}
	size_t FileX::GetCurrentDirectoryFolder(char* pPath)
	{
		size_t sz = GetCurrentDirectoryA(MAX_PATH, pPath);
		strcat_s(pPath, MAX_PATH, "\\");
		return sz;
	}

	size_t FileX::GetCurrentDirectoryFolder(wchar_t* pPath)
	{
		size_t sz = GetCurrentDirectoryW(MAX_PATH, pPath);
		wcscat_s(pPath, MAX_PATH, L"\\");
		return sz;
	}

	size_t FileX::GetCurrentDirectoryFolder(std::string& msPath)
	{
		msPath.resize(MAX_PATH);
		msPath.resize(GetCurrentDirectoryA(MAX_PATH, const_cast<char*>(msPath.data())));
		msPath.append("\\");
		return msPath.size();
	}

	size_t FileX::GetCurrentDirectoryFolder(std::wstring& wsPath)
	{
		wsPath.resize(MAX_PATH);
		wsPath.resize(GetCurrentDirectoryW(MAX_PATH, const_cast<wchar_t*>(wsPath.data())));
		wsPath.append(L"\\");
		return wsPath.size();
	}

	std::string FileX::GetCurrentDirectoryFolder_RETA()
	{
		std::string path;
		GetCurrentDirectoryFolder(path);
		return path;
	}

	std::wstring FileX::GetCurrentDirectoryFolder_RETW()
	{
		std::wstring path;
		GetCurrentDirectoryFolder(path);
		return path;
	}



	void FileX::BackSlash(const char* pPath, std::string& msPath)
	{
		msPath = pPath;
		BackSlash(const_cast<char*>(msPath.data()));
	}

	void FileX::BackSlash(const wchar_t* pPath, std::wstring& wsPath)
	{
		wsPath = pPath;
		BackSlash(const_cast<wchar_t*>(wsPath.data()));
	}

	std::string FileX::BackSlash_RET(const char* pPath)
	{
		std::string msPath = pPath;
		BackSlash(const_cast<char*>(msPath.data()));
		return msPath;
	}

	std::wstring FileX::BackSlash_RET(const wchar_t* pPath)
	{
		std::wstring wsPath = pPath;
		BackSlash(const_cast<wchar_t*>(wsPath.data()));
		return wsPath;
	}

	void FileX::BackSlash(char* pPath)
	{
		for (size_t ite = 0; pPath[ite]; ite++)
		{
			switch (pPath[ite])
			{
			case '/':
				pPath[ite] = '\\';
				break;
			case '\\':
				pPath[ite] = '/';
				break;
			}
		}
	}

	void FileX::BackSlash(wchar_t* pPath)
	{
		for (size_t ite = 0; pPath[ite]; ite++)
		{
			switch (pPath[ite])
			{
			case L'/':
				pPath[ite] = L'\\';
				break;
			case L'\\':
				pPath[ite] = L'/';
				break;
			}
		}
	}



	bool FileX::PathRemoveFileName(std::string& msPath, std::string& msRemoved)
	{
		size_t pos = msPath.rfind("\\");
		if (pos == std::wstring::npos) return false;
		msRemoved = msPath.substr(0, pos + 1);
		return true;
	}

	bool FileX::PathRemoveFileName(std::wstring& wsPath, std::wstring& wsRemoved)
	{
		size_t pos = wsPath.rfind(L"\\");
		if (pos == std::wstring::npos) return false;
		wsRemoved = wsPath.substr(0, pos + 1);
		return true;
	}

	std::string FileX::PathRemoveFileName_RET(std::string& msPath)
	{
		std::string folder;
		PathRemoveFileName(msPath, folder);
		return folder;
	}

	std::wstring FileX::PathRemoveFileName_RET(std::wstring& wsPath)
	{
		std::wstring folder;
		PathRemoveFileName(wsPath, folder);
		return folder;
	}



	bool FileX::PathGetFileName(std::string& msPath, std::string& msFileName)
	{
		size_t position = msPath.find_last_of('\\');
		if (position == std::string::npos) return false;

		msFileName = msPath.substr(position + 1);

		return true;
	}

	bool FileX::PathGetFileName(std::wstring& wsPath, std::wstring& wsFileName)
	{
		size_t position = wsPath.find_last_of('\\');
		if (position == std::string::npos) return false;

		wsFileName = wsPath.substr(position + 1);

		return true;
	}

	std::string FileX::PathGetFileName_RET(std::string& msPath)
	{
		std::string filename;
		PathGetFileName(msPath, filename);
		return filename;
	}

	std::wstring FileX::PathGetFileName_RET(std::wstring& wsPath)
	{
		std::wstring filename;
		PathGetFileName(wsPath, filename);
		return filename;
	}



	std::streamsize FileX::GetFileSize(const wchar_t* wsFile)
	{
		std::ifstream ifsFile(wsFile, std::ios::binary);
		if (!ifsFile.is_open()) return false;

		ifsFile.seekg(0, std::ios::end);
		std::streamsize szFile = ifsFile.tellg();

		return szFile;
	}

	std::streamsize FileX::GetFileSize(const char* msFile)
	{
		std::ifstream ifsFile(msFile, std::ios::binary);
		if (!ifsFile.is_open()) return false;

		ifsFile.seekg(0, std::ios::end);
		std::streamsize szFile = ifsFile.tellg();

		return szFile;
	}
}

