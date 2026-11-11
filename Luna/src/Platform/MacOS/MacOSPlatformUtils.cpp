#include <lunapch.h>
#include "Luna/Utils/PlatformUtils.h"

namespace Luna {

    std::string FileDialogs::OpenFile(const char* filter)
    {
        LUNA_CORE_ASSERT(false, "NO SUPPORT FOR MACOS AS OF NOW!");
        return std::string();
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        LUNA_CORE_ASSERT(false, "NO SUPPORT FOR MACOS AS OF NOW!");
        return std::string();
    }

}
