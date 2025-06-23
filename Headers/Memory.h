#pragma once



class Memory
{
	public:
		static void Put(void* _Address, BYTE _Byte)
		{
			DWORD oldProtect;
			VirtualProtect(_Address, sizeof _Byte, PAGE_EXECUTE_READWRITE, &oldProtect);

			memset(_Address, _Byte, sizeof _Byte);

			VirtualProtect(_Address, sizeof _Byte, oldProtect, &oldProtect);
		}
};