#pragma once



class Pattern
{
	public:
		Pattern() = default;
		explicit Pattern(std::uintptr_t _Ptr) : m_Handle(reinterpret_cast<void*>(_Ptr)), m_Name(nullptr), m_Pattern(nullptr) {}
		explicit Pattern(const char* _Name, const char* _Pattern) : m_Handle(nullptr), m_Name(_Name), m_Pattern(_Pattern) {}

		void Scan(std::function<void(const Pattern&)> _Callback)
		{
			std::vector<BYTE> patternBytes;
			std::istringstream iss(m_Pattern);
			std::string w;

			while (iss >> w)
			{
				const char* data = w.data();

				if (data[0] == '?')
				{
					patternBytes.push_back(NULL);
				}
				else if (w.length() == 2 && isxdigit(data[0]) && isxdigit(data[1]))
				{
					patternBytes.push_back(StringToByte(w));
				}
				else
				{
					return;
				}
			}

			MODULEINFO moduleInfo;
			GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &moduleInfo, sizeof MODULEINFO);

			std::uintptr_t moduleStart = std::uintptr_t(moduleInfo.lpBaseOfDll);

			for (DWORD i = 0; i < moduleInfo.SizeOfImage; i++)
			{
				BYTE* data = reinterpret_cast<BYTE*>(moduleStart + i);

				bool found = true;

				for (size_t j = 0; j < patternBytes.size(); j++)
				{
					if (patternBytes[j] != NULL && data[j] != patternBytes[j])
					{
						found = false;

						break;
					}
				}

				if (found)
				{
					m_Handle = data;

					_Callback(*this);

					return;
				}
			}
		}

		template <typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> As() const
		{
			return static_cast<T>(m_Handle);
		}

		template <typename T>
		std::enable_if_t<std::is_lvalue_reference_v<T>, T> As() const
		{
			return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(m_Handle);
		}

		template <typename T>
		std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> As() const
		{
			return reinterpret_cast<std::uintptr_t>(m_Handle);
		}

		template <typename T>
		Pattern Add(T _Offset) const
		{
			return Pattern(this->As<std::uintptr_t>() + _Offset);
		}

		void Put(BYTE _Value) const
		{
			return Memory::Put(m_Handle, _Value);
		}

	private:
		const char* m_Name;
		const char* m_Pattern;
		void* m_Handle;

		static BYTE StringToByte(const std::string& _str)
		{
			std::istringstream iss(_str);
			uint32_t value = 0;

			iss >> std::hex >> value;

			return static_cast<BYTE>(value);
		}
};