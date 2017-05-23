#include <IOKit/IOService.h>
#include "IOATABlockStorageDriver.h"

class com_sarnau_enableATA6 : public IOATABlockStorageDriver
{
	OSDeclareDefaultStructors ( com_sarnau_enableATA6 )

protected:
	virtual IOReturn 	identifyAndConfigureATADevice ( void );
};
