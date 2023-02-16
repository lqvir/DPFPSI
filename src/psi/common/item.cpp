#include "item.h"
#include <cstring>
namespace PSI{
    std::string Item::to_string() const
    {
        return util::to_string(get_as<uint32_t>());
    }
}
