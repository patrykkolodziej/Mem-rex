# Mem-rex
A simple class which allows us to reading and writing to the memory of another process.
# Simple examples
Example of reading memory from example process, then adding 5 to it and write it back.
```C++
#include "memory.hpp"
int main()
{
	const auto got_id = g_pmemory->get_process_id("example_process.exe");
	
	if(!got_id)
		return 0;
		
	const auto got_handle = g_pmemory->get_handle("example_process.exe");
  
  	if(!got_handle)
		return 0;
	
	const uint32_t example_address = 0x58CA23;
  
	auto example_value = g_pmemory->read<int>(example_address);
  
	example_value += 5;
  
	g_pmemory->write<int>(example_address, example_value);
}
```

```C++
#include "memory.hpp"
int main()
{
	const auto got_id = g_pmemory->get_process_id("example_process.exe");
	
	if(!got_id)
		return 0;
		
	const auto got_handle = g_pmemory->get_handle("example_process.exe");
  
  	if(!got_handle)
		return 0;

	const uint32_t example_address = 0x58CA23;
	const auto example_module = g_pmemory->get_module("example_module.dll");

	auto example_int = g_pmemory->read<int>(example_address);
	auto example_float = g_pmemory->read<float>(example_module + example_address);

	example_int += 5;
	example_float = example_int * example_float / 2;

	g_pmemory->write<int>(example_address, example_int);
	g_pmemory->write<float>(example_module + example_address, example_float);
}

```

