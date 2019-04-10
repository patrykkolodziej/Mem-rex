#include "memory.hpp"
std::unique_ptr<c_memory> g_pmemory = std::make_unique<c_memory>();

bool c_memory::get_process_id(const std::string process_name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			if (entry.szExeFile == process_name)
			{
				process_id = entry.th32ProcessID;
				CloseHandle(snapshot);
				return true;
			}
		}
	}

	return false;
}

bool c_memory::get_handle(const std::string process_name)
{

	if (!process_id)
		return false;

	const auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, process_id);

	if (!handle)
		return false;

	process_handle = handle;

	return true;
}

uint32_t c_memory::get_module(const std::string module_name)
{
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(entry);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);

	if (Module32First(snapshot, &entry))
	{
		while (Module32Next(snapshot, &entry))
		{
			if (entry.szModule == module_name)
			{
				CloseHandle(snapshot);
				return reinterpret_cast<uint32_t>(entry.modBaseAddr);
			}
		}
	}

	return NULL;
}

bool c_memory::free_memory(const uint32_t address, const size_t size)
{
	return VirtualFreeEx(process_handle, reinterpret_cast<LPVOID>(address), size, MEM_RELEASE);
}

uint32_t c_memory::allocate_memory(const size_t size)
{
	return reinterpret_cast<uint32_t>(VirtualAllocEx(process_handle, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
}

