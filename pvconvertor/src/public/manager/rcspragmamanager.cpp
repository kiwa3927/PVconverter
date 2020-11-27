
#include "rcspragmamanager.h"
#include <stdlib.h>


const char *rcsPragmaManager::warpper_getenv(const std::string &key) const
{
    const char *p = getenv(key.c_str());

    if (p == NULL)
    {
        std::map<std::string, std::string>::const_iterator it = m_envMap.find(key);
        if (it != m_envMap.end())
        {
            p = it->second.c_str();
        }
    }

    return p;
}
