#include "Infrastructure/PipePair.h"

namespace SimpleRockets::Launcher::Infrastructure
{
    PipePair::PipePair()
        : readEnd(nullptr)
        , writeEnd(nullptr)
    {
        SECURITY_ATTRIBUTES attributes = {};
        attributes.nLength = sizeof(attributes);
        attributes.bInheritHandle = TRUE;

        if (CreatePipe(&readEnd, &writeEnd, &attributes, 0) == FALSE)
        {
            readEnd = nullptr;
            writeEnd = nullptr;
            return;
        }

        SetHandleInformation(writeEnd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        SetHandleInformation(readEnd, HANDLE_FLAG_INHERIT, 0);
    }

    PipePair::~PipePair()
    {
        Close();
    }

    Boolean PipePair::IsValid() const
    {
        return readEnd != nullptr && writeEnd != nullptr ? True : False;
    }

    HANDLE PipePair::Read() const
    {
        return readEnd;
    }

    HANDLE PipePair::Write() const
    {
        return writeEnd;
    }

    void PipePair::Close()
    {
        if (writeEnd != nullptr)
        {
            CloseHandle(writeEnd);
            writeEnd = nullptr;
        }

        if (readEnd != nullptr)
        {
            CloseHandle(readEnd);
            readEnd = nullptr;
        }
    }
}
