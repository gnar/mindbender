#include "sound.h"

#include <stdio.h>

#include <physfs/physfs.h>
#include <fmod.h>
#include <fmod_errors.h>

#include <resource/manager.h>

#include <iostream>
using namespace std;

FMOD_RESULT F_CALLBACK fmod_open(
	const char *name, int unicode, unsigned int *filesize,
	void **handle, void **userdata)
{
	PHYSFS_file *f = Res::Manager.OpenFile(name);
	if (!f) return FMOD_ERR_FILE_NOTFOUND;
	
	*filesize = PHYSFS_fileLength(f);
	*handle = (void*)f;
	*userdata = 0;

	//cout << "fmod_open [" << name << "] " << "handle = " << *handle << " filesize = " << *filesize << endl;
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK fmod_close(void *handle, void *userdata)
{
	PHYSFS_file *f = (PHYSFS_file*)handle;
	PHYSFS_close(f);
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK fmod_seek(
	void *handle, unsigned int pos, void *userdata)
{
	PHYSFS_file *f = (PHYSFS_file*)handle;
	PHYSFS_uint64 p = pos;
	PHYSFS_seek(f, p); 
	//cout << "SEEK)  handle=" << handle << "  pos=" << pos << endl;
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK fmod_read(
	void *handle, void *buffer,
	unsigned int sizebytes, unsigned int *bytesread,
	void *userdata)
{
	PHYSFS_file *f = (PHYSFS_file*)handle;
	PHYSFS_sint64 r = PHYSFS_read(f, buffer, 1, sizebytes);
	
	//*bytesread = r;
	*bytesread = sizebytes; // FIXME: FMOD will crash with "*bytesread = r;" !!!
	
	if (r == -1) { // PHYSFS error?
		cout << "ERROR handle=" << handle << " PhysFS read error (read=-1): " << PHYSFS_getLastError() << endl;
		return FMOD_ERR_FILE_EOF;
	} else if (r < PHYSFS_sint64(sizebytes)) {
	//	cout << "handle=" << handle << " PhysFS read error: (read " << r << ", requested " << sizebytes << ")" << PHYSFS_getLastError() << endl;
		return FMOD_OK;
	} else {
		return FMOD_OK;
	}

}

static FMOD_SYSTEM *fsystem = 0;

FMOD_SYSTEM *SoundSystem()
{
	return ::fsystem;
}

void InitSound(int freq)
{
	/*if (freq == 0)    freq = 32000; // default value
	if (freq <  4000) freq = 4000;
	if (freq > 65535) freq = 65535;
	
	int channels = 0;
	if (channels <= 0) channels = 32; // 32 = default value, also clip between 1 
	if (channels > 64) channels = 64; // and 64*/

	::fsystem = 0;

	FMOD_RESULT result = FMOD_System_Create(&::fsystem);
	if (result != FMOD_OK)
	{
		printf("FMOD error: (%d) %s\n", result, FMOD_ErrorString(result));
		::fsystem = 0;
		return;
	}
	
	result = FMOD_System_Init(::fsystem, 2*16, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		printf("FMOD error: (%d) %s\n", result, FMOD_ErrorString(result));
		::fsystem = 0;
		return;
	}
	
	// Glue FMOD to PhysFS
	FMOD_System_SetFileSystem(::fsystem, &fmod_open, &fmod_close, &fmod_read, &fmod_seek, 2048);
}

void DoneSound()
{
	if (::fsystem != 0)
	{
 		FMOD_System_Release(::fsystem);
		::fsystem = 0;
	}
}


