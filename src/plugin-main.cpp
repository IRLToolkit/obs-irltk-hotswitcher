#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs-data.h>

#include <QtCore/QTimer>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QSslSocket>

#include "Config.h"
#include "HttpManager.h"
#include "forms/settings-dialog.h"

#include "plugin-main.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

ConfigPtr _config;

HttpManagerPtr httpManager;

bool obs_module_load(void)
{
	_config = ConfigPtr(new Config());
	_config->Load();

	httpManager = HttpManagerPtr(new HttpManager());
	httpManager->SetApiKey(_config->APIKey);

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

#ifdef DEBUG_MODE
	if (QSslSocket::supportsSsl()) {
		blog(LOG_INFO, "SSL is supported.");
		blog(LOG_INFO, "QT SSL version: '%s'", QSslSocket::sslLibraryBuildVersionString().toStdString().c_str());
	} else {
		blog(LOG_ERROR, "SSL is not supported!");
	}
#endif
	blog(LOG_INFO, "Plugin loaded successfully (version %s)", PLUGIN_VERSION);
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

HttpManagerPtr GetHttpManager() {
	return httpManager;
}