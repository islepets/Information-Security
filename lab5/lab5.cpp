#include <iostream>
#include <windows.h>
#include <algorithm>
#include <vector>
#include <locale>
#include <math.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

enum ids {
	ID0 = 0,
	ID1,
	ID2,
	ID3,
	ID4,
	ID5,
	ID6,
	ID7
};

HWND hBorder = NULL;
HWND hSimple = NULL;
HWND hKeys = NULL;
HWND hText = NULL;
HWND hShifr = NULL;
HWND HDeshifr = NULL;


static uint32_t _p, _g, _y, _x;

vector<pair<uint32_t, uint32_t>> blocks;

unsigned long pow_on_mod(unsigned long x, unsigned long y, unsigned long n) {
	unsigned long s, t, u;
	int i;
	s = 1; t = x; u = y;
	while (u) {
		if (u & 1) s = (s * t) % n;
		u >>= 1;
		t = (t * t) % n;
	}
	return(s);
}

class GenetateSimpleNumber {
private:

	uint32_t input_number;


	bool test_Miller_Rabin(unsigned long long m, int iterations = 5) {
		if (m == 2 || m == 3) return true;
		if (m < 2 || m % 2 == 0) return false;

		unsigned long long d = m - 1;
		unsigned long long s = 0;

		while (d % 2 == 0) {
			d /= 2;
			s++;
		}

		for (int i = 0; i < iterations; i++) {
			// Выбираем a от 2 до m-2
			unsigned long long a;
			if (m <= 4) {
				a = 2;
			}
			else {
				a = 2 + (rand() % (m - 3));
			}

			unsigned long long x = pow_on_mod(a, d, m);

			if (x == 1 || x == m - 1) {
				continue;
			}

			bool composite = true;
			for (unsigned long long j = 0; j < s - 1; j++) {
				x = pow_on_mod(x, 2, m);
				if (x == m - 1) {
					composite = false;
					break;
				}
				if (x == 1) {
					return false;
				}
			}

			if (composite) {
				return false;
			}
		}

		return true;
	}

	uint32_t find_next(uint32_t start) {
		if (start < 2) start = 2;
		if (start % 2 == 0 && start > 2) start++;

		uint32_t temp = start;

		while (true) {
			if (test_Miller_Rabin(temp, 10)) {
				return temp;
			}
			temp += 2;
		}
	}

public:

	GenetateSimpleNumber(uint32_t input_number) {
		this->input_number = input_number;
	}

	uint32_t run() {
		return find_next(input_number);
	}
};

class GenerateKey {
private:
	uint32_t p;
	vector<uint32_t> divisiors;
	uint32_t g;
	uint32_t x;
	uint32_t y;

	void findDivisiors(int n) {
		for (int i = 2; i <= sqrt(n); i++) {
			if (n % i == 0)
				divisiors.push_back(i);
			if (i * i != n) {
				divisiors.push_back(n / i);
			}
		}

		sort(divisiors.begin(), divisiors.end());
	}
	void searchG() {

		findDivisiors(p - 1);

		for(g = 2; g < p; g++){
			bool status = true;
			for (int i = 0; i < divisiors.size(); i++) {
				if (pow_on_mod(g, (p - 1) / divisiors[i], p) == 1) {
					status = false;
					break;
				}
			}
			if (status == true) return;
		}

		g = 2;
	}
	void searchX() {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(2, p - 2);

		x = dis(gen);
	}

public:

	GenerateKey(uint32_t p) {
		this->p = p;
	}

	void calculate() {
		searchG();
		searchX();

		y = pow_on_mod(g, x, p);
	}

	void get(uint32_t& g, uint32_t& y, uint32_t& x) {
		g = this->g;
		y = this->y;
		x = this->x;
	}

};

class El_Gomal {
private:
	uint32_t k;
	uint32_t p;

	int gcd(int x, int y) {
		int g;
		if (x < 0) x = -x;
		if (y < 0) y = -y;
		if (x + y == 0) return -1;
		g = y;
		while (x > 0) {
			g = x;
			x = y % x;
			y = g;
		}
		return g;
	}

	void findK() {
		do {
			k = 2 + rand() % (p - 3);
		} while (gcd(k, p - 1) != 1);
	}

	uint32_t mod_inverse(uint32_t a, uint32_t m) {
		int32_t m0 = m, t, q;
		int32_t x0 = 0, x1 = 1;

		if (m == 1) return 0;

		while (a > 1) {
			q = a / m;
			t = m;
			m = a % m;
			a = t;
			t = x0;
			x0 = x1 - q * x0;
			x1 = t;
		}

		if (x1 < 0) x1 += m0;

		return (uint32_t)x1;
	}

public:

	El_Gomal(uint32_t p) {
		this->p = p;
	}


	void encrypt(uint32_t& a, uint32_t& b, uint32_t M, uint32_t g, uint32_t y) {

		findK();

		a = pow_on_mod(g, k, p);
		b = (pow_on_mod(y, k, p) * M) % p;
	}

