#ifndef BACKREST_H
#define BACKREST_H


#include <iostream>
#include "ErrorAipuLib.h"
using namespace std;

class Backrest
{
public:
	Backrest();
	~Backrest();
	void ProcessImageInBack(char* data, int size, int client);
	void LoadConfiguration(string nameFile);
	void SetIsRegister(bool option);
	void ReloadRecognitionFace();
	void SetConfigurationDatabase();
	void RecognitionFaceFiles(string file, int client);
	void SetIsFinishLoadFiles(bool value);
	bool GetIsFinishLoadFiles();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();

	Rx::subject<string> userDetected;
	Rx::observable<string> observableUserJSON = userDetected.get_observable();

private:
	const string DIRECTORY_CONFIGURATION = "configuration";
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<string> shootUserJSON = userDetected.get_subscriber();
	//ToolsImage* toolsImage = new ToolsImage();
	bool isFree = true;
	void ObserverTemplateImage();
	void ObserverIdentifyFace();
	void ObserverDatabase();
	void ObserverError();
	void SetDirectoryConfiguration();
	void SetNameFileConfigurationFace(string name);
	void SetNameFileConfigurationIdentify(string name);
	void SetNameFileConfigurationDatabase(string name);
	
};


#endif // !BACKREST_H

