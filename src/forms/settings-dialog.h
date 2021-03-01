#pragma once

#include <QtWidgets/QDialog>

#include "ui_settings-dialog.h"

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget* parent = 0);
	~SettingsDialog();
	void showEvent(QShowEvent* event);
	void ToggleShowHide();
	void RefreshStatus();
	void SetServerStatusIndicator(bool active=false);
	void ForceStopStream();

private Q_SLOTS:
	void SaveSettings();
	void DialogButtonClicked(QAbstractButton *button);

private:
	Ui::SettingsDialog* ui;
};
