#include <obs-module.h>
#include <iostream>
#include <obs-frontend-api.h>
#include <QString>
#include <QAbstractButton>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QIcon>
#include <QPixmap>

#include "../plugin-main.h"
#include "../Config.h"
#include "../HttpManager.h"
#include "settings-dialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) :
	QDialog(parent, Qt::Dialog),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked,
		this, &SettingsDialog::DialogButtonClicked);
	connect(ui->apiRefreshButton, &QPushButton::clicked,
		this, &SettingsDialog::RefreshStatus);
}

void SettingsDialog::showEvent(QShowEvent* event) {
	auto conf = GetConfig();

	ui->apiKey->setText(conf->APIKey);
	SettingsDialog::SetServerStatusIndicator();
	SettingsDialog::RefreshStatus();
}

void SettingsDialog::ToggleShowHide() {
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}

void SettingsDialog::SaveSettings() {
	auto conf = GetConfig();
	conf->APIKey = ui->apiKey->text();

	auto httpManager = GetHttpManager();
	httpManager->SetApiKey(ui->apiKey->text());

	conf->Save();
#ifdef DEBUG_MODE
	blog(LOG_INFO, "Settings saved!");
#endif
}

void SettingsDialog::DialogButtonClicked(QAbstractButton *button) {
	QDialogButtonBox::ButtonRole signalButton = ui->buttonBox->buttonRole(button);
	if (signalButton == QDialogButtonBox::ApplyRole || signalButton == QDialogButtonBox::AcceptRole) {
		QRegExpValidator keyValidator = QRegExpValidator(QRegExp("[A-Za-z0-9]{64}"));
		int pos = 0;
		QString apiKeyText = ui->apiKey->text();
		if (!(keyValidator.validate(apiKeyText, pos) == QValidator::Acceptable)) {
			QMessageBox msgBox;
			msgBox.setWindowTitle(obs_module_text("IRLTKHotSwitcher.Panel.ErrorTitle"));
			msgBox.setText(obs_module_text("IRLTKHotSwitcher.Panel.InvalidAPIKey"));
			msgBox.exec();
			return;
		}
		SettingsDialog::SaveSettings();
		if (signalButton == QDialogButtonBox::AcceptRole) {
			QDialog::accept();
		}
	}
}

SettingsDialog::~SettingsDialog() {
	delete ui;
}

void SettingsDialog::RefreshStatus() {
	auto httpManager = GetHttpManager();
	httpManager->GetIrltkServerStatus([this] (obs_data_t *reply) {
		if (obs_data_get_bool(reply, "status")) {
			SettingsDialog::SetServerStatusIndicator(obs_data_get_bool(reply, "serverStatus"));
		} else {
			QMessageBox msgBox;
				msgBox.setWindowTitle(obs_module_text("IRLTKHotSwitcher.Panel.ErrorTitle"));
				int statusCode = obs_data_get_int(reply, "statusCode");
				QString comment = obs_data_get_string(reply, "comment");
				msgBox.setText(QString(obs_module_text("IRLTKHotSwitcher.Panel.HTTPError")).arg(statusCode).arg(comment));
				msgBox.exec();
		}
	});
}

void SettingsDialog::SetServerStatusIndicator(bool active) {
	if (active) {
		ui->serverStatusIndicator->setPixmap(QPixmap(":/logos/checkmark"));
	} else {
		ui->serverStatusIndicator->setPixmap(QPixmap(":/logos/times"));
	}
}