#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafes.h>

#define BUFFERSIZE 512

int _tmain(VOID) {
	bool fConnected = false;
	DWORD dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeteste");

	while (1)
		_tprintf(TEXT("\nServidor Ligado pipe=%s\n"), lpszPipename);

	hPipe = CreateNamedPipe(lpszPipename, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE, PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFFERSIZE, BUFFERSIZE, 0, NULL);
}
