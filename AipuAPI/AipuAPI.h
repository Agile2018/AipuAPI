#pragma once
#ifdef AIPUAPI_EXPORTS
#define AIPUAPI_API __declspec(dllexport)
#else
#define AIPUAPI_API __declspec(dllimport)
#endif

#include <string>

using namespace std;

class AIPUAPI_API AipuAPI
{
public:
	AipuAPI();
	~AipuAPI();
	void InitLibrary();

	void LoadConfiguration(string nameFile);

	void SetFileVideo(string file);

	void SetWidthFrame(int value);

	void SetHeightFrame(int value);

	void CaptureFlow(int optionFlow);

	void SetIpCamera(string ip);
	void SetDeviceVideo(string device);
	void SetFaceConfidenceThresh(int value);

	void SetRefreshInterval(int value);

	void SetMinEyeDistance(int minDistance);
	void SetMaxEyeDistance(int maxDistance);
	void SetSequenceFps(int fps);
	void SetClient(int value);
	void SetFlagFlow(bool flag);
	void Terminate();
	void ReloadRecognitionFace();
	void SetConfigurationDatabase();
	void ShowWindow(int option);
	void SetIsRegister(bool option);
	void SetNameWindow(string name);
	void RecognitionFaceFiles(string file, int client);
	void SetIsFinishLoadFiles(bool value);
	bool GetIsFinishLoadFiles();
	void ResetLowScore();
	int GetCountLowScore();
	void ResetCountNotDetect();
	int GetCountNotDetect();
	void SetDeepTrack(string value);
	void ResetCountRepeatUser();
	int GetCountRepeatUser();
	string GetUserJSON();
	string GetMessageError();

private:
	void ObserverEvent();
};
