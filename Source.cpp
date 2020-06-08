#include <Windows.h>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <math.h>
#include <string>
#include <tchar.h>

#define PATH L"D:\\"
#define DEST_PATH  L"D:\\log.txt"
#define MASK_FOUND L".txt"
#define SIZE_BUF 260

HWND hWnd, hTextCreate, hButtonCreate, hTextDelete, hButtonDelete, 
			hTextCopyStart, hTextCopyFinish, hButtonCopy,
			hTextMoveStart, hTextMoveFinish, hButtonMove,
			hButtonOpen, hButtonSave, hTextOpen, hTextSave,
			hList, hTextFind, hButtonFind, hTextAttributes, hButtonAttributes,
			hTextSize, hTextDate, hTextLastAccess, hButtonProperties, hTextProperties,
			hButtonHardLink, hTextHardLink, hTextHardLinkFile;
HINSTANCE hInstance;

OPENFILENAME ofn;

OVERLAPPED olf = { 0 };
LARGE_INTEGER li = { 0 };

struct time
{
	char year[5],
				day[3],
				mounth[3],
				hours[3],
				minutes[3],
			date[18];
	char* GetTime()
	{
		strcat_s(date, day);
		strcat_s(date, ".");
		strcat_s(date, mounth);
		strcat_s(date, ".");
		strcat_s(date, year);
		strcat_s(date, "  ");
		strcat_s(date, hours);
		strcat_s(date, ":");
		strcat_s(date, minutes);
		date[17] = '\0';

		
		return date;
	}
};


wchar_t* cut_str(wchar_t* str)
{
	int count = 0;
	for (int i = 0; i < wcslen(str); i++)
	{
		if (str[i] == L'\0')
			break;
		count++;
	}
	wchar_t* c = new wchar_t[count + 1];
	for (int i = 0; i <= count; i++)
	{
		c[i] = str[i];
	}

	return c;
}

void dw_to_char(DWORD dw, char* c, int n)
{
	char str[100];
	DWORD dwb = dw;
	int i = 0;
	while (dwb > 0)
	{
		dwb /= 10;
		i++;
	}
	int j = i;
	while (0 < dw)
	{
		str[j - 1] = '0' + dw % 10;
		dw /= 10;
		j--;
	}
	str[i] = L'\0';
	for (int i = 0; i < n; i++)
		c[i] = str[i];
}

time ConvertSysTimeToTime(SYSTEMTIME sTime, time cTime)
{
	dw_to_char(sTime.wYear, cTime.year, 4);
	dw_to_char(sTime.wDay, cTime.day, 2);
	dw_to_char(sTime.wMonth, cTime.mounth, 2);
	dw_to_char(sTime.wHour, cTime.hours, 2);
	dw_to_char(sTime.wMinute, cTime.minutes, 2);
	return cTime;
}

void WriteData(const wchar_t* path)
{
	wchar_t str[100];
	GetDlgItemText(hWnd, 10, str, 100);
	WIN32_FIND_DATA wfd = { 0 };
	wchar_t* str2;
	str2 = cut_str(str);
	SetCurrentDirectory(path);

	HANDLE search = FindFirstFile(str2, &wfd); //1 параметр - маска поиска!
	if (search == INVALID_HANDLE_VALUE)
		return;
	do
	{
		DWORD attributes;
		
		LPWSTR strTmp = (LPWSTR)calloc(SIZE_BUF + 1, sizeof(WCHAR));

		GetCurrentDirectory(SIZE_BUF, strTmp);
		wcscat_s(strTmp, SIZE_BUF, L"\\");
		wcscat_s(strTmp, SIZE_BUF, wfd.cFileName);

		LPWSTR strTmp_ = (LPWSTR)calloc(wcslen(strTmp) + 1, sizeof(WCHAR));
		wcscpy_s(strTmp_, wcslen(strTmp) + 1, strTmp);

		if (wcscmp(wfd.cFileName, L".") &&
			wcscmp(wfd.cFileName, L".."))
		{//Тут форматирование для записи в listbox
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				wcscat_s(strTmp, SIZE_BUF, L"\r\n");
								
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)strTmp);
				WriteData(strTmp_);

				SetCurrentDirectory(path); 
			}
			else
			{
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)strTmp);
			}
		}
		free(strTmp);
		free(strTmp_);
		//. и .. - не записывать
	} while (FindNextFile(search, &wfd));

	FindClose(search);
}

void copy_str(wchar_t* c)
{
	int i;
	for (i = 0; i < 100; i++)
	{
		if (c[i] == L'\0')
			break;
	}
	for (int j = i + 1; i < 100; i++)
	{
		c[i] = L'\0';
	}
}

