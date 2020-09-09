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
    QString apiAuth = "IRLTKAPI " + conf->APIKey;
    getHttpPtr()->addRequestHeader("Authorization", apiAuth.toUtf8());

	conf->Save();
#ifdef DEBUG_MODE
    blog(LOG_INFO, "Saved settings!");
#endif
}

void SettingsDialog::DialogButtonClicked(QAbstractButton *button) {
    QDialogButtonBox::ButtonRole signalButton = ui->buttonBox->buttonRole(button);
    if (signalButton == QDialogButtonBox::ApplyRole || signalButton == QDialogButtonBox::AcceptRole) {
        QRegExpValidator keyValidator = QRegExpValidator(QRegExp("[A-Fa-f0-9]{64}"));
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
    auto conf = GetConfig();
    //auto reply = getHttpPtr()->get(QString("https://api.irl.run/v1/server/status"));
    auto reply = getHttpPtr()->get(QString("http://www.google.com"));
    connect(reply, &HttpReply::finished, this, [this](auto &reply) {
        if (reply.isSuccessful()) {
            //obs_data_t *statusData = obs_data_create_from_json(reply.body().toStdString().c_str());
            //SettingsDialog::SetServerStatusIndicator(obs_data_get_bool(statusData, "ready"));
            //obs_data_release(statusData);
            qDebug() << reply.body();
        } else {
            if (reply.statusCode() == 401) {
                QMessageBox msgBox;
                msgBox.setWindowTitle(obs_module_text("IRLTKHotSwitcher.Panel.ErrorTitle"));
                msgBox.setText(obs_module_text("IRLTKHotSwitcher.Panel.401Error"));
                msgBox.exec();
            } else {
                QMessageBox msgBox;
                msgBox.setWindowTitle(obs_module_text("IRLTKHotSwitcher.Panel.ErrorTitle"));
                msgBox.setText(QString(obs_module_text("IRLTKHotSwitcher.Panel.UnknownHTTPError")).arg(reply.statusCode()));
                msgBox.exec();
            }
#ifdef DEBUG_MODE
            blog(LOG_INFO, "API Error! Code: %d | Phrase: %s", reply.statusCode(), reply.reasonPhrase().toStdString().c_str());
#endif
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