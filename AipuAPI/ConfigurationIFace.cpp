#include "ConfigurationIFace.h"

ConfigurationIFace::ConfigurationIFace()
{
	ObserverError();
}

ConfigurationIFace::~ConfigurationIFace()
{
}

void ConfigurationIFace::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});
}

void ConfigurationIFace::SetValueJSONToConfiguration() {
	const Json::Value params = jsonBody[PARAMS];
	for (Json::Value::const_iterator it = params.begin();
		it != params.end(); ++it) {

		if (it.key().asString() == MAXFACES) {
			maxDetect = it->asInt();
		}
		else if (it.key().asString() == MINEYE) {
			minEyeDistance = it->asInt();
		}
		else if (it.key().asString() == MAXEYE) {
			maxEyeDistance = it->asInt();
		}
		else if (it.key().asString() == PRECISION) {
			accuracy = it->asInt();
		}
		else if (it.key().asString() == MODEDETECT) {
			modeDetect = it->asInt();
		}

	}

}

void ConfigurationIFace::ParseJSONToObject() {
	jsonBody.clear();
	stringJSON = manageFile->ReadFileText();
	bool parsingSuccessful = reader.parse(stringJSON, jsonBody);
	if (parsingSuccessful)
	{
		SetValueJSONToConfiguration();
	}
	else {
		error->CheckError(ERROR_PARSING, error->medium);
	}

}

void ConfigurationIFace::ParseMapToJSON() {
	jsonBody.clear();
	jsonParams.clear();
	std::map<std::string, std::int16_t> params;

	params.insert(std::pair<std::string, std::int16_t>(MAXFACES, maxDetect));
	params.insert(std::pair<std::string, std::int16_t>(MINEYE, minEyeDistance));
	params.insert(std::pair<std::string, std::int16_t>(MAXEYE, maxEyeDistance));
	params.insert(std::pair<std::string, std::int16_t>(PRECISION, accuracy));
	params.insert(std::pair<std::string, std::int16_t>(MODEDETECT, modeDetect));
	std::map<std::string, std::int16_t>::const_iterator it = params.begin(),
		end = params.end();

	for (; it != end; ++it) {
		jsonParams[it->first] = it->second;

	}

	jsonBody[CONFIGURATION] = DETECT_CONFIGURATION;
	jsonBody[PARAMS] = jsonParams;

	SaveConfiguration();

}

