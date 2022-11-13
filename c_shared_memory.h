// Copyright (c) Matthieu V All rights reserved.

#pragma once

#include <Windows.h>
#include <stdbool.h>

#define EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct c_shared_memory c_shared_memory;

	EXPORT DWORD get_last_error();

	// Returns false if the buffer was too small
	EXPORT bool get_last_error_as_string(char* buffer, size_t buffer_size);

	// Returns c_shared_memory object, or NULL if it failed
	// On failure, get_last_error() will return the error code, and get_last_error_as_string() will return the error message
	EXPORT c_shared_memory* c_shared_memory_create(const char* name, DWORD size);

	EXPORT void c_shared_memory_destroy(c_shared_memory* memory);

	EXPORT bool c_shared_memory_is_originator(const c_shared_memory* memory);

	EXPORT void* c_shared_memory_get(c_shared_memory* memory);
	EXPORT const void* c_shared_memory_const_get(const c_shared_memory* memory);

#ifdef __cplusplus
}
#endif
