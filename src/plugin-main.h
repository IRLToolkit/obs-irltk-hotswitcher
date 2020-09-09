#include <obs.hpp>
#include <memory>
#include "../lib/http/src/http.h"

#define DEBUG_MODE

class Config;
typedef std::shared_ptr<Config> ConfigPtr;

ConfigPtr GetConfig();

Http *getHttpPtr();