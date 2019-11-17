#ifndef ErrorAipuLib_h
#define ErrorAipuLib_h

#include <iostream>  
#include <string>
#include "rx.hpp"
#include "Either.h"
#include "CodesError.h"

namespace Rx {
	using namespace rxcpp;
	using namespace rxcpp::subjects;
	using namespace rxcpp::operators;
	using namespace rxcpp::util;

}
using namespace std;

class ErrorAipuLib
{
public:
	ErrorAipuLib();
	~ErrorAipuLib();

	enum ErrorWeight
	{
		gross = GROSS_ERROR,
		medium = MEDIUM_ERROR,
		less = LESS_ERROR,
		none = PROCESS_OK
	};
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	void CheckError(int errorCode, ErrorWeight errorWeight,
		string message);
private:	
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();

};


#endif // !ErrorAipuLib_h

