#include "Infrastructure/PathUtil.h"

#include <string>

#include <Windows.h>

namespace SimpleRockets::Launcher::Infrastructure
{
    std::filesystem::path ModuleDirectory()
    {
        DWORD capacity = MAX_PATH;
        while (true)
        {
            std::wstring buffer(capacity, L'\0');
            const DWORD written = GetModuleFileNameW(nullptr, buffer.data(), capacity);
            if (written == 0)
            {
                return std::filesystem::path();
            }

            if (written < capacity)
            {
                buffer.resize(written);
                return std::filesystem::path(buffer).parent_path();
            }

            capacity *= 2;
        }
    }
}
