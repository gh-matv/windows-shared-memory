# Windows Shared Memory

# Description

Windows Shared Memory is a **c library** that allows you to **create memory shared between multiple processes**.

# Features

- create shared memory
- get last error as string
- check if shared memory already exists
- release shared memory handle on destroy (cpp only)
- typed shared memory (cpp only)
- exported c functions for use in other languages (see example: [py_main.py](py_main.py))

# Installation

*(Windows only)*

### Header only:

Include the [`SharedMemory.hpp`](SharedMemory.hpp) where you want to use the Shared Memory *(use [`c_shared_memory_header_only.h`](c_shared_memory_header_only.h) for the c version)*.

### Header and source:

Include the [`c_shared_memory.h`](c_shared_memory.h) where you want to use the Shared Memory.  
Compile with the [`c_shared_memory.c`](c_shared_memory.c) file or with the built dll available [here](https://github.com/nicolasventer/File-Watcher/releases).

### Build the dll:

```bash
g++ -shared -fPIC -static -o c_shared_memory.dll c_shared_memory.c
```

Note: the `-static` flag is required.

### Requirements

c++11 or later required for compilation.  
No external dependencies.

# Example

*Content of [main.cpp](main.cpp):*
```cpp
#include <iostream>

#include "SharedMemory.hpp"
#include "Windows.h" // For Sleep()

typedef struct A
{
	int a;
	int b;
} A;

int main()
{
	SharedMemory<A> memory("test");
	if (!memory.isValid())
	{
		std::cout << "Failed to create shared memory: " << getLastErrorAsString() << std::endl;
		return 1;
	}

	if (memory.isOriginator())
	{
		A* data = memory.get();
		data->a = 1;
		data->b = 2;
		printf("Originator: Set data to %d, %d\n", data->a, data->b);
		printf("Waiting for accessor to set data to 3, 4...\n");
		while (data->a != 3 || data->b != 4) Sleep(100);
		printf("Originator: Accessor set data to %d, %d\n", data->a, data->b);
	}
	else
	{
		A* data = memory.get();
		printf("Accessor: Got data %d, %d\n", data->a, data->b);
		A new_data = {3, 4};
		*data = new_data; // other way to set data
		printf("Accessor: Set data to %d, %d\n", data->a, data->b);
	}

	return 0;
}
```

Output 1st run:
```
Originator: Set data to 1, 2
Waiting for accessor to set data to 3, 4...
Originator: Accessor set data to 3, 4
```

Output 2nd run:
```
Accessor: Got data 1, 2
Accessor: Set data to 3, 4
```

# Usage

```cpp	
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
};
```

# Licence

MIT Licence. See [LICENSE file](LICENSE).
Please refer me with:

	Copyright (c) Matthieu V All rights reserved.
