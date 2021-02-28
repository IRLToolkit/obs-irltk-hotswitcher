#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMutex>

#include "plugin-main.h"

class HttpManager {
	public:
		HttpManager();
		~HttpManager();

		void SetApiKey(QString apiKey);
		HttpReply *_HttpGet(QString url);

		void GetIrltkServerStatus(std::function<void(obs_data_t*)> cb);

	private:
		Http *httpPtr;
		QMutex requestMutex;
};