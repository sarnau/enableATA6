---
title: "enableATA6"
date: "2001-01-01"
categories:
   - "software"
   - "macintosh"
---

This little kernel extension allows using hard drives with more than 128MB on a Macintosh Cube. It does so by setting the flags for ATA6 inside the driver.

Download the source code: [enableATA6.zip](enableATA6.zip)

enableATA6.h
------------
``` c
#include <IOKit/IOService.h>
#include "IOATABlockStorageDriver.h"

class com_sarnau_enableATA6 : public IOATABlockStorageDriver
{
	OSDeclareDefaultStructors ( com_sarnau_enableATA6 )

protected:
	virtual IOReturn 	identifyAndConfigureATADevice ( void );
};
```

enableATA6.cpp
--------------

``` c
#include "enableATA6.h"

#define	super IOATABlockStorageDriver
OSDefineMetaClassAndStructors ( com_sarnau_enableATA6, IOATABlockStorageDriver );

/***
*  This kext overloads the standard IOATABlockStorageDriver (by having a IOProbeScore of 1000)
*
*  We only have to overload the identifyAndConfigureATADevice function and patch a variable
*  to enable LBA support. The problem is the IOATABusInfo class - this class checks the bus
*  interface hardware and detects that the computer is not capable of supporting LBA drives.
*  And so LBA support is disabled in general. There is a good reason to do this, because without
*  this support in the ROM the system can't read the kernel from the drive, if the kernel is
*  beyond the 128MB border. For this reason we should partition a drive and make sure that the
*  boot partition is 127.99 GB or smaller. If we don't boot from this drive, we don't have to
*  partition it.
***/
IOReturn 	com_sarnau_enableATA6::identifyAndConfigureATADevice(void)
{
	// call super class (and execute IDENTIFY DEVICE commamd - see ATA6 spec for more information)
	IOReturn	err = super::identifyAndConfigureATADevice();

	// does the drive support the 48 bit command set?
	if(fDeviceIdentifyData[kATAIdentifyCommandSetSupported2] & kATASupports48BitAddressingMask)
	{
		// and is the 48 bit command set really enabled for that drive?
		if(fDeviceIdentifyData[kATAIdentifyCommandsEnabled] & kATASupports48BitAddressingMask)
		{
			// then force the use of the LBA commands in the driver
			fUseExtendedLBA = true;

			// set the "48-bit LBA supported" bit in the "ATA Features" entry in the registry
			// (this seems not to be necessary, but maybe 3rd party software checks the registry)
			fSupportedFeatures |= kIOATAFeature48BitLBA;

			// we don't increase the buffer size, which is normally done by the super class
			// when a LBA drive is recognized. So we are limited to 256 instead of 2048 blocks
			// per read or write.
		}
	}

	return err;
}
```
