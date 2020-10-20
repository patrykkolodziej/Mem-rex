# Mem-rex
A simple class which allows us to reading and writing to the memory of another process.
# Simple examples
Example of reading memory from example process, then adding 5 to it and write it back.
```C++
#include "memory.hpp"
int main()
{
	std::unique_ptr<c_mem_rex> mem_rex = std::make_unique<c_mem_rex>("example_process.exe");
	const auto example_module = mem_rex->get_module("example_module.dll");		
	const uint32_t example_address = 0x58CA23;
  
	auto example_value = mem_rex->read<int>(example_module + example_address);
  
	example_value += 5;
  
	mem_rex->write<int>(example_module + example_address, example_value);
}
```


