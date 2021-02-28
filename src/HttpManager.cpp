#include "HttpManager.h"

using json = nlohmann::json;

HttpManager::HttpManager()
{
	httpPtr = new Http;
	QString userAgent = QString("irltk-hotswitcher/%1 (obs-studio)").arg(PLUGIN_VERSION);
	httpPtr->addRequestHeader("User-Agent", userAgent.toUtf8());
}

HttpManager::~HttpManager()
{
	;
}

void HttpManager::SetApiKey(QString apiKey)
{
	httpPtr->addRequestHeader("X-API-KEY", apiKey.toUtf8());
#ifdef DEBUG_MODE
	blog(LOG_INFO, "[HttpManager] Set Authorization header to %s", QT_TO_UTF8(apiKey));
#endif
}

HttpReply *HttpManager::_HttpGet(QString url)
{
	requestMutex.lock();
	auto reply = httpPtr->get(url);
	requestMutex.unlock();
#ifdef DEBUG_MODE
	QObject::connect(reply, &HttpReply::finished, [=] () {
		blog(LOG_INFO, "[HttpManager] Received response for url: %s | Code: %d | Body: %s", QT_TO_UTF8(url), reply->statusCode(), reply->body().constData());
	});
#endif
	return reply;
}

void HttpManager::GetIrltkServerStatus(std::function<void(obs_data_t*)> cb)
{
	auto httpReply = _HttpGet("https://api.irl.run/server/@me/status");
	QObject::connect(httpReply, &HttpReply::finished, [=] () {
		obs_data_t *reply = obs_data_create();
		if (httpReply->isSuccessful()) {
			json httpReplyJson = json::parse(httpReply->body().toStdString());
			obs_data_set_int(reply, "statusCode", httpReplyJson["httpStatus"].get<int>());
			if (httpReplyJson["httpStatus"].get<int>() == 200) {
				obs_data_set_bool(reply, "status", true);
				obs_data_set_bool(reply, "serverStatus", httpReplyJson["online"].get<bool>());
			} else {
				obs_data_set_bool(reply, "status", false);
				obs_data_set_string(reply, "comment", httpReplyJson["message"].get<std::string>().c_str());
			}
		} else {
			obs_data_set_int(reply, "statusCode", httpReply->statusCode());
			obs_data_set_bool(reply, "status", false);
			obs_data_set_string(reply, "comment", QT_TO_UTF8(httpReply->reasonPhrase()));
		}
		cb(reply);
		obs_data_release(reply);
	});
}