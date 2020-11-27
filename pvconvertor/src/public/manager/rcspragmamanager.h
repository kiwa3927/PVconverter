

#ifndef RCSPRAGMAMANAGER_H
#define RCSPRAGMAMANAGER_H

#include <string>
#include <map>

class rcsPragmaManager
{
public:
    void clear() { m_envMap.clear(); }
    bool insertEnvVar(const std::string &key , const std::string &val);
    const char *warpper_getenv(const std::string &key) const;
private:
    std::map<std::string, std::string> m_envMap;
};

inline
bool rcsPragmaManager::insertEnvVar(const std::string &key, const std::string &val)
{
    
    return m_envMap.insert(std::make_pair(key, val)).first->second == val;
}

#endif 
