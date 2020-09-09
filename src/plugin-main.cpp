#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs-data.h>

#include <QtCore/QTimer>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>

#include "Config.h"
#include "forms/settings-dialog.h"

#include "plugin-main.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

ConfigPtr _config;

Http *httpPtr;

bool obs_module_load(void)
{
    _config = ConfigPtr(new Config());
    _config->Load();
    
    obs_frontend_push_ui_translation(obs_module_get_string);
    QMainWindow* mainWindow = (QMainWindow*)obs_frontend_get_main_window();
	SettingsDialog* settingsDialog = new SettingsDialog(mainWindow);
    obs_frontend_pop_ui_translation();
    
    const char* menuActionText = obs_module_text("IRLTKHotSwitcher.Panel.DialogTitle");
	QAction* menuAction = (QAction*)obs_frontend_add_tools_menu_qaction(menuActionText);
	QObject::connect(menuAction, &QAction::triggered, [settingsDialog] {
		// The settings dialog belongs to the main window. Should be ok
		// to pass the pointer to this QAction belonging to the main window
		settingsDialog->ToggleShowHide();
	});
    QString apiAuth = "IRLTKAPI " + _config->APIKey;
    QString userAgent = QString("irltk-hotswitcher/%1 (obs-studio)").arg(PLUGIN_VERSION);
    httpPtr = new Http;
#ifdef DEBUG_MODE
    blog(LOG_INFO, "Using agent: '%s'", userAgent.toStdString().c_str());
    blog(LOG_INFO, "Using authorization: '%s'", apiAuth.toStdString().c_str());
#endif
    httpPtr->addRequestHeader("User-Agent", userAgent.toUtf8());
    httpPtr->addRequestHeader("Authorization", apiAuth.toUtf8());
    blog(LOG_INFO, "Loaded successfully (version %s)", PLUGIN_VERSION);
    return true;
}

void obs_module_unload()
{
    _config.reset();
    blog(LOG_INFO, "Finished unloading.");
}

ConfigPtr GetConfig() {
	return _config;
}

Http *getHttpPtr() {
    return httpPtr;
}