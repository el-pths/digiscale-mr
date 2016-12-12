//main.cpp

#include <windows.h>
#include <stdio.h>
#include "header.h"

HWND hDlg;
HANDLE hThread;
HANDLE hCom;
FILE *savefile;

DWORD WINAPI ThreadProc(LPVOID lpp){
	char buf[4];
	DWORD num;
	char str[256];
	UINT val;
	while(true){
		ReadFile(hCom, (char *) &buf, sizeof(buf), &num, NULL);
		if(num != 0 && num != 4){
			ReadFile(hCom, (char *) &buf[num], sizeof(buf) - num, &num, NULL);
			num = 4;
		}
		if(num == 4){
			val = 0x00;
			val |= buf[0]&0xff;
			val |= (buf[1]&0xff)<<8;
			val |= (buf[2]&0xff)<<16;
			val |= (buf[3]&0xff)<<24;
			sprintf((char *) str, "%u", val);
			SetDlgItemText(hDlg, ID_MESSAGETEXT, (char *) &str);
			if(savefile) fprintf(savefile, "%u\n", val);
		}
	}
	return 0;
}

BOOL CALLBACK DlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_INITDIALOG:
		hDlg = hw;
		EnableWindow(GetDlgItem(hw, ID_CLOSEBUTTON), FALSE);
		EnableWindow(GetDlgItem(hw, ID_SENDBUTTON), FALSE);
		SetDlgItemText(hw, ID_EDITCOM, (char *) "COM9");
		return TRUE;
	case WM_COMMAND:
		switch(wp){
		case ID_OPENBUTTON:
			char nCom[256];
			GetDlgItemText(hw, ID_EDITCOM, (char *) &nCom, sizeof(nCom));
			hCom = CreateFile((char *) &nCom, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			DCB state;
			GetCommState(hCom, &state);
			state.BaudRate = 9600;
			SetCommState(hCom, &state);
			COMMTIMEOUTS t;
			t.ReadIntervalTimeout = 3;
			t.ReadTotalTimeoutConstant = 2;
			t.ReadTotalTimeoutMultiplier = 3;
			t.WriteTotalTimeoutConstant = 2;
			t.WriteTotalTimeoutMultiplier = 3;
			SetCommTimeouts(hCom, &t);
			char filename[256];
			if(GetDlgItemText(hw, ID_EDITFILE, (char *) &filename, sizeof(filename))){
				savefile = fopen((char *) &filename, "w");
			}
			ResumeThread(hThread);
			EnableWindow(GetDlgItem(hw, ID_OPENBUTTON), FALSE);
			EnableWindow(GetDlgItem(hw, ID_CLOSEBUTTON), TRUE);
			EnableWindow(GetDlgItem(hw, ID_SENDBUTTON), TRUE);
			break;
		case ID_CLOSEBUTTON:
			CloseHandle(hCom);
			if(savefile) fclose(savefile);
			SuspendThread(hThread);
			EnableWindow(GetDlgItem(hw, ID_OPENBUTTON), TRUE);
			EnableWindow(GetDlgItem(hw, ID_CLOSEBUTTON), FALSE);
			EnableWindow(GetDlgItem(hw, ID_SENDBUTTON), FALSE);
			SetDlgItemText(hw, ID_EDITFILE, NULL);
			SetDlgItemText(hw, ID_MESSAGETEXT, NULL);
			break;
		case ID_SENDBUTTON:
			DWORD n;
			char message[] = "hello";
			WriteFile(hCom, (char *) &message, sizeof(message), &n, NULL);
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
