#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <memory>
#include <vector>

struct module_t
{
	std::string module_name;
	uint32_t module_address;
};

class c_mem_rex
{
private:
	std::string process_name;
	uint32_t process_id;
	HANDLE process_handle;
	std::vector<module_t> modules;

	bool get_process_id()
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

		CloseHandle(snapshot);
		throw std::exception("Couldn't get process id. Is proccess runing?");
		return false;
	}

	bool get_handle()
	{

		process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, process_id);

		if (!process_handle)
		{
			CloseHandle(process_handle);
			throw std::exception("Couldn't get process handle.");
			return false;
		}

		return true;
	}

	bool get_modules()
	{
		MODULEENTRY32 entry;
		entry.dwSize = sizeof(entry);
		module_t current_module;
		const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);

		if (Module32First(snapshot, &entry))
		{
			while (Module32Next(snapshot, &entry))
			{
				current_module.module_name = entry.szModule;
				current_module.module_address = reinterpret_cast<uint32_t>(entry.modBaseAddr);
				modules.push_back(current_module);
			}
		}
		else
		{
			CloseHandle(snapshot);
			throw std::exception("Couldn't find any modules in process.");
			return false;
		}

		CloseHandle(snapshot);
		return true;
	}
public:
	c_mem_rex(const std::string process)
	{
		process_name = process;
		process_id = NULL;
		process_handle = nullptr;
		try
		{
			if (get_process_id())
				if (get_handle())
					get_modules();
		}
		catch (std::exception& e)
		{
			throw std::exception(e.what());
		}
	}

	~c_mem_rex()
	{
		CloseHandle(process_handle);
	}

	bool free_memory(const uint32_t address, const size_t size)
	{
		return VirtualFreeEx(process_handle, reinterpret_cast<LPVOID>(address), size, MEM_RELEASE);
	}

	uint32_t allocate_memory(const size_t size)
	{
		return reinterpret_cast<uint32_t>(VirtualAllocEx(process_handle, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	}

	uint32_t get_module(const std::string module_name)
	{
		for (auto& s: modules)
		{
			if (s.module_name == module_name)
				return s.module_address;
		}
	}

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
extern std::unique_ptr<c_mem_rex> mem_rex;
