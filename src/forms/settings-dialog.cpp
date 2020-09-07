#include <obs-frontend-api.h>

#include "../plugin-main.h"
#include "../Config.h"
#include "settings-dialog.h"

#define CHANGE_ME "changeme"

SettingsDialog::SettingsDialog(QWidget* parent) :
	QDialog(parent, Qt::Dialog),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::accepted,
		this, &SettingsDialog::FormAccepted);
}

void SettingsDialog::showEvent(QShowEvent* event) {
	auto conf = GetConfig();

	ui->apiKey->setText(conf->APIKey);
}

void SettingsDialog::ToggleShowHide() {
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}

void SettingsDialog::FormAccepted() {
	auto conf = GetConfig();

    if (ui->apiKey->text() != CHANGE_ME) {
		conf->APIKey = ui->apiKey->text();
	}

	conf->Save();
}

SettingsDialog::~SettingsDialog() {
	delete ui;
}
