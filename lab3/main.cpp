#include <windows.h>
#include <locale>
#include <fstream>
#include <iostream>
#include <string>
//#include <openssl/md5.h>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <commdlg.h> 

#pragma warning(disable: 4996)  

#define MD_5			2
#define MaHash8v64		3
#define SHIFR			4
#define DESHIFR			5
#define SELECT_FILE		7 

HWND hWndTextPanel = NULL;
HWND hWndSeed = NULL; 
HWND hWndShifr = NULL; 
HWND hWndFilePath = NULL; 

HWND hButtonShift = NULL;
HWND hButtonDeShift = NULL;

static const unsigned char sTable[256] =
{
 0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
 0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
 0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
 0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
 0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
 0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
 0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
 0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
 0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
 0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
 0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
 0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
 0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
 0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
 0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
 0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

static __int32 LFSR(__int32 x0, __int32 x1, __int32 x2, __int32 x3, __int32 x4, unsigned long long& ShiftRegister)
{
	ShiftRegister = ((((ShiftRegister >> x0)
		^ (ShiftRegister >> x1) ^ (ShiftRegister >> x2) ^ (ShiftRegister >> x3) ^ (ShiftRegister >> x4)
		^ ShiftRegister) & 0x00000001) << 31) | (ShiftRegister >> 1);

	return ShiftRegister & 0x00000001;
}
static std::string stop_and_go(__int64 number, unsigned long long seed)
{
	unsigned long long lfsr1, lfsr2, lfsr3, prev_lfsr1;
	unsigned long long state1, state2, state3;
	std::string res = "";

	state1 = seed + 0x1111111;
	state2 = seed + 0x2222222;
	state3 = seed + 0x3333333;

	state1 = ((state1 << 13) ^ (state1 >> 19)) ^ seed;
	state2 = ((state2 << 17) ^ (state2 >> 15)) ^ seed;
	state3 = ((state3 << 19) ^ (state3 >> 13)) ^ seed;

	lfsr1 = LFSR(31, 7, 9, 1, 0, state1);
	lfsr2 = LFSR(31, 27, 9, 21, 0, state2);
	lfsr3 = LFSR(31, 8, 19, 10, 3, state3);

	prev_lfsr1 = lfsr1;

	for (int i = 0; i < number; i++)
	{
		lfsr1 = LFSR(31, 7, 9, 1, 0, state1);

		if (prev_lfsr1 == 1) { lfsr2 = LFSR(31, 27, 9, 21, 0, state2); }

		lfsr3 = LFSR(31, 8, 19, 10, 3, state3);

		(lfsr2 ^ lfsr3) ? res += "1" : res += "0";

		prev_lfsr1 = lfsr1;
	}

	return res;
}

// Шифрование
// MD-5 из библиотеки OpenSSL
/*std::string md5(const std::wstring& str)
{
	unsigned char hash[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)str.c_str(), str.length() * sizeof(wchar_t), hash);

	std::stringstream ss;
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

	return ss.str();
}*/

// MaHash8v64
#define LROT14(x) (((x) << 14) | ((x) >> 18)) // макрос сдвига влево
#define RROT14(x) (((x) << 18) | ((x) >> 14)) // макрос сдвига вправо
unsigned long long _MaHash8v64(unsigned char* str, unsigned int len)
{
	unsigned int sh1, sh2, hash1 = len, hash2 = len, i;
	unsigned long long digest;

	for (i = 0; i != len; i++, str++) // для каждого байта строки выполняется операция
	{
		hash1 += sTable[(*str + i) & 255]; // берем значение из таблицы
		hash1 = LROT14(hash1 + ((hash1 << 6) ^ (hash1 >> 11))); // делаем циклический сдвиг

		hash2 += sTable[(*str + i) & 255]; // так же берем значение из таблицы
		hash2 = RROT14(hash2 + ((hash2 << 6) ^ (hash2 >> 11))); // делаем циклический сдвиг

		// Алгоритм перемешивания половинок 
		sh1 = hash1; 
		sh2 = hash2;
		hash1 = ((sh1 >> 16) & 0xffff) | ((sh2 & 0xffff) << 16); 
		hash2 = ((sh2 >> 16) & 0xffff) | ((sh1 & 0xffff) << 16);
	}

	digest = (((unsigned long long)hash2) << 32) | ((unsigned long long)hash1); // итоговая сборка результата
	return digest;
}

std::wstring SelectFile(HWND hwnd)
{
	OPENFILENAMEW ofn;
	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0Все файлы (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"txt";

	if (GetOpenFileNameW(&ofn))
	{
		return std::wstring(szFileName);
	}

	return L"";
}
std::wstring ReadFileContent(const std::wstring& filePath)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		return L"";
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0)
	{
		return L"";
	}

	std::string content(size, '\0');
	file.read(&content[0], size);
	file.close();

	int len = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, NULL, 0);
	std::wstring wcontent(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, &wcontent[0], len);

	return wcontent;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		CreateWindowW(L"Static", L"Ключ:", WS_CHILD | WS_VISIBLE, 10, 10, 177, 20, hwnd, NULL, NULL, NULL);
		hWndSeed = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 188, 9, 177, 22, hwnd, NULL, NULL, NULL);

		hWndTextPanel = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL, 370, 10, 400, 100, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"Button", L"MD-5", WS_CHILD | WS_VISIBLE, 10, 38, 355, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"Button", L"MaHash8v64", WS_CHILD | WS_VISIBLE, 10, 60, 355, 20, hwnd, (HMENU)MaHash8v64, NULL, NULL);

		CreateWindowW(L"Button", L"Выбрать файл", WS_CHILD | WS_VISIBLE, 10, 85, 100, 20, hwnd, (HMENU)SELECT_FILE, NULL, NULL);
		hWndFilePath = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 115, 85, 250, 22, hwnd, NULL, NULL, NULL);

		hWndShifr = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL, 10, 265, 960, 190, hwnd, NULL, NULL, NULL);

		hButtonShift = CreateWindowW(L"Button", L"Шифрование", WS_CHILD | WS_VISIBLE, 10, 240, 100, 20, hwnd, (HMENU)SHIFR, NULL, NULL);
		hButtonDeShift = CreateWindowW(L"Button", L"Дешифрование", WS_CHILD | WS_VISIBLE, 115, 240, 110, 20, hwnd, (HMENU)DESHIFR, NULL, NULL);

		ShowWindow(hButtonDeShift, SW_HIDE);
		break;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == MD_5)
		{
			wchar_t buffer[255];
			GetWindowText(hWndSeed, buffer, 256);
			if (buffer[0] == L'\0')
			{
				MessageBox(hwnd, L"Введите seed", L"Создание", MB_OK);
				break;
			}

			std::wstring str(buffer);
			//std::string hash = md5(str);

			std::ofstream outfile_pas("password.txt", std::ios::trunc);
			if (outfile_pas.is_open())
			{
				//outfile_pas << hash;
				outfile_pas.close();
			}
			//std::wstring hashWide(hash.begin(), hash.end());
			//SetWindowTextW(hWndTextPanel, hashWide.c_str());
		}

		if (LOWORD(wParam) == MaHash8v64)
		{
			wchar_t buffer[256];
			GetWindowText(hWndSeed, buffer, 256);
			if (buffer[0] == L'\0')
			{
				MessageBox(hwnd, L"Введите seed", L"Создание", MB_OK);
				break;
			}

			int len = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
			char* content = new char[len];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, content, len, NULL, NULL);

			__int64 hash = _MaHash8v64((unsigned char*)content, len-1);

			delete[]content;

			std::ofstream outfile_pas("password.txt", std::ios::trunc);
			if (outfile_pas.is_open())
			{
				outfile_pas << hash;
				outfile_pas.close();
			}

			std::stringstream ss;
			ss << std::hex << std::setw(16) << std::setfill('0') << hash;
			std::string hashHex = ss.str();
			std::wstring hashWide(hashHex.begin(), hashHex.end());
			SetWindowTextW(hWndTextPanel, hashWide.c_str());
		}

		if (LOWORD(wParam) == SHIFR || (LOWORD(wParam) == DESHIFR))
		{
			if (LOWORD(wParam) == SHIFR) {
				ShowWindow(hButtonShift, SW_HIDE);
				ShowWindow(hButtonDeShift, SW_SHOW);
			}
			else {
				ShowWindow(hButtonShift, SW_SHOW);
				ShowWindow(hButtonDeShift, SW_HIDE);
			}

			uint32_t len_shifr = GetWindowTextLengthW(hWndShifr);
			wchar_t* posl = new wchar_t[len_shifr + 1];
			GetWindowTextW(hWndShifr, posl, len_shifr + 1);

			std::wstring current_text(posl);
			bool is_hex_data = true;
			for (size_t i = 0; i < current_text.length() && i < 100; i++) {
				if (!iswxdigit(current_text[i])) {
					is_hex_data = false;
					break;
				}
			}

			int bytes_len;
			char* bytes_data;
			std::wstring original_text;

			if (is_hex_data && (LOWORD(wParam) == DESHIFR)) {
				bytes_len = current_text.length() / 2;
				bytes_data = new char[bytes_len];
				for (int i = 0; i < bytes_len; i++) {
					std::wstring byte_str = current_text.substr(i * 2, 2);
					bytes_data[i] = (char)wcstol(byte_str.c_str(), nullptr, 16);
				}
				original_text = L"";
			}
			else {
				bytes_len = WideCharToMultiByte(CP_ACP, 0, posl, len_shifr, NULL, 0, NULL, NULL);
				bytes_data = new char[bytes_len];
				WideCharToMultiByte(CP_ACP, 0, posl, len_shifr, bytes_data, bytes_len, NULL, NULL);
				original_text = current_text;
			}

			wchar_t buffer[256];
			GetWindowTextW(hWndTextPanel, buffer, 256);

			std::wstring hash_str(buffer);
			unsigned long long key = 0;
			size_t hex_len = hash_str.length();
			if (hex_len > 16) hex_len = 16;
			std::wstring first_part = hash_str.substr(0, hex_len);
			key = wcstoull(first_part.c_str(), nullptr, 16);

			std::string gen_bits = stop_and_go(bytes_len * 8, key);

			std::string encrypted_bytes;
			encrypted_bytes.resize(bytes_len);

			for (int i = 0; i < bytes_len; i++) {
				unsigned char byte_mask = 0;
				for (int bit = 0; bit < 8; bit++) {
					int gen_bit = gen_bits[i * 8 + bit] - '0';
					byte_mask |= (gen_bit << (7 - bit));
				}
				encrypted_bytes[i] = bytes_data[i] ^ byte_mask;
			}

			std::wstring result_text;

			if (LOWORD(wParam) == SHIFR) {
				std::stringstream hex_stream;
				hex_stream << std::hex << std::setfill('0');
				for (int i = 0; i < bytes_len; i++) {
					hex_stream << std::setw(2) << (unsigned int)(unsigned char)encrypted_bytes[i];
				}
				std::string hex_result = hex_stream.str();
				result_text = std::wstring(hex_result.begin(), hex_result.end());
			}
			else {
				int wchar_len = MultiByteToWideChar(CP_ACP, 0, encrypted_bytes.c_str(), bytes_len, NULL, 0);
				if (wchar_len > 0) {
					wchar_t* result = new wchar_t[wchar_len + 1];
					MultiByteToWideChar(CP_ACP, 0, encrypted_bytes.c_str(), bytes_len, result, wchar_len);
					result[wchar_len] = L'\0';
					result_text = result;
					delete[] result;
				}
			}

			SetWindowTextW(hWndShifr, result_text.c_str());

			delete[] posl;
			delete[] bytes_data;
		}

		if (LOWORD(wParam) == SELECT_FILE)
		{
			std::wstring filePath = SelectFile(hwnd);
			if (!filePath.empty())
			{
				SetWindowTextW(hWndFilePath, filePath.c_str());

				std::wstring fileContent = ReadFileContent(filePath);
				if (!fileContent.empty())
				{
					SetWindowTextW(hWndShifr, fileContent.c_str());
					MessageBoxW(hwnd, L"Файл успешно загружен!", L"Информация", MB_OK);
				}
				else
				{
					MessageBoxW(hwnd, L"Не удалось прочитать файл или файл пуст!", L"Ошибка", MB_OK);
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
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;

	setlocale(LC_ALL, "Rus");

	WNDCLASSEX wc = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIconW(NULL, IDI_ASTERISK),
		.hCursor = LoadCursorW(NULL, IDC_ARROW),
		.hbrBackground = CreateSolidBrush(RGB(135, 18, 18)),
		.lpszMenuName = NULL,
		.lpszClassName = L"WndMain",
		.hIconSm = LoadIconW(NULL, IDI_ASTERISK)
	};

	RegisterClassExW(&wc);
	hwnd = CreateWindowExW(0, wc.lpszClassName, L"Лабораторная работа №3", WS_OVERLAPPEDWINDOW, 300, 100, 1000, 500, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;
}