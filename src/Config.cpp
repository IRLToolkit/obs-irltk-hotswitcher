#include <obs-frontend-api.h>

#include <QtCore/QCryptographicHash>
#include <QtCore/QTime>
#include <QtWidgets/QSystemTrayIcon>

#define SECTION_NAME "IRLTKHotswitcher"
#define PARAM_APIKEY "APIKey"

#include "plugin-main.h"
#include "Config.h"

#define QT_TO_UTF8(str) str.toUtf8().constData()

Config::Config() :
	APIKey(""),
	SettingsLoaded(false)
{
	qsrand(QTime::currentTime().msec());

	SetDefaults();

	obs_frontend_add_event_callback(OnFrontendEvent, this);
}

Config::~Config()
{
	obs_frontend_remove_event_callback(OnFrontendEvent, this);
}

void Config::Load()
{
	config_t* obsConfig = GetConfigStore();

	APIKey = config_get_string(obsConfig, SECTION_NAME, PARAM_APIKEY);
#ifdef DEBUG_MODE
    blog(LOG_INFO, "API Key: %s", APIKey.toStdString().c_str());
#endif
}

void Config::Save()
{
	config_t* obsConfig = GetConfigStore();

	config_set_string(obsConfig, SECTION_NAME, PARAM_APIKEY,
		QT_TO_UTF8(APIKey));

	config_save(obsConfig);
}

void Config::SetDefaults()
{
	// OBS Config defaults
	config_t* obsConfig = GetConfigStore();
	if (obsConfig) {
		config_set_default_string(obsConfig,
			SECTION_NAME, PARAM_APIKEY, QT_TO_UTF8(APIKey));
	}
}

config_t* Config::GetConfigStore()
{
	return obs_frontend_get_profile_config();
}

void Config::OnFrontendEvent(enum obs_frontend_event event, void* param)
{
	auto config = reinterpret_cast<Config*>(param);

	if (event == OBS_FRONTEND_EVENT_PROFILE_CHANGED) {
		config->SetDefaults();
		config->Load();
	}
}