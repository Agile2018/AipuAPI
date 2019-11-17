#ifndef ConfigurationFile_h
#define ConfigurationFile_h

#include "Configuration.h"
#include "ErrorAipuLib.h"

class ConfigurationFile : public Configuration
{
public:
	ConfigurationFile();
	~ConfigurationFile();
	void ParseJSONToObject();
	void ParseMapToJSON();

	void SetNameFileConfigurationVideo(string name) {
		nameFileConfigurationVideo = name;
	}

	string GetNameFileConfigurationVideo() {
		return nameFileConfigurationVideo;
	}

	void SetNameFileConfigurationFaceModel(string name) {
		nameFileConfigurationFaceModel = name;
	}

	string GetNameFileConfigurationFaceModel() {
		return nameFileConfigurationFaceModel;
	}

	void SetNameDirectoryTempWork(string name) {
		nameDirectoryTempWork = name;
	}

	string GetNameDirectoryTempWork() {
		return nameDirectoryTempWork;
	}

	void SetNameDirectoryTraining(string name) {
		nameDirectoryTraining = name;
	}

	string GetNameDirectoryTraining() {
		return nameDirectoryTraining;
	}

	void SetNameFileConfigurationDatabase(string name) {
		nameFileConfigurationDatabase = name;
	}

	string GetNameFileConfigurationDatabase() {
		return nameFileConfigurationDatabase;
	}

	void SetNameFileConfigurationIdentify(string name) {
		nameFileConfigurationIdentify = name;
	}

	string GetNameFileConfigurationIdentify() {
		return nameFileConfigurationIdentify;
	}

	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();

private:
	const string LABEL_ERROR_PARSING = "Error parsing the JSON string";
	ErrorAipuLib* error = new ErrorAipuLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	void ObserverError();
	void SetValueJSONToConfiguration();
	string nameFileConfigurationVideo;
	string nameFileConfigurationFaceModel;
	string nameFileConfigurationIdentify;
	string nameDirectoryTempWork;
	string nameDirectoryTraining;
	string nameFileConfiguration;
	string nameDirectoryConfiguration;
	string nameFileConfigurationDatabase;
	const string PARAMS = "Params";
	const string FILEVIDEO = "file_video";
	const string FILEFACE = "file_face";
	const string FILEDATABASE = "file_database";
	const string DIRECTORYWORK = "directory_work";
	const string DIRECTORYTRAIN = "directory_train";
	const string CONFIGURATION = "configuration";
	const string FILE_CONFIGURATION = "files_configuration";
	const string FILEIDENTIFY = "file_identify";
};


#endif // !ConfigurationFile_h

