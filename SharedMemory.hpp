// Copyright (c) Matthieu V All rights reserved.

#pragma once

namespace _private_shared_memory
{
	typedef struct c_shared_memory
	{
		void* hMapFile; // HANDLE
		void* pBuf;		// LPVOID
		char* name;
		unsigned long size; // DWORD
		bool is_originator; // True if this process created the shared memory, false if it just opened it
	} c_shared_memory;
} // namespace _private_shared_memory

#include <string>

static std::string getLastErrorAsString(size_t size = 1024);

template <typename T> class SharedMemory
{
public:
	SharedMemory(const char* name_);
	~SharedMemory();
	SharedMemory(const SharedMemory& other);
	SharedMemory& operator=(const SharedMemory& other);
	SharedMemory(SharedMemory&& other);

	bool isValid() const;
	bool isOriginator() const;

	T* get();
	const T* get() const;

private:
	std::string name;
	_private_shared_memory::c_shared_memory* memory;
};

// ============================================================
// ============================================================
// ===================== Implementations  =====================
// ============================================================
// ============================================================

// C Declarations
namespace _private_shared_memory
{
	static unsigned long get_last_error(); // DWORD

	// Returns false if the buffer was too small
	static bool get_last_error_as_string(char* buffer, size_t buffer_size);

	// Returns c_shared_memory object, or NULL if it failed
	// On failure, GetLastError() will return the error code, and get_last_error_as_string() will return the error message
	static c_shared_memory* c_shared_memory_create(const char* name, unsigned long size); // DWORD

	static void c_shared_memory_destroy(c_shared_memory* memory);

	static bool c_shared_memory_is_originator(const c_shared_memory* memory);

	static void* c_shared_memory_get(c_shared_memory* memory);
	static const void* c_shared_memory_const_get(const c_shared_memory* memory);
} // namespace _private_shared_memory

#include <cstring>

std::string getLastErrorAsString(size_t size)
{
	std::string result;
	result.resize(size);
	_private_shared_memory::get_last_error_as_string(&result[0], size);
	result.resize(strlen(result.data()));
	return result;
}

template <typename T>
SharedMemory<T>::SharedMemory(const char* name_) :
	name(name_), memory(_private_shared_memory::c_shared_memory_create(name.c_str(), sizeof(T)))
{
}

template <typename T> SharedMemory<T>::~SharedMemory()
{
	if (memory) _private_shared_memory::c_shared_memory_destroy(memory);
}

template <typename T> SharedMemory<T>::SharedMemory(const SharedMemory& other) : SharedMemory(other.name.c_str()) {}

template <typename T> SharedMemory<T>& SharedMemory<T>::operator=(const SharedMemory<T>& other)
{
	using namespace _private_shared_memory;
	if (this != &other)
	{
		c_shared_memory_destroy(memory);
		memory = c_shared_memory_create(other.name.c_str(), sizeof(T));
	}
	return *this;
}

template <typename T> SharedMemory<T>::SharedMemory(SharedMemory&& other) : name(std::move(other.name)), memory(other.memory)
{
	other.memory = nullptr;
}

template <typename T> bool SharedMemory<T>::isValid() const { return memory != nullptr; }

template <typename T> bool SharedMemory<T>::isOriginator() const
{
	return _private_shared_memory::c_shared_memory_is_originator(memory);
}

template <typename T> T* SharedMemory<T>::get() { return (T*)_private_shared_memory::c_shared_memory_get(memory); }

template <typename T> const T* SharedMemory<T>::get() const
{
	return (const T*)_private_shared_memory::c_shared_memory_get(memory);
}

// ============================================================
// ============================================================
// ===================== C Implementation  ====================
// ============================================================
// ============================================================

#include <Windows.h>
#include <stdio.h>

#include <stdio.h>

namespace _private_shared_memory
{
	unsigned long get_last_error() { return GetLastError(); }

	bool get_last_error_as_string(char* buffer, size_t buffer_size)
	{
		DWORD error_code = GetLastError();
		if (error_code == 0)
		{
			buffer[0] = '\0';
			return true; // No error message has been recorded
		}

		LPSTR message_buffer = NULL;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&message_buffer,
			0,
			NULL);

		int count = snprintf(buffer, buffer_size, "Error %ld: %s", error_code, message_buffer);

		LocalFree(message_buffer);

		return count > 0; // Returns false if the buffer was too small
	}

	c_shared_memory* c_shared_memory_create(const char* name, DWORD size)
	{
		c_shared_memory* result = (c_shared_memory*)malloc(sizeof(c_shared_memory));
		result->name = strdup(name);
		result->size = size;

		result->hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, // use paging file
			NULL,													// default security
			PAGE_READWRITE,											// read/write access
			0,														// maximum object size (high-order DWORD)
			size,													// maximum object size (low-order DWORD)
			name													// name of mapping object
		);

		if (result->hMapFile == NULL)
		{
			free(result);
			return NULL;
		}

		result->pBuf = MapViewOfFile(result->hMapFile,
			FILE_MAP_ALL_ACCESS, // read/write access
			0,
			0,
			size);

		if (result->pBuf == NULL)
		{
			CloseHandle(result->hMapFile);
			free(result);
			return NULL;
		}

		result->is_originator = GetLastError() != ERROR_ALREADY_EXISTS;
		return result;
	}

	void c_shared_memory_destroy(c_shared_memory* memory)
	{
		free(memory->name);
		UnmapViewOfFile(memory->pBuf);
		CloseHandle(memory->hMapFile);
		free(memory);
	}

	bool c_shared_memory_is_originator(const c_shared_memory* memory) { return memory->is_originator; }
	void* c_shared_memory_get(c_shared_memory* memory) { return memory->pBuf; }
	const void* c_shared_memory_const_get(const c_shared_memory* memory) { return memory->pBuf; }

} // namespace _private_shared_memory
