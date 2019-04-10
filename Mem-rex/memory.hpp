#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <memory>


class c_memory {
private:
	uint32_t process_id;
	HANDLE process_handle;
public:
	bool get_process_id(const std::string process_name);
	bool get_handle(const std::string process_name);
	uint32_t get_module(const std::string module_name);
	bool free_memory(const uint32_t address, const size_t size);
	uint32_t allocate_memory(const size_t size);

	template<typename t>
	t read(const uint32_t address)
	{
		t out;

		ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(address), &out, sizeof(t), NULL);

		return out;
	}

	template<typename t>
	void write(const uint32_t address, const t in)
	{
		WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(address), &in, sizeof(t), NULL);
	}

	template<typename t>
	void write_protected(const uint32_t address, const t in)
	{
		uint32_t old;
		VirtualProtectEx(process_handle, reinterpret_cast<LPCVOID>(address), sizeof(t), PAGE_EXECUTE_READWRITE, &old);
		write(address, in);
		VirtualProtectEx(process_handle, reinterpret_cast<LPCVOID>(address), sizeof(t), old, NULL);
	}
};
extern std::unique_ptr<c_memory> g_pmemory;