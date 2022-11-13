// Copyright (c) Matthieu V All rights reserved.

#include "c_shared_memory.h"
#include <stdio.h>

struct c_shared_memory
{
	HANDLE hMapFile;
	LPVOID pBuf;
	char* name;
	DWORD size;
	bool is_originator; // True if this process created the shared memory, false if it just opened it
};

DWORD get_last_error() { return GetLastError(); }

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
