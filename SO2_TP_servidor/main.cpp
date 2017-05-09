#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>

#define BUFFERSIZE 512


DWORD WINAPI ThreadCliente(LPVOID lpvPARAM);

int _tmain(VOID) {
	bool fConnected = false;
	DWORD dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");

//#ifdef UNICODE
//	_setmode(_fileno(stdin), _O_WTEXT);
//	_setmode(_fileno(stdout), _O_WTEXT);
//#endif

	while (1) {
		_tprintf(TEXT("\nServidor Ligado pipe=%s\n"), lpszPipename);

		hPipe = CreateNamedPipe(lpszPipename, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFFERSIZE, BUFFERSIZE, 0, NULL);

		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("\nCriação do pipe=%s falhou erro=%d\n"), lpszPipename, GetLastError());
			return -1;

			//aguardar por clientes
		}
			fConnected = ConnectNamedPipe(hPipe, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); //sucesso
		

		if (fConnected) {
			printf("Cliente ligado.Thread a ser criada\n");

			//criar threads para cada cliente

			hThread = CreateThread(NULL, 0, ThreadCliente, (LPVOID)hPipe, 0, &dwThreadId);
			if (hThread == NULL) {
				_tprintf(TEXT("Erro na criação da thread. Error = %d"), GetLastError());
				return -1;
			}//criou bem a thread, ja nao precisa handle
			else
				CloseHandle(hThread);
		}//nao ligou a pipe fecha handle de pipe
		else
			CloseHandle(hPipe);
	}
	return 0;
}//encerra servidor


DWORD WINAPI ThreadCliente(LPVOID lpvPARAM) {
	HANDLE hHeap = GetProcessHeap(); // alloca espaço

	//alloca espaço para pedido
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFFERSIZE*sizeof(TCHAR));

	//aloca espaço para resposta
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFFERSIZE*sizeof(TCHAR));

	DWORD cbBytesRead = 0;
	DWORD cbReplyBytes = 0;
	DWORD cbWritten = 0;

	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;


	//se parametro nulo
	if (lpvPARAM == NULL) {
		printf("\nERRO - o handle no parametro é nulo\n");
		if (pchReply != NULL)
			HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL)
			HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;//termina thread
	}

	//sem memoria para pedido
	if (pchRequest == NULL) {
		printf("\nERRO - não houve memoria para o pedido\n");
		if (pchReply != NULL)
			HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;//termina thread
	}

	//sem memoria para respsota
	if (pchRequest == NULL) {
		printf("\nERRO - não houve memoria para resposta\n");
		if (pchReply != NULL)
			HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;//termina thread
	}

	printf("Thread do servidor - a receber mensagens\n");

	hPipe = (HANDLE)lpvPARAM;//envia o handle

	//dialogo com cliente
	while (1) {

		////obtem mensagem
		//fSuccess = ReadFile(hPipe, pchRequest, BUFFERSIZE*sizeof(TCHAR), &cbBytesRead, NULL);
	
		////leitura corre mal
		//if (!fSuccess || cbBytesRead == 0) {
		//	if (GetLastError() == ERROR_BROKEN_PIPE)
		//		_tprintf(TEXT("Cliente desligou-se. Erro=%d\n"), GetLastError());
		//	else
		//		_tprintf(TEXT("ReadFile falhou. Erro = %d\n"), GetLastError());
		//	break;
		//}
	
		//Escreve a resposta no pipe
		_tprintf(TEXT("[ESCRITOR] Frase: "));
		_fgetts(pchReply, 256, stdin);
		cbReplyBytes = _tcslen(pchReply)*sizeof(TCHAR);
		fSuccess = WriteFile(hPipe, pchReply, cbReplyBytes, &cbWritten, NULL);
		_tprintf(TEXT("[Servidor] Enviei %d bytes ao leitor... (WriteFile)\n"), cbReplyBytes);
		if (!fSuccess || cbReplyBytes != cbWritten) {
			_tprintf(TEXT("WriteFile falhou. Erro=%d\n"), GetLastError());
			break;
		}

	}


	//desligar ligaçoes
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	//libertar memoria
	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	printf("Thread a terminar\n");
	return 1;
}



	