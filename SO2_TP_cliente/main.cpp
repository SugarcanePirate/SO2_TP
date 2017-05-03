#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#define BUFSIZE 512

int _tmain(int argc, TCHAR *argv[]){
	HANDLE hPipe;
	LPTSTR lpvMessage;
	TCHAR chBuf[BUFSIZE];
	BOOL fSuccess = FALSE;
	DWORD cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");

	if (argc > 1){
		lpvMessage = argv[1];
	}else {
		lpvMessage = TEXT("Mensagem de recurso");
	}

	while (true){
		hPipe = CreateFile(lpszPipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	
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

	do{
		fSuccess = ReadFile(hPipe, chBuf, BUFSIZE * sizeof(TCHAR), &cbRead, NULL);
		
		if(!fSuccess && GetLastError() != ERROR_MORE_DATA) {
			break;
		}

		_tprintf(TEXT("Recebido:\"%s\"\n"), chBuf);
	} while (!fSuccess);

	if (!fSuccess){
		_tprintf(TEXT("ReadFile deu erro. Erro=%d\n"), GetLastError());
		return -1;
	}

	CloseHandle(hPipe);

	return 0;
}