# Load dll filewatcher.dll
import ctypes
import os
import time

# Get the path of the dll file
dll_path = os.path.abspath('c_shared_memory.dll')
# Load the dll file
dll = ctypes.cdll.LoadLibrary(dll_path)

# define opaque type c_shared_memory


class c_shared_memory(ctypes.Structure):
    pass

# define struct A


class A(ctypes.Structure):
    _fields_ = [
        ('a', ctypes.c_int),
        ('b', ctypes.c_int),
    ]

# define all functions


# unsigned long get_last_error();
dll.get_last_error.argtypes = []
dll.get_last_error.restype = ctypes.c_ulong
# bool get_last_error_as_string(char* buffer, size_t buffer_size);
dll.get_last_error_as_string.argtypes = [ctypes.c_char_p, ctypes.c_size_t]
dll.get_last_error_as_string.restype = ctypes.c_bool
# c_shared_memory* c_shared_memory_create(const char* name, DWORD size);
dll.c_shared_memory_create.argtypes = [ctypes.c_char_p, ctypes.c_ulong]
dll.c_shared_memory_create.restype = ctypes.POINTER(c_shared_memory)
# void c_shared_memory_destroy(c_shared_memory* memory);
dll.c_shared_memory_destroy.argtypes = [ctypes.POINTER(c_shared_memory)]
dll.c_shared_memory_destroy.restype = None
# bool c_shared_memory_is_originator(const c_shared_memory* memory);
dll.c_shared_memory_is_originator.argtypes = [ctypes.POINTER(c_shared_memory)]
dll.c_shared_memory_is_originator.restype = ctypes.c_bool
# void* c_shared_memory_get(c_shared_memory* memory);
dll.c_shared_memory_get.argtypes = [ctypes.POINTER(c_shared_memory)]
dll.c_shared_memory_get.restype = ctypes.c_void_p
#  const void* c_shared_memory_const_get(const c_shared_memory* memory);
dll.c_shared_memory_const_get.argtypes = [ctypes.POINTER(c_shared_memory)]
dll.c_shared_memory_const_get.restype = ctypes.c_void_p


memory = dll.c_shared_memory_create(b'test', ctypes.sizeof(A))

if memory == ctypes.c_void_p(0):
    buffer = ctypes.create_string_buffer(1024)
    dll.get_last_error_as_string(buffer, 1024)
    print(f"Failed to create shared memory: {buffer.value.decode('utf-8')}")
    exit(1)

if dll.c_shared_memory_is_originator(memory) == ctypes.c_bool(True):
    data = ctypes.cast(dll.c_shared_memory_get(memory), ctypes.POINTER(A))
    data.contents.a = 1
    data.contents.b = 2
    print(f"Originator: Set data to {data.contents.a}, {data.contents.b}")
    print("Waiting for accessor to set data to 3, 4...")
    while data.contents.a != 3 or data.contents.b != 4:
        time.sleep(0.1)
    print(
        f"Originator: Accessor set data to {data.contents.a}, {data.contents.b}")
else:
    data = ctypes.cast(dll.c_shared_memory_get(memory), ctypes.POINTER(A))
    print(f"Accessor: Got data {data.contents.a}, {data.contents.b}")
    data.contents.a = 3
    data.contents.b = 4
    print(f"Accessor: Set data to {data.contents.a}, {data.contents.b}")

dll.c_shared_memory_destroy(memory)
