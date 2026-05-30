#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstdlib>
#include <string>
#include <numeric>
#include <cmath>
#include <vector>
#include <map>

#define INDEX_BUTTON_GEN_1 1
#define INDEX_BUTTON_GEN_2 2
#define INDEX_BUTTON_GEN_3 3
#define INDEX_BUTTON_TEST1 4 
#define INDEX_BUTTON_TEST2 5
#define INDEX_BUTTON_TEST3 6

HWND hWndTextPanel = NULL;
HWND hWndEditLength = NULL;
HWND hWndResult = NULL;
HWND hWndSeed = NULL;

bool frequency_test(const wchar_t* content, int len);
bool posl_equal_bit(wchar_t* content, int len);
bool deviations(wchar_t* content, int len);
std::string parka_millera(__int64 number, __int64 seed);
std::string stop_and_go(__int64 number, __int64 seed);


LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		CreateWindowW(L"Static", L"Длина последовательности:", WS_CHILD | WS_VISIBLE, 10, 10, 200, 20, hwnd, NULL, NULL, NULL);
		hWndEditLength = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 210, 9, 105, 22, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"Static", L"Ключ:", WS_CHILD | WS_VISIBLE, 350, 10, 45, 20, hwnd, NULL, NULL, NULL);
		hWndSeed = CreateWindowW(L"EDit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 395, 9, 255, 22, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"Button", L"Генератор Парка-Миллера", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 35, 305, 30, hwnd, (HMENU)INDEX_BUTTON_GEN_1, NULL, NULL);
		CreateWindowW(L"Button", L"Генератор Стоп-Пошёл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 305, 30, hwnd, (HMENU)INDEX_BUTTON_GEN_2, NULL, NULL);

		CreateWindowW(L"Button", L"Частотный тест", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 545, 180, 30, hwnd, (HMENU)INDEX_BUTTON_TEST1, NULL, NULL);
		CreateWindowW(L"Button", L"Тест на последовательность одинаковых бит", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 545, 305, 30, hwnd, (HMENU)INDEX_BUTTON_TEST2, NULL, NULL);
		CreateWindowW(L"Button", L"Тест на произвольные отклонения", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 510, 545, 305, 30, hwnd, (HMENU)INDEX_BUTTON_TEST3, NULL, NULL);

		CreateWindowW(L"Static", L"Результат работы:", WS_CHILD | WS_VISIBLE, 100, 125, 125, 20, hwnd, NULL, NULL, NULL);
		hWndResult = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY, 10, 150, 305, 300, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"Static", L"Последовательность:", WS_CHILD | WS_VISIBLE, 520, 45, 145, 20, hwnd, NULL, NULL, NULL);
		hWndTextPanel = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL, 350, 70, 465, 460, hwnd, NULL, NULL, NULL);

		break;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wparam) == INDEX_BUTTON_GEN_1)
		{
			wchar_t buffer[256];
			GetWindowText(hWndEditLength, buffer, 256);

			if (buffer[0] == L'\0')
			{
				MessageBox(hwnd, L"Последовательность не введена", L"Создание", MB_OK);
				break;
			}
			
			__int32 size_str = _wtoi(buffer);

			if (size_str < 10'000)
			{
				MessageBox(hwnd, L"Введите более длинную последовательность", L"Создание", MB_OK);
				break;
			}

			wchar_t seed[256];
			GetWindowText(hWndSeed, seed, 256);

			if (seed[0] == L'\0')
			{
				MessageBox(hwnd, L"Seed не введён", L"Создание", MB_OK);
				break;
			}

			unsigned __int64 key = _wtoi(seed);

			std::ofstream outfile("posl.txt", std::ios::trunc);
			if (outfile.is_open())
			{
				outfile << parka_millera(size_str, key);
			}
			outfile.close();

			SetWindowTextW(hWndResult, L"");
			MessageBox(hwnd, L"Последовательность создана!", L"Создание", MB_OK);

			std::ifstream infile("posl.txt");
			if (infile.is_open())
			{

				std::string content;
				std::getline(infile, content);
				infile.close();

				if (content.empty())
				{
					MessageBoxW(hwnd, L"Файл пуст!", L"Предупреждение", MB_OK);
					SetWindowTextW(hWndTextPanel, L"");
					break;
				}
				else
				{
					int len = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, NULL, 0);
					wchar_t* wcontent = new wchar_t[len];
					MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, wcontent, len);
					SetWindowTextW(hWndTextPanel, wcontent);
					delete[] wcontent;
				}
			}
		}

		if (LOWORD(wparam) == INDEX_BUTTON_GEN_2)
		{
			wchar_t buffer[256];
			GetWindowText(hWndEditLength, buffer, 256);

			if (buffer[0] == L'\0')
			{
				MessageBox(hwnd, L"Последовательность не введена", L"Создание", MB_OK);
				break;
			}

			__int32 size_str = _wtoi(buffer);

			if (size_str < 10'000)
			{
				MessageBox(hwnd, L"Введите более длинную последовательность", L"Создание", MB_OK);
				break;
			}

			wchar_t seed[256];
			GetWindowText(hWndSeed, seed, 256);

			if (seed[0] == L'\0')
			{
				MessageBox(hwnd, L"Seed не введён", L"Создание", MB_OK);
				break;
			}

			unsigned __int64 key = _wtoi(seed);

			std::ofstream outfile("posl.txt", std::ios::trunc);
			if (outfile.is_open())
			{
				outfile << stop_and_go(size_str, key);
			}
			outfile.close();

			SetWindowTextW(hWndResult, L"");
			MessageBox(hwnd, L"Последовательность создана!", L"Создание", MB_OK);

			std::ifstream infile("posl.txt");
			if (infile.is_open())
			{

				std::string content;
				std::getline(infile, content);
				infile.close();

				if (content.empty())
				{
					MessageBoxW(hwnd, L"Файл пуст!", L"Предупреждение", MB_OK);
					SetWindowTextW(hWndTextPanel, L"");
					break;
				}
				else
				{
					int len = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, NULL, 0);
					wchar_t* wcontent = new wchar_t[len];
					MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, wcontent, len);
					SetWindowTextW(hWndTextPanel, wcontent);
					delete[] wcontent;
				}
			}
		}

		if (LOWORD(wparam) == INDEX_BUTTON_TEST1)
		{
			std::ifstream infile("posl.txt");
			if (infile.is_open())
			{

				std::string content;
				std::getline(infile, content);
				infile.close();

				if (content.empty())
				{
					MessageBoxW(hwnd, L"Файл пуст!", L"Предупреждение", MB_OK);
					SetWindowTextW(hWndTextPanel, L"");
					break;
				}
				else
				{
					int len = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, NULL, 0);
					wchar_t* wcontent = new wchar_t[len];
					MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, wcontent, len);

					bool res = frequency_test(wcontent, len);
					(res) ? MessageBox(hwnd, L"Тест пройден!", L"Частотный тест!", MB_OK) : MessageBox(hwnd, L"Тест НЕ пройден!", L"Частотный тест!", MB_OK);

					delete[] wcontent;
				}
			}
		}

		if (LOWORD(wparam) == INDEX_BUTTON_TEST2)
		{
			std::ifstream infile("posl.txt");
			if (infile.is_open())
			{

				std::string content;
				std::getline(infile, content);
				infile.close();

				if (content.empty())
				{
					MessageBoxW(hwnd, L"Файл пуст!", L"Предупреждение", MB_OK);
					SetWindowTextW(hWndTextPanel, L"");
					break;
				}
				else
				{
					int len = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, NULL, 0);
					wchar_t* wcontent = new wchar_t[len];
					MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, wcontent, len);

					bool res = posl_equal_bit(wcontent, len);
					(res) ? MessageBox(hwnd, L"Тест пройден!", L"Тест на последовательность одинаковых бит!", MB_OK) : MessageBox(hwnd, L"Тест НЕ пройден!", L"Тест на последовательность одинаковых бит!", MB_OK);

					delete[] wcontent;
				}
			}
		}

		if (LOWORD(wparam) == INDEX_BUTTON_TEST3)
		{
			std::ifstream infile("posl.txt");
			if (infile.is_open())
			{

				std::string content;
				std::getline(infile, content);
				infile.close();

				if (content.empty())
				{
					MessageBoxW(hwnd, L"Файл пуст!", L"Предупреждение", MB_OK);
					SetWindowTextW(hWndTextPanel, L"");
					break;
				}
				else
				{
					int len = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, NULL, 0);
					wchar_t* wcontent = new wchar_t[len];
					MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, wcontent, len);

					bool res = deviations(wcontent, len);
					(res) ? MessageBox(hwnd, L"Тест пройден!", L"Тест на произвольные отклонения!", MB_OK) : MessageBox(hwnd, L"Тест НЕ пройден!", L"Тест на произвольные отклонения!", MB_OK);

					delete[] wcontent;
				}
			}
		}

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		std::ofstream outfile("posl.txt", std::ios::trunc);
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

int WINAPI WinMain(HINSTANCE hInstanse, HINSTANCE hPrecInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASSEXW wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.hIcon = LoadIconW(NULL, IDI_ASTERISK);
	wc.hIconSm = LoadIconW(NULL, IDI_ASTERISK);
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);

	wc.hbrBackground = CreateSolidBrush(RGB(54, 52, 179));
	wc.hInstance = hInstanse;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"WinMainClass";

	RegisterClassExW(&wc);
	hwnd = CreateWindowExW(0, wc.lpszClassName, L"Лабораторная работа №2", WS_OVERLAPPEDWINDOW, 300, 100, 840, 620, NULL, NULL, hInstanse, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return msg.wParam;
}