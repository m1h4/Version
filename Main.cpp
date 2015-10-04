#include <Windows.h>
#include <Tchar.h>

#pragma comment(lib,"version.lib")

VOID Print(LPCTSTR fmt,...)
{	
	TCHAR buffer[0x8000];
    
    va_list args;
    va_start(args,fmt);
	_vsntprintf(buffer,sizeof(buffer)/sizeof(TCHAR),fmt,args);
    va_end(args);

	DWORD written;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),buffer,(DWORD)_tcslen(buffer),&written,NULL);
}

VOID PrintUsage(VOID)
{
	Print(TEXT("Displays program version information.\n\nVERSION [path]\n\n  path        Path to the file whose version information is to be displayer.\n\n"));
}

VOID PrintError(VOID)
{
	TCHAR buffer[512];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR)buffer,sizeof(buffer),NULL);
	Print(buffer);
}

INT _tmain(UINT argc,LPCTSTR* argv)
{
	argc--;
	argv++;

	if(!argc)
	{
		PrintUsage();
		return 0;
	}

	DWORD size = GetFileVersionInfoSize(argv[0],NULL);
	if(!size)
	{
		PrintError();
		return 0;
	}

	LPBYTE versionInfo = new BYTE[size];
	if(!versionInfo)
	{
		Print(TEXT("Out of memory.\n"));
		return 0;
	}

	if(!GetFileVersionInfo(argv[0],NULL,size,versionInfo))
	{
		delete[] versionInfo;
		PrintError();
		return 0;
	}

	UINT len = NULL;
	LPVOID ffi = NULL;
	VS_FIXEDFILEINFO fixedFileInfo;
	
	if(VerQueryValue(versionInfo,TEXT("\\"),(LPVOID*)&ffi,&len))
	{
		CopyMemory(&fixedFileInfo,ffi,sizeof(VS_FIXEDFILEINFO));

		if(fixedFileInfo.dwSignature != VS_FFI_SIGNATURE)
		{
			delete[] versionInfo;
			Print(TEXT("File version information signature mismatch.\n"));
			return 0;
		}
	}
	else
	{
		Print(TEXT("Failed to read version information.\n"));
		return 0;
	}

	Print(TEXT("File version: %d.%d.%d.%d\nProduct version: %d.%d.%d.%d\n"),HIWORD(fixedFileInfo.dwFileVersionMS),LOWORD(fixedFileInfo.dwFileVersionMS),HIWORD(fixedFileInfo.dwFileVersionLS),LOWORD(fixedFileInfo.dwFileVersionLS),HIWORD(fixedFileInfo.dwProductVersionMS),LOWORD(fixedFileInfo.dwProductVersionMS),HIWORD(fixedFileInfo.dwProductVersionLS),LOWORD(fixedFileInfo.dwProductVersionLS));

	delete[] versionInfo;

	return 0;
}