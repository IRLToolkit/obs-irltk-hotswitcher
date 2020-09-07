#include <obs.hpp>
#include <memory>

class Config;
typedef std::shared_ptr<Config> ConfigPtr;

ConfigPtr GetConfig();