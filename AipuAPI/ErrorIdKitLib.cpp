#include "ErrorIdKitLib.h"

ErrorIdKitLib::ErrorIdKitLib()
{
}

ErrorIdKitLib::~ErrorIdKitLib()
{
}

void ErrorIdKitLib::CheckError(int errorCode, ErrorWeight errorWeight) {
	string messageError;
	char msg[CHAR_LENGTH_MSG];
	Either* either = new Either();

	if (errorCode != IENGINE_E_NOERROR)
	{
		sprintf_s(msg, "Error occurs! %s (code: %d)\n", IEngine_GetErrorMsg(errorCode), errorCode);
		messageError = msg;
		either->SetCode(errorWeight);
		either->SetLabel(messageError);
		shootError.on_next(either);

	}

}