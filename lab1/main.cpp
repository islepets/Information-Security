#include <iostream>
#include<Windows.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <numeric>
#include <cmath>
#include <vector>
#include <map>

#define INDEX_BUTTON_GEN 1
#define INDEX_BUTTON_TEST1 4 
#define INDEX_BUTTON_TEST2 5
#define INDEX_BUTTON_TEST3 6

HWND hWndTextPanel = NULL;
HWND hWndEditLength = NULL;
HWND hWndResult = NULL;

bool frequency_test(const wchar_t* content, int len);
bool posl_equal_bit(wchar_t* content, int len);
bool deviations(wchar_t* content, int len);

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	

	switch (msg)
	{
		case WM_CREATE:
		{
			CreateWindowW(L"Static", L"Длина последовательности:", WS_CHILD | WS_VISIBLE, 10, 10, 200, 20, hwnd, NULL, NULL, NULL);
			hWndEditLength = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 210, 10, 105, 20, hwnd, NULL, NULL, NULL);
			CreateWindowW(L"Button", L"Сгенерировать последовательность", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 35, 305, 30, hwnd, (HMENU)INDEX_BUTTON_GEN, NULL, NULL);
			hWndTextPanel = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL, 320, 10, 650, 540, hwnd, NULL, NULL, NULL);
			CreateWindowW(L"Button", L"Частотный тест", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 450, 305, 30, hwnd, (HMENU)INDEX_BUTTON_TEST1, NULL, NULL);
			CreateWindowW(L"Button", L"Тест на последовательность одинаковых бит", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 485, 305, 30, hwnd, (HMENU)INDEX_BUTTON_TEST2, NULL, NULL);
			hWndResult = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY, 10, 70, 305, 375, hwnd, NULL, NULL, NULL);
			CreateWindowW(L"Button", L"Тест на произвольные отклонения", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 520, 305, 30, hwnd, (HMENU)INDEX_BUTTON_TEST3, NULL, NULL);
			break;
		}
		case WM_COMMAND:
		{
			if (LOWORD(wparam) == INDEX_BUTTON_GEN)
			{
				wchar_t buffer[256];
				GetWindowTextW(hWndEditLength, buffer, 256);

				int number = _wtoi(buffer);

				if (number < 10000)
				{
					MessageBox(hwnd, L"Введите более длинную последовательность", L"Создание", MB_OK);
					break;
				}

				std::ofstream outfile("posl.txt", std::ios::trunc);
				if (outfile.is_open())
				{
					for (int i = 0; i < number; i++)
					{
						outfile << rand() % 2;
					}
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
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

int WINAPI WinMain(HINSTANCE hInstanse, HINSTANCE hPrecInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(0));
	HWND hwnd;
	MSG msg;
	WNDCLASSEXW wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.hIcon = LoadIconW(NULL, IDI_ASTERISK);
	wc.hIconSm = LoadIconW(NULL, IDI_ASTERISK);
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);

	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hInstance = hInstanse;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"WinMainClass";

	RegisterClassExW(&wc);
	hwnd = CreateWindowExW(0, wc.lpszClassName, L"Лабораторная работа №1", WS_OVERLAPPEDWINDOW, 300, 100, 1000, 600, NULL, NULL, hInstanse, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return msg.wParam;
}