	uint32_t decrypt(uint32_t a, uint32_t b, uint32_t x) {
		uint32_t s = pow_on_mod(a, x, p);

		uint32_t s_inv = mod_inverse(s, p);
		return (b * s_inv) % p;
	}

};


LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
		case WM_CREATE:
		{
			CreateWindowW(L"Static", L"Граница:", WS_CHILD | WS_VISIBLE, 10, 10, 60, 20, hwnd, (HMENU)ID0, NULL, NULL);
			hBorder = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE, 75, 10, 115, 20, hwnd, (HMENU)ID1, NULL, NULL);
			CreateWindowW(L"Button", L"Генерация простого числа", WS_CHILD | WS_VISIBLE, 10, 35, 180, 40, hwnd, (HMENU)ID2, NULL, NULL);
			hSimple = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | ES_READONLY, 10, 80, 180, 20, hwnd, (HMENU)ID3, NULL, NULL);
			hKeys = CreateWindowW(L"Button", L"Генерация ключей", WS_CHILD | WS_VISIBLE, 10, 105, 180, 40, hwnd, (HMENU)ID4, NULL, NULL);
			hText = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL, 200, 10, 500, 400, hwnd, (HMENU)ID5, NULL, NULL);
			hShifr = CreateWindowW(L"Button", L"Шифрование", WS_CHILD | WS_VISIBLE, 330, 420, 180, 40, hwnd, (HMENU)ID6, NULL, NULL);
			HDeshifr = CreateWindowW(L"Button", L"Дешифрование", WS_CHILD | WS_VISIBLE, 520, 420, 180, 40, hwnd, (HMENU)ID7, NULL, NULL);
			break;
		}

		case WM_COMMAND:
		{
			if (LOWORD(wParam) == ID2){
				/// Используем алгоритм Рябина-Миллера

				wchar_t buffer[256];
				GetWindowText(hBorder, buffer, 256);
				if (buffer[0] == L'\0') {
					MessageBox(hwnd, L"Введите границу", L"Создание", MB_OK);
					break;
				}

				int32_t border = _wtoi(buffer);
				GenetateSimpleNumber genSimNum(border);
				uint32_t res = genSimNum.run();

				wsprintf(buffer, L"%d", res);
				SetWindowText(hSimple, buffer);
			}

			if (LOWORD(wParam) == ID4) {
				// генерируем ключи

				wchar_t buffer[256];
				GetWindowText(hSimple, buffer, 256);
				if (buffer[0] == L'\0') {
					MessageBox(hwnd, L"Сгенерируйте простое число", L"Создание", MB_OK);
					break;
				}

				int32_t p = _wtoi(buffer);
				GenerateKey gk(p);
				gk.calculate();

				_p = p;
				gk.get(_g, _y, _x);

				wchar_t output[256];
				wsprintf(output, L"p=%d\ng=%d\ny=%d", _p, _g, _y);
				MessageBoxW(hwnd, output, L"Подсчитанные значения", MB_OK);
			}

			if LOWORD(wParam == ID6) {

				blocks.clear();
				
				uint32_t lenght = GetWindowTextLength(hText);
				wchar_t* buffer = new wchar_t[lenght + 1];
				GetWindowText(hText, buffer, lenght + 1);
				if (buffer[0] == L'\0') {
					MessageBox(hwnd, L"Введите текст для шифрования", L"Ошибка", MB_OK);
					break;
				}

				El_Gomal elg(_p);

				wstring ciphertext_display;

				for (int i = 0; i < lenght; i++) {
					uint32_t M = buffer[i];
					uint32_t a, b;
					elg.encrypt(a, b, M, _g, _y);

					blocks.push_back({ a, b });

					wchar_t block[64];
					wsprintf(block, L"[%u,%u] ", a, b);
					ciphertext_display += block;
				}
				
				SetWindowTextW(hText, ciphertext_display.c_str());
				delete[]buffer;
			}

			if (LOWORD(wParam) == ID7) {

				wstring str;
				El_Gomal gl(_p);

				for (int i = 0; i < blocks.size(); i++) {
					str += (wchar_t)gl.decrypt(blocks[i].first, blocks[i].second, _x);
				}
				SetWindowText(hText, str.c_str());
			}

			break;	
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	return NULL;
}


int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HWND hwnd;
	MSG msg;

	setlocale(LC_ALL, "Rus");

	srand(time(NULL));

	WNDCLASSEX wc = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIconW(NULL, IDI_ASTERISK),
		.hCursor = LoadCursorW(NULL, IDC_ARROW),
		.hbrBackground = CreateSolidBrush(RGB(156, 34, 227)),
		.lpszMenuName = NULL,
		.lpszClassName = L"WndMain",
		.hIconSm = LoadIconW(NULL, IDI_ASTERISK)
	};

	RegisterClassExW(&wc);
	hwnd = CreateWindowExW(0, wc.lpszClassName, L"Лабораторная работа №5 - Ассиметричная криптография", WS_OVERLAPPEDWINDOW, 300, 100, 730, 500, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;
}