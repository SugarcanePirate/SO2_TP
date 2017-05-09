#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>

#define BUFSIZE 512

int _tmain(int argc, TCHAR *argv[]){
	HANDLE hPipe;
	LPTSTR lpvMessage;
	TCHAR chBuf[BUFSIZE];
	BOOL fSuccess = FALSE;
	DWORD cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	if (argc > 1){
		lpvMessage = argv[1];
	}else {
		lpvMessage = TEXT("Mensagem de recurso");
	}

	while (true){
		hPipe = CreateFile(lpszPipename, GENERIC_READ | GENERIC_WRITE | PIPE_WAIT, 0, NULL, OPEN_EXISTING, 0, NULL);
	
		if (hPipe != INVALID_HANDLE_VALUE){
			break;
		}
		
		if (GetLastError() != ERROR_PIPE_BUSY){
			_tprintf(TEXT("Não foi possivel abrir o pipe. Erro=%d\n"), GetLastError());
			return -1;
		}

		if (!WaitNamedPipe(lpszPipename, 30000)){
			_tprintf(TEXT("Esperei por instancia 30 segundos. Sair"));
			return -1;
		}
	}

	dwMode = PIPE_READMODE_MESSAGE;

	fSuccess = SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);

	if (!fSuccess){
		_tprintf(TEXT("SetNamedPipeHandleState falhou. Erro=%d\n"), GetLastError());
		return -1;
	}

	cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
	_tprintf(TEXT("A enviar %d bytes: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(hPipe, lpvMessage, cbToWrite, &cbWritten, NULL);

	if (!fSuccess){
		_tprintf(TEXT("WriteFile falhou. Erro=%d\n"), GetLastError());
		return -1;
	}
	_tprintf(TEXT("\nMensagem enviada. Aguardar resposta\n"));

	while (1) {
		fSuccess = ReadFile(hPipe, chBuf, BUFSIZE * sizeof(TCHAR), &cbRead, NULL);
		chBuf[cbRead / sizeof(TCHAR)] = '\0';
		if (!fSuccess || !cbRead)
			break;
		_tprintf(TEXT("[LEITOR] Recebi %d bytes: '%s'... (ReadFile)\n"), cbRead, chBuf);
		Sleep(10000);
		if (!fSuccess && GetLastError() != ERROR_MORE_DATA) {
			break;
		}
	}

	if (!fSuccess){
		_tprintf(TEXT("ReadFile deu erro. Erro=%d\n"), GetLastError());
		return -1;
	}

	CloseHandle(hPipe);

	return 0;
}