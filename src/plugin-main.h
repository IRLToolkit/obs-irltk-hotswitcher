#include <obs.hpp>
#include <memory>
#include "../lib/http/src/http.h"
#include "../lib/json/single_include/nlohmann/json.hpp"
#include "plugin-macros.generated.h"

#define DEBUG_MODE

#define QT_TO_UTF8(str) str.toUtf8().constData()

class Config;
typedef std::shared_ptr<Config> ConfigPtr;

class HttpManager;
typedef std::shared_ptr<HttpManager> HttpManagerPtr;

ConfigPtr GetConfig();

HttpManagerPtr GetHttpManager();