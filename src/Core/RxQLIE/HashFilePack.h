#pragma once
#include <RxQLIE/Cryptor.h>


namespace ZQF::RxQLIE
{
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
