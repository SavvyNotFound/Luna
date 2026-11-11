#pragma once

#include <string>

namespace Luna {

    class FileDialogs
    {
        public:
            // Both return empty str if cancelled
            static std::string OpenFile(const char* filter);
            static std::string SaveFile(const char* filter);
    };

}
