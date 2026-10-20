#include "Infrastructure/FileHash.h"

#include <Windows.h>
#include <bcrypt.h>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure
{
    namespace
    {
        constexpr UInt32 ReadChunkBytes = 64 * 1024;

        struct HashResources
        {
            BCRYPT_ALG_HANDLE algorithm = nullptr;
            BCRYPT_HASH_HANDLE hash = nullptr;
            PUCHAR object = nullptr;
            PUCHAR digest = nullptr;
            HANDLE file = INVALID_HANDLE_VALUE;

            ~HashResources()
            {
                if (hash != nullptr)
                {
                    BCryptDestroyHash(hash);
                }
                if (algorithm != nullptr)
                {
                    BCryptCloseAlgorithmProvider(algorithm, 0);
                }
                if (object != nullptr)
                {
                    HeapFree(GetProcessHeap(), 0, object);
                }
                if (digest != nullptr)
                {
                    HeapFree(GetProcessHeap(), 0, digest);
                }
                if (file != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(file);
                }
            }
        };

        [[nodiscard]] std::wstring ToHex(const Byte* data, UInt32 length)
        {
            constexpr wchar_t Digits[] = L"0123456789ABCDEF";

            std::wstring text(length * 2, L'\0');
            for (UInt32 index = 0; index < length; ++index)
            {
                text[index * 2] = Digits[data[index] >> 4];
                text[index * 2 + 1] = Digits[data[index] & 0x0F];
            }
            return text;
        }
    }

    Boolean ComputeSha256(const std::filesystem::path& path, std::wstring& hexDigest)
    {
        HashResources resources;
        resources.file = CreateFileW(
            path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (resources.file == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        if (BCryptOpenAlgorithmProvider(&resources.algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0)
        {
            return false;
        }

        DWORD objectLength = 0;
        DWORD digestLength = 0;
        DWORD written = 0;
        if (BCryptGetProperty(resources.algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectLength),
                sizeof(objectLength), &written, 0) < 0)
        {
            return false;
        }
        if (BCryptGetProperty(resources.algorithm, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&digestLength),
                sizeof(digestLength), &written, 0) < 0)
        {
            return false;
        }

        resources.object = static_cast<PUCHAR>(HeapAlloc(GetProcessHeap(), 0, objectLength));
        resources.digest = static_cast<PUCHAR>(HeapAlloc(GetProcessHeap(), 0, digestLength));
        if (resources.object == nullptr || resources.digest == nullptr)
        {
            return false;
        }

        if (BCryptCreateHash(resources.algorithm, &resources.hash, resources.object, objectLength, nullptr, 0, 0) < 0)
        {
            return false;
        }

        Byte chunk[ReadChunkBytes];
        while (true)
        {
            DWORD read = 0;
            if (!ReadFile(resources.file, chunk, ReadChunkBytes, &read, nullptr))
            {
                if (GetLastError() == ERROR_HANDLE_EOF)
                {
                    break;
                }
                return false;
            }
            if (read == 0)
            {
                break;
            }

            if (BCryptHashData(resources.hash, chunk, read, 0) < 0)
            {
                return false;
            }
        }

        if (BCryptFinishHash(resources.hash, resources.digest, digestLength, 0) < 0)
        {
            return false;
        }

        hexDigest = ToHex(resources.digest, digestLength);
        return true;
    }
}
