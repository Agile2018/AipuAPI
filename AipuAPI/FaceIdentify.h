#ifndef FACEIDENTIFY_H
#define FACEIDENTIFY_H

#include "ConfigurationIdentify.h"
#include "ErrorIdKitLib.h"
#include "Molded.h"
#include "User.h"
#include "AFaceAPI.h"
#include "Format.h"

class FaceIndentify
{
public:
	FaceIndentify();
	~FaceIndentify();
	void LoadConnection();
	void EnrollUser(Molded* modelImage);
	void SetIsRegister(bool option) {
		isRegister = option;
	}

	bool GetFlagEnroll() {
		return flagEnroll;
	}

	void ResetCountRepeatUser() {
		countRepeatUser = 0;
	}

	int GetCountRepeatUser() {
		return countRepeatUser;
	}

	void SetParamsIdentify();
	ConfigurationIdentify* configuration = new ConfigurationIdentify();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	Rx::subject<User*> userSubject;
	Rx::observable<User*> observableUser = userSubject.get_observable();
private:
	ErrorIdKitLib* error = new ErrorIdKitLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<User*> shootUser = userSubject.get_subscriber();
	bool flagEnroll = false;
	Format* format = new Format();
	bool isRegister = true;
	void ObserverError();
	int countRepeatUser = 0;
};


#endif // !FACEIDENTIFY_H

