#include "ErrorFaceLib.h"

ErrorFaceLib::ErrorFaceLib()
{
}

ErrorFaceLib::~ErrorFaceLib()
{
}

void ErrorFaceLib::BuildMessageErrorLicense(int errorCode, ErrorWeightFace errorWeight) {
	int hwIdLen = BUFFER_LENGTH;
	char hwId[BUFFER_LENGTH];
	Either* either = new Either();
	IFACE_GetHardwareId(hwId, &hwIdLen);
	char msg[CHAR_LENGTH];
	sprintf_s(msg, "Your license is invalid or not present, \nplease contact support for license with this HwId %s\n", hwId);
	string msgError = msg;
	either->SetCode(errorWeight);
	either->SetLabel(msgError);
	shootError.on_next(either);

}

void ErrorFaceLib::BuildMessageOtherError(int errorCode, ErrorWeightFace errorWeight) {
	const int errorBufferLength = BUFFER_LENGTH;
	char errorBuffer[errorBufferLength];
	char msg[CHAR_LENGTH];
	string msgError;
	Either* either = new Either();

	int getErrorMsgErrorCode = IFACE_GetErrorMessage(errorCode,
		errorBufferLength, errorBuffer);
	if (getErrorMsgErrorCode == IFACE_OK)
	{
		sprintf_s(msg, "Error occurs! %s (code: %d)\n", errorBuffer, errorCode);
		msgError = msg;
	}
	else
	{
		sprintf_s(msg, "Error occurs! %s (code %d)\n", "Error occurred during error code to message translation", getErrorMsgErrorCode);
		msgError = msg;
	}
	either->SetCode(errorWeight);
	either->SetLabel(msgError);
	shootError.on_next(either);

}
void ErrorFaceLib::BuildMessageOk() {
	Either* either = new Either();
	either->SetCode(ErrorWeightFace::none);
	either->SetLabel("OK");
	shootError.on_next(either);

}

void ErrorFaceLib::CheckError(int errorCode, ErrorWeightFace errorWeight) {

	switch (errorCode)
	{
	case IFACE_OK:
		BuildMessageOk();
		break;
	case IFACE_ERR_LICENSE_INTEGRATION_GENERIC:
		BuildMessageErrorLicense(errorCode, errorWeight);
		break;
	default:
		BuildMessageOtherError(errorCode, errorWeight);
		break;
	}


}