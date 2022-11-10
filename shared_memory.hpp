
#pragma once

#include <Windows.h>

#include <string>

class shared_memory
{
private:

	std::string name;
	bool is_valid = false;

	// Internal
	HANDLE hMapFile;
	LPCTSTR pBuf;

public:

	enum create_shared_memory_result
	{
		cant_create,   // First so we can use an if : cant_create == 0
		is_server,
		is_client, 		
	};

	create_shared_memory_result create_shared_memory()
	{
		hMapFile = CreateFileMappingA(
			INVALID_HANDLE_VALUE,	// use paging file
			NULL,					// default security
			PAGE_READWRITE,			// read/write access
			0,						// maximum object size (high-order DWORD)
			1024,					// maximum object size (low-order DWORD)
			name.c_str()			// name of mapping object
		);

		if (hMapFile == NULL)
		{
			std::cout << "Could not create file mapping object (" << GetLastError() << ")." << std::endl;
			std::cout << GetLastErrorAsString() << std::endl;
			return cant_create;
		}

		return GetLastError() == ERROR_ALREADY_EXISTS ? is_client : is_server;
	}

	bool map_file(DWORD size)
	{
		pBuf = (LPCTSTR)MapViewOfFile(hMapFile,
			FILE_MAP_ALL_ACCESS,   // read/write access
			0,
			0,
			size);

		std::cout << "pBuf=" << pBuf << std::endl;

		if (pBuf == NULL)
		{
			std::cout << "Could not map view of file (" << GetLastError() << ")." << std::endl;
			std::cout << GetLastErrorAsString() << std::endl;
			CloseHandle(hMapFile);
			return false;
		}

		return true;
	}

	shared_memory(const std::string name, DWORD size = 1024) : name(name)
	{
		create_shared_memory_result csm;
		bool mff;

		is_valid = (csm = create_shared_memory()) && (mff = map_file(size)); 

	}

	~shared_memory()
	{
		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
	}
	
};


template<typename T>
class bound_shared_memory : public shared_memory
{
public:
	// Server
	bound_shared_memory(const std::string name, const T& object) : shared_memory(name, (DWORD)sizeof(T))
	{
		memcpy((void*)pBuf, &object, sizeof(object));
	}

	// Client
	bound_shared_memory(const std::string name) : shared_memory(name, (DWORD)sizeof(T))
	{
	}

public:
	T* operator->()
	{
		return (T*)pBuf;
	}

	T& operator=(const T& object)
	{
		memcpy((void*)pBuf, &object, sizeof(object));
		return (T&)pBuf;
	}

	operator T()
	{
		return (T&)pBuf;
	}
};
