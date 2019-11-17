#ifndef ErrorIdKitLib_h
#define ErrorIdKitLib_h

#include "rx.hpp"
#include "CodesError.h"
#include "Either.h"
#include "idkit.h"

#define CHAR_LENGTH_MSG		256

namespace Rx {
	using namespace rxcpp;
	using namespace rxcpp::subjects;
	using namespace rxcpp::operators;
	using namespace rxcpp::util;

}

class ErrorIdKitLib
{
public:
	ErrorIdKitLib();
	~ErrorIdKitLib();
	enum ErrorWeight
	{
		gross = GROSS_ERROR,
		medium = MEDIUM_ERROR,
		less = LESS_ERROR,
		none = PROCESS_OK
	};
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	void CheckError(int errorCode, ErrorWeight errorWeight);
private:

	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
};


#endif // !ErrorIdKitLib_h