char* cut_str(char* str)
{
	int count = 0;
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == L'\0')
			break;
		count++;
	}
	char* c = new char[count + 1];
	for (int i = 0; i <= count; i++)
	{
		c[i] = str[i];
	}

	return c;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
					{
						case 101:
						{
							HANDLE hFile;
							wchar_t PATH_Create[MAX_PATH];
							GetDlgItemText(hWnd, 1, PATH_Create, 250);
							if (wcslen(PATH_Create) == 0)
							{
								MessageBox(hWnd, L"Write path", L"ERROR", MB_OK);
								return 0;
							}
							hFile = CreateFile(PATH_Create, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
							if (hFile == INVALID_HANDLE_VALUE)
							{
								CloseHandle(hFile);
								return 1;
							}
								
							CloseHandle(hFile);
						}
						return 0;

						case 102:
						{			
							wchar_t PATH_Delete[MAX_PATH];
							GetDlgItemText(hWnd, 2, PATH_Delete, 250);
							if (wcslen(PATH_Delete) == 0)
							{
								MessageBox(hWnd, L"Write path", L"ERROR", MB_OK);
								return 0;
							}
							DeleteFile(PATH_Delete);
						}
						return 0;

						case 103:
						{
							wchar_t	PATH_Copy_Start[MAX_PATH],
								    PATH_Copy_Finish[MAX_PATH];
							GetDlgItemText(hWnd, 3, PATH_Copy_Start, 250);
							GetDlgItemText(hWnd, 4, PATH_Copy_Finish, 250);
							if (wcslen(PATH_Copy_Start) == 0 || wcslen(PATH_Copy_Finish) == 0)
							{
								MessageBox(hWnd, L"Write path", L"ERROR", MB_OK);
								return 0;
							}
							if (!CopyFile(PATH_Copy_Start, PATH_Copy_Finish, TRUE))
							{
								MessageBox(hWnd, L"File no copied", L"ERROR", MB_OK);
								return 0;
							}
						}
						return 0;

						case 104:
						{
							wchar_t	PATH_Move_Start[MAX_PATH],
									PATH_Move_Finish[MAX_PATH];
							GetDlgItemText(hWnd, 5, PATH_Move_Start, 250);
							GetDlgItemText(hWnd, 6, PATH_Move_Finish, 250);
							if (wcslen(PATH_Move_Start) == 0 || wcslen(PATH_Move_Finish) == 0)
							{
								MessageBox(hWnd, L"Write path", L"ERROR", MB_OK);
								return 0;
							}
							if (!MoveFileEx(PATH_Move_Start, PATH_Move_Finish, MOVEFILE_REPLACE_EXISTING))
							{
								MessageBox(hWnd, L"File no rename", L"ERROR", MB_OK);
								return 0;
							}
						}
						return 0;

						case 105:
						{
							DWORD dwNumRead = 0;
							HANDLE hFile;
							wchar_t PATH_Open[100];
							
							ZeroMemory(&ofn, sizeof(ofn));
							ofn.lStructSize = sizeof(ofn);
							ofn.hwndOwner = hWnd;
							ofn.lpstrFile = PATH_Open;
							ofn.nMaxFile = sizeof(PATH_Open);
							ofn.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0";
							ofn.nFilterIndex = 2;
							ofn.lpstrFileTitle = NULL;
							ofn.nMaxFileTitle = 0;
							ofn.lpstrInitialDir = NULL;
							ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
							//GetDlgItemText(hWnd, 7, PATH_Open, 100);
							
							GetOpenFileName(&ofn);
							hFile = CreateFile(PATH_Open, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							if (hFile == INVALID_HANDLE_VALUE)
							{
								CloseHandle(hFile);
								return 1;
							}
							li.QuadPart = 0;
							olf.Offset = li.LowPart;
							olf.OffsetHigh = li.HighPart;
							char buffer[100];
							if (!ReadFile(hFile, buffer, 260, &dwNumRead, &olf))
							{
								CloseHandle(hFile);
								return 1;
							}
							buffer[dwNumRead] = L'\0';
							SetDlgItemTextA(hWnd, 8, buffer);
							CloseHandle(hFile);
							
						}
						return 0;

						case 106:
						{
							DWORD dwNumRead = 0;
							HANDLE hFile;
							wchar_t PATH_Open[100];
							GetDlgItemText(hWnd, 7, PATH_Open, 100);
							hFile = CreateFile(PATH_Open, GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							if (hFile == INVALID_HANDLE_VALUE)
							{
								CloseHandle(hFile);
								return 1;
							}
							wchar_t buffer[100];
							char buf[100];
							
							GetDlgItemText(hWnd, 8, buffer, 99);
							GetSaveFileName(&ofn);
							copy_str(buffer);
							WideCharToMultiByte(CP_UTF8, NULL, buffer, sizeof(buffer), buf, sizeof(buf), NULL, NULL);
							char* buff;
							buff = cut_str(buf);
							
							if(!WriteFile(hFile, buff, strlen(buff), &dwNumRead, &olf))
							{
								return 1;
							}
							CloseHandle(hFile);
						}
						return 0;

						case 107:
						{
							WriteData(PATH);
						}
						return 0;

						case 108:
						{
							DWORD attributes;
							wchar_t str[100];
							wchar_t* str2;
							GetDlgItemText(hWnd, 11, str, 100);
							str2 = cut_str(str);
							attributes = GetFileAttributes(str2);
							
							if (attributes & FILE_ATTRIBUTE_DIRECTORY)
							{
								SetDlgItemText(hWnd, 11, L"Директория");
							
							}
							if (attributes & FILE_ATTRIBUTE_READONLY)
							{
								SetDlgItemText(hWnd, 11, L"Только чтение");
							}
							if (attributes & FILE_ATTRIBUTE_HIDDEN)
							{
								SetDlgItemText(hWnd, 11, L"Cкрытый");
							
							}
							if (attributes & FILE_ATTRIBUTE_NORMAL)
							{
								SetDlgItemText(hWnd, 11, L"Обычный");
								
							}
							if (attributes & FILE_ATTRIBUTE_SYSTEM)
							{
								SetDlgItemText(hWnd, 11, L"Системный");
							
							}
							if (attributes & FILE_ATTRIBUTE_ARCHIVE)
							{
								SetDlgItemText(hWnd, 11, L"Архив");
							
							}
							if (attributes & FILE_ATTRIBUTE_READONLY)
							{
								SetFileAttributes(str2, FILE_ATTRIBUTE_HIDDEN);
							}
							else
							{
								SetFileAttributes(str2, FILE_ATTRIBUTE_READONLY);
							}
						}
						return 0;

						case 109:
						{
							wchar_t PATH_open[100];
							GetDlgItemText(hWnd, 15, PATH_open, 100);
							HANDLE hFile = CreateFile(PATH_open, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							DWORD size_file;
							size_file = GetFileSize(hFile, NULL);
							char str[100];
							DWORD dwb = size_file;							
							int i = 0;
							while (dwb > 0)
							{
								dwb /= 10;
								i++;
							}
							int j = i;
							if(i == 0)
							{
								SetDlgItemTextA(hWnd, 12, "0");
							}
							else
							{
								while (0 < size_file)
								{
									str[j - 1] = '0' + size_file % 10;
									size_file /= 10;
									j--;
								}
								str[i] = '\0';
								SetDlgItemTextA(hWnd, 12, str);
							}
							FILETIME lpCreate = { 0 }, lpAccess = { 0 };
							GetFileTime(hFile, &lpCreate, &lpAccess, NULL);
							SYSTEMTIME lpsCreate = { 0 }, lpsAccess = {0};
							FileTimeToSystemTime(&lpCreate, &lpsCreate);
							FileTimeToSystemTime(&lpAccess, &lpsAccess);
							char* cCreate, cAccess;
							time cTimeC = { 0 }, cTimeA = {0};
							cTimeC = ConvertSysTimeToTime(lpsCreate, cTimeC);												

							cCreate = cTimeC.GetTime();
							SetDlgItemTextA(hWnd, 13, cCreate);

							cTimeA = ConvertSysTimeToTime(lpsAccess, cTimeA);

							cCreate = cTimeA.GetTime();
							SetDlgItemTextA(hWnd, 14, cCreate);

							
						}
						return 0;

						case 110:
						{
							wchar_t PATH_file[100], PATH_hard_link[100];
							GetDlgItemText(hWnd, 16, PATH_file, 100);
							GetDlgItemText(hWnd, 17, PATH_hard_link, 100);
							if (!CreateHardLink(PATH_hard_link, PATH_file, NULL))
							{
								MessageBox(hWnd, L"Не удалось создать", L"ERROR", MB_OK);
							}
							else
							{
								MessageBox(hWnd, L"Ссылка создана !!!", L"ERROR", MB_OK);
							}

						}
						return 0;

						default:
							return 0;

					}
		
			}
		return 0;
		
		case WM_CREATE:
			{
				hButtonCreate = CreateWindowEx(0, L"BUTTON", L"Создать файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 30, 120, 20, hWnd, (HMENU)(101), hInstance, nullptr);
				hTextCreate = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 30, 200, 20, hWnd, (HMENU)(1), hInstance, nullptr);
				hButtonDelete = CreateWindowEx(0, L"BUTTON", L"Удалить файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 70, 120, 20, hWnd, (HMENU)(102), hInstance, nullptr);
				hTextDelete = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 70, 200, 20, hWnd, (HMENU)(2), hInstance, nullptr);
				hButtonCopy = CreateWindowEx(0, L"BUTTON", L"Копировать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 100, 120, 50, hWnd, (HMENU)(103), hInstance, nullptr);
				hTextCopyStart = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 100, 200, 20, hWnd, (HMENU)(3), hInstance, nullptr);
				hTextCopyFinish = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 130, 200, 20, hWnd, (HMENU)(4), hInstance, nullptr);
				hButtonMove = CreateWindowEx(0, L"BUTTON", L"Переименовать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 160, 120, 50, hWnd, (HMENU)(104), hInstance, nullptr);
				hTextMoveStart = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 160, 200, 20, hWnd, (HMENU)(5), hInstance, nullptr);
				hTextMoveFinish = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 190, 200, 20, hWnd, (HMENU)(6), hInstance, nullptr);
				hButtonOpen = CreateWindowEx(0, L"BUTTON", L"Открыть файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 220, 120, 20, hWnd, (HMENU)(105), hInstance, nullptr);
				hTextOpen = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 220, 200, 20, hWnd, (HMENU)(7), hInstance, nullptr);
				hButtonSave = CreateWindowEx(0, L"BUTTON", L"Сохранить файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 250, 120, 20, hWnd, (HMENU)(106), hInstance, nullptr);
				hTextSave = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 20, 250, 200, 100, hWnd, (HMENU)(8), hInstance, nullptr);
				hList = CreateWindowEx(0, L"ListBox", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 400, 20, 350, 100, hWnd, (HMENU)(9), hInstance, nullptr);
				hButtonFind = CreateWindowEx(0, L"BUTTON", L"Найти файл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 610, 130, 140, 20, hWnd, (HMENU)(107), hInstance, nullptr);
				hTextFind = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 400, 130, 200, 20, hWnd, (HMENU)(10), hInstance, nullptr);
				hButtonAttributes = CreateWindowEx(0, L"BUTTON", L"Сменить атр. чтения", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 610, 160, 140, 20, hWnd, (HMENU)(108), hInstance, nullptr);
				hTextAttributes = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 400, 160, 200, 20, hWnd, (HMENU)(11), hInstance, nullptr);
				hButtonProperties = CreateWindowEx(0, L"BUTTON", L"Свойства файла", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 610, 200, 140, 20, hWnd, (HMENU)(109), hInstance, nullptr);
				hTextProperties = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 400, 200, 200, 20, hWnd, (HMENU)(15), hInstance, nullptr);
				hTextSize = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 470, 230, 200, 20, hWnd, (HMENU)(12), hInstance, nullptr);
				hTextDate = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 470, 252, 200, 20, hWnd, (HMENU)(13), hInstance, nullptr);
				hTextLastAccess = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 470, 274, 200, 20, hWnd, (HMENU)(14), hInstance, nullptr);
				hButtonHardLink = CreateWindowEx(0, L"BUTTON", L"Жесткая ссылка", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 610, 300, 140, 50, hWnd, (HMENU)(110), hInstance, nullptr);
				hTextHardLink = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 400, 300, 200, 20, hWnd, (HMENU)(16), hInstance, nullptr);
				hTextHardLinkFile = CreateWindowEx(0, L"Edit", nullptr, WS_BORDER | WS_CHILD | WS_VISIBLE, 400, 330, 200, 20, hWnd, (HMENU)(17), hInstance, nullptr);
			}
		return 0;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
		return 0;

		default:

			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsrance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg{};

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyClass";
	wc.lpszMenuName = 0;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WinProc;

	if (!(RegisterClassEx(&wc)))
		return FALSE;

	if ((hWnd = CreateWindowEx(0, wc.lpszClassName, L"Window", (WS_OVERLAPPEDWINDOW&~WS_THICKFRAME) | WS_VISIBLE, 50, 50, 800, 400, nullptr, nullptr, wc.hInstance, nullptr)) == INVALID_HANDLE_VALUE)
		return FALSE;

	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, nullptr, 0, 0))
		{
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	

	return static_cast<int>(msg.wParam);
}