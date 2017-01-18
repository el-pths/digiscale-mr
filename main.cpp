//main.cpp

#include <windows.h>
#include <stdio.h>
#include "header.h"

HWND hDlg;
HANDLE hThread;
HANDLE hCom;
FILE *savefile;

DWORD WINAPI ThreadProc(LPVOID lpp){
	char buf[16];
	DWORD num;
	char str[256];
	char *c = str;
	while(true){
		ReadFile(hCom, (char *) &buf, sizeof(buf), &num, NULL);
		for(int i = 0; i<num; i++){
			if(buf[i] == '\n'){
				*c = '\0';
				c = str;
				SetDlgItemText(hDlg, ID_MESSAGETEXT, (char *) &str);
			} else *(c++) = buf[i];
			if(savefile) fprintf(savefile, "%c", buf[i]);
		}
	}
	return 0;
}

BOOL onInitDialog(HWND hw){
	hDlg = hw;
	EnableWindow(GetDlgItem(hw, ID_CLOSEBUTTON), FALSE);
	EnableWindow(GetDlgItem(hw, ID_SENDBUTTON), FALSE);
	SetDlgItemText(hw, ID_EDITCOM, (char *) "COM9");
	return TRUE;
}

void setComSettings(){
	DCB state;
	GetCommState(hCom, &state);
	state.BaudRate = 115200;
	SetCommState(hCom, &state);
	COMMTIMEOUTS t;
	t.ReadIntervalTimeout = 3;
	t.ReadTotalTimeoutConstant = 2;
	t.ReadTotalTimeoutMultiplier = 3;
	t.WriteTotalTimeoutConstant = 2;
	t.WriteTotalTimeoutMultiplier = 3;
	SetCommTimeouts(hCom, &t);
}

void openPort(HWND hw){
	char nCom[256];
	GetDlgItemText(hw, ID_EDITCOM, (char *) &nCom, sizeof(nCom));
	hCom = CreateFile((char *) &nCom, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	setComSettings();
	char filename[256];
	if(GetDlgItemText(hw, ID_EDITFILE, (char *) &filename, sizeof(filename))){
		savefile = fopen((char *) &filename, "w");
	}
	ResumeThread(hThread);
	EnableWindow(GetDlgItem(hw, ID_OPENBUTTON), FALSE);
	EnableWindow(GetDlgItem(hw, ID_CLOSEBUTTON), TRUE);
	EnableWindow(GetDlgItem(hw, ID_SENDBUTTON), TRUE);
}

void closePort(HWND hw){
	CloseHandle(hCom);
	if(savefile) fclose(savefile);
	SuspendThread(hThread);
	EnableWindow(GetDlgItem(hw, ID_OPENBUTTON), TRUE);
	EnableWindow(GetDlgItem(hw, ID_CLOSEBUTTON), FALSE);
	EnableWindow(GetDlgItem(hw, ID_SENDBUTTON), FALSE);
	SetDlgItemText(hw, ID_EDITFILE, NULL);
	SetDlgItemText(hw, ID_MESSAGETEXT, NULL);
}

void sendMessage(){
	DWORD n;
	char message[] = "hello";
	WriteFile(hCom, (char *) &message, sizeof(message), &n, NULL);
}

BOOL CALLBACK DlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_INITDIALOG:
		return onInitDialog(hw);
	case WM_COMMAND:
		switch(wp){
		case ID_OPENBUTTON:
			openPort(hw);
			break;
		case ID_CLOSEBUTTON:
			closePort(hw);
			break;
		case ID_SENDBUTTON:
			sendMessage();
			break;
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hw, 0);
		return TRUE;
	default:
		return FALSE;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	hThread = CreateThread(NULL, 0, &ThreadProc, NULL, CREATE_SUSPENDED, NULL);
	return DialogBox(NULL, MAKEINTRESOURCE(ID_DIALOG), NULL, DlgProc);
}
