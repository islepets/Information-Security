#include <iostream>
#include <windows.h>
#include <locale>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

#define MaHash8v64		3
#define SHIFR			4
#define DESHIFR			5
#define CHECK_BOX		6

HWND hWndTextPanel = NULL;
HWND hWndSeed = NULL;
HWND hWndShifr = NULL;
HWND hButtonShift = NULL;
HWND hButtonDeShift = NULL;
HWND hCheckBox = NULL;

class CryptoGenerator {
private:
	static const unsigned char sTable[256];

	static __int32 LFSR(__int32 x0, __int32 x1, __int32 x2, __int32 x3, __int32 x4, unsigned long long& ShiftRegister) {
		ShiftRegister = ((((ShiftRegister >> x0)
			^ (ShiftRegister >> x1) ^ (ShiftRegister >> x2) ^ (ShiftRegister >> x3) ^ (ShiftRegister >> x4)
			^ ShiftRegister) & 0x00000001) << 31) | (ShiftRegister >> 1);

		return ShiftRegister & 0x00000001;
	}

#define LROT14(x) (((x) << 14) | ((x) >> 18))
#define RROT14(x) (((x) << 18) | ((x) >> 14))

public:
	static string stop_and_go(__int64 number, unsigned long long seed) {
		unsigned long long lfsr1, lfsr2, lfsr3, prev_lfsr1;
		unsigned long long state1, state2, state3;
		string res = "";

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

		for (int i = 0; i < number; i++) {
			lfsr1 = LFSR(31, 7, 9, 1, 0, state1);

			if (prev_lfsr1 == 1) {
				lfsr2 = LFSR(31, 27, 9, 21, 0, state2);
			}

			lfsr3 = LFSR(31, 8, 19, 10, 3, state3);

			(lfsr2 ^ lfsr3) ? res += "1" : res += "0";

			prev_lfsr1 = lfsr1;
		}

		return res;
	}

	static unsigned long long _MaHash8v64(unsigned char* str, unsigned int len) {
		unsigned int sh1, sh2, hash1 = len, hash2 = len, i;
		unsigned long long digest;

		for (i = 0; i != len; i++, str++) {
			hash1 += sTable[(*str + i) & 255];
			hash1 = LROT14(hash1 + ((hash1 << 6) ^ (hash1 >> 11)));

			hash2 += sTable[(*str + i) & 255];
			hash2 = RROT14(hash2 + ((hash2 << 6) ^ (hash2 >> 11)));

			sh1 = hash1;
			sh2 = hash2;
			hash1 = ((sh1 >> 16) & 0xffff) | ((sh2 & 0xffff) << 16);
			hash2 = ((sh2 >> 16) & 0xffff) | ((sh1 & 0xffff) << 16);
		}

		digest = (((unsigned long long)hash2) << 32) | ((unsigned long long)hash1);
		return digest;
	}
};

const unsigned char CryptoGenerator::sTable[256] = {
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

class MatrixShifr {
private:
	string input_string;
	vector<string> input_string_blocks;	
	double encryptMatrics[7][7];
	double decryptMatrics[7][7];
	long long shifr;
	string posl;
	vector<vector<int>> blocks_int;
	vector<int>iv;

public:

	vector<vector<int>> result;

	double determinant(double matrix[7][7], int n) {
		if (n == 1) return matrix[0][0];
		if (n == 2) {
			return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
		}

		double det = 0;
		double submatrix[7][7];

		for (int k = 0; k < n; k++) {
			int subi = 0;
			for (int i = 1; i < n; i++) {
				int subj = 0;
				for (int j = 0; j < n; j++) {
					if (j == k) continue;
					submatrix[subi][subj] = matrix[i][j];
					subj++;
				}
				subi++;
			}
			double sign = (k % 2 == 0) ? 1.0 : -1.0;
			det += sign * matrix[0][k] * determinant(submatrix, n - 1);
		}
		return det;
	}

	void getMinor(double matrix[7][7], double minor[7][7], int row, int col, int n) {
		int minor_i = 0, minor_j = 0;
		for (int i = 0; i < n; i++) {
			if (i == row) continue;
			minor_j = 0;
			for (int j = 0; j < n; j++) {
				if (j == col) continue;
				minor[minor_i][minor_j] = matrix[i][j];
				minor_j++;
			}
			minor_i++;
		}
	}

	void calculateInverseMatrix() {
		double det = determinant(encryptMatrics, 7);

		double cofactor[7][7];
		double minor[7][7];

		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 7; j++) {
				getMinor(encryptMatrics, minor, i, j, 7);
				double minorDet = determinant(minor, 6);
				double sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;
				cofactor[i][j] = sign * minorDet;
			}
		}

		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 7; j++) {
				decryptMatrics[i][j] = cofactor[j][i] / det;
			}
		}
	}

	// функция получения входной строки из поля private
	void getInputString(string str) {
		input_string = str; // просто присваем значение
	}

	// разбиение входной строки на строковые блоки
	void splitInputString() {
		input_string_blocks.clear(); // очищаем блоки, которые будем заполнять
		string str;
		for (int i = 0; i < input_string.length(); ++i) { // проходимся по всей строке
			str += input_string[i];  // накапливаем значения
			if (str.length() == 7) { // если длина нужная, тогда добавляем как блок
				input_string_blocks.push_back(str);
				str.clear();
			}
		}
		if (str.length() < 7 && !str.empty()) { // если юлок получается < 7, тогда добавим столько нулей, сколько не хватает
			while (str.length() < 7)
				str += '\0';
			input_string_blocks.push_back(str);
		}
	}

	// получение хэша из файла
	void getPoslInTxt(string name_txt) {
		ifstream infile(name_txt); // открываем файл по имени переданного аргумента
		string line;
		if (infile.is_open()) {
			getline(infile, line); // читаем вложенный в файл хэш
			infile.close();
		}
		shifr = stoull(line); // преобразуем строку в число
	}

	//генерируем последовательность (как улучшение -> передавать в аргумент функции размер генерируемой полседовательности, чтобы использовать для генерации iv)
	void GetPoslInGenerator() {
		posl = CryptoGenerator::stop_and_go(392, shifr);
	}


	// генерируем матрицу 7 на 7
	void GenerateMatrics() {
		int bitPos = 0;
		for (int i = 0; i < 7; i++) { // пройдемся по всем строкам матрицы
			for (int j = 0; j < 7; j++) { // здесь проходимся по столбцам матрицы
				uint32_t value = 0;
				for (int k = 0; k < 8; k++) { // так как байты состоят из 8 бит соответсивенно проходимся столько раз
					if (bitPos < posl.length()) {
						value = (value << 1) | (posl[bitPos] - '0'); // заполняем значения при этом переводя значения последовательности в числа
						bitPos++; // перейдем к следующей битовой позиции
					}
				}
				encryptMatrics[i][j] = (double)(value % 256); // заносим значения
			}
		}
		calculateInverseMatrix(); // после завершения, высчитываем обратную матрицу, для дешифрования
	}

	// преобразуем буквы в коды
	vector<int> transformOneBlock(string block) {
		vector<int> res(7, 0);
		for (int i = 0; i < 7; ++i) {
			res[i] = (unsigned char)block[i];
		}
		return res;
	}

	// делаем преобразование над всеми блоками
	void fullTransformBlocks() {
		blocks_int.clear();
		for (const auto& block : input_string_blocks) {
			blocks_int.push_back(transformOneBlock(block));
		}
	}

	// функция шифрования
	void _shifr() {
		result.clear();
		result.resize(blocks_int.size());
		for (auto& row : result) row.resize(7, 0); // очистим входной массив и заполним нулями

		for (int block = 0; block < blocks_int.size(); block++) { // идем по всем блокам
			for (int i = 0; i < 7; i++) { // идем по строчкам
				double sum = 0;
				for (int j = 0; j < 7; j++) { // здесь идем по столбцам
					sum += encryptMatrics[i][j] * blocks_int[block][j]; // накапливаем сумму
				}
				result[block][i] = (int)round(sum);
			}
		}
	}

	// функция дешифрования
	void _deshifr() {
		result.clear();
		result.resize(blocks_int.size());
		for (auto& row : result) row.resize(7, 0);

		for (int block = 0; block < blocks_int.size(); block++) {
			for (int i = 0; i < 7; i++) {
				double sum = 0;
				for (int j = 0; j < 7; j++) {
					sum += decryptMatrics[i][j] * blocks_int[block][j];
				}
				result[block][i] = (int)round(sum);
			}
		}
	}

	string getStringResult() {
		stringstream ss;
		for (auto& el : result) {
			for (int val : el) {
				ss << val << " ";
			}
		}
		return ss.str();
	}

	void parseEncryptedString(string encrypted) {
		blocks_int.clear();

		stringstream ss(encrypted);
		int val;
		vector<int> block;

		while (ss >> val) {
			block.push_back(val);
			if (block.size() == 7) {
				blocks_int.push_back(block);
				block.clear();
			}
		}
	}

	void generatorIV() {
		iv.clear();
		iv.resize(7, 0);

		string randomStr = CryptoGenerator::stop_and_go(56, shifr);

		for (int i = 0; i < 7; i++) {
			unsigned char bit_value = 0;
			for (int j = 0; j < 8; j++) {
				int bit_pos = i * 8 + j;
				if(bit_pos < randomStr.length())
					bit_value = (bit_value << 1) | (randomStr[bit_pos] - '0');
			}

			iv[i] = bit_value;
		}
	}


	vector<int> encryptGammaBlocks(vector<int>gamma) {
		vector<int>resultBlocks(7, 0);

		for (int i = 0; i < 7; i++) {
			double sum = 0;
			for (int j = 0; j < 7; j++) {
				sum += encryptMatrics[i][j] * gamma[j];
			}

			resultBlocks[i] = (int)round(sum);
			if (resultBlocks[i] < 0) resultBlocks[i] += 256;
			resultBlocks[i] %= 256;
		}

		return resultBlocks;
	}


	void shifrAndDeshifrByOFB() {

		result.clear();
		result.resize(blocks_int.size());
		for (auto& el : result) el.resize(7, 0);

		generatorIV();
		vector<int>gamma = iv;

		for (int i = 0; i < result.size(); i++) {
			vector<int> gammaBlocks = encryptGammaBlocks(gamma);
			for (int j = 0; j < 7; j++) {
				result[i][j] = blocks_int[i][j] ^ gammaBlocks[j];
			}
			gamma = gammaBlocks;
		}
	}


};

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CREATE: {
		CreateWindowW(L"Static", L"Ключ:", WS_CHILD | WS_VISIBLE, 10, 10, 40, 20, hwnd, NULL, NULL, NULL);
		hWndSeed = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 9, 130, 22, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"Button", L"MaHash8v64", WS_CHILD | WS_VISIBLE, 10, 32, 170, 20, hwnd, (HMENU)MaHash8v64, NULL, NULL);
		hWndTextPanel = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 181, 9, 300, 43, hwnd, NULL, NULL, NULL);
		hWndShifr = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 10, 100, 470, 190, hwnd, NULL, NULL, NULL);
		hButtonShift = CreateWindowW(L"Button", L"Шифрование", WS_CHILD | WS_VISIBLE, 10, 80, 100, 20, hwnd, (HMENU)SHIFR, NULL, NULL);
		hButtonDeShift = CreateWindowW(L"Button", L"Дешифрование", WS_CHILD | WS_VISIBLE, 115, 80, 110, 20, hwnd, (HMENU)DESHIFR, NULL, NULL);
		hCheckBox = CreateWindowEx(0, L"BUTTON", L"OFB", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_GROUP, 10, 55, 100, 20, hwnd, (HMENU)CHECK_BOX, NULL, NULL);
		break;
	}

	case WM_COMMAND: {
		if (LOWORD(wParam) == MaHash8v64) {
			wchar_t buffer[256];
			GetWindowText(hWndSeed, buffer, 256);
			if (buffer[0] == L'\0') {
				MessageBox(hwnd, L"Введите seed", L"Создание", MB_OK);
				break;
			}

			int len = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
			char* content = new char[len];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, content, len, NULL, NULL);

			unsigned long long hash = CryptoGenerator::_MaHash8v64((unsigned char*)content, len - 1);
			delete[] content;

			ofstream outfile_pas("password.txt", ios::trunc);
			if (outfile_pas.is_open()) {
				outfile_pas << hash;
				outfile_pas.close();
			}

			stringstream ss;
			ss << hex << setw(16) << setfill('0') << hash;
			string hashHex = ss.str();
			wstring hashWide(hashHex.begin(), hashHex.end());
			SetWindowTextW(hWndTextPanel, hashWide.c_str());
		}

		if (LOWORD(wParam) == SHIFR) {
			wchar_t buffer[4096];
			GetWindowText(hWndShifr, buffer, 4096);

			if (buffer[0] == L'\0') {
				MessageBox(hwnd, L"Введите текст для шифрования", L"Ошибка", MB_OK);
				break;
			}

			int len = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
			char* content = new char[len];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, content, len, NULL, NULL);
			string inputText(content);
			delete[] content;

			MatrixShifr matrixShifr;
			matrixShifr.getInputString(inputText);
			matrixShifr.splitInputString();
			matrixShifr.getPoslInTxt("password.txt");
			matrixShifr.GetPoslInGenerator();
			matrixShifr.GenerateMatrics();
			matrixShifr.fullTransformBlocks();

			if (SendMessage(hCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED)
				matrixShifr.shifrAndDeshifrByOFB();
			else
				matrixShifr._shifr();

			string encryptedText = matrixShifr.getStringResult();

			int outLen = MultiByteToWideChar(CP_ACP, 0, encryptedText.c_str(), -1, NULL, 0);
			wchar_t* outBuffer = new wchar_t[outLen];
			MultiByteToWideChar(CP_ACP, 0, encryptedText.c_str(), -1, outBuffer, outLen);

			SetWindowTextW(hWndShifr, outBuffer);
			delete[] outBuffer;
		}

		if (LOWORD(wParam) == DESHIFR) {
			wchar_t buffer[4096];
			GetWindowText(hWndShifr, buffer, 4096);

			if (buffer[0] == L'\0') {
				MessageBox(hwnd, L"Введите текст для дешифрования", L"Ошибка", MB_OK);
				break;
			}

			int len = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
			char* content = new char[len];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, content, len, NULL, NULL);
			string encryptedText(content);
			delete[] content;

			MatrixShifr matrixShifr;
			matrixShifr.getPoslInTxt("password.txt");
			matrixShifr.GetPoslInGenerator();
			matrixShifr.GenerateMatrics();
			matrixShifr.parseEncryptedString(encryptedText);

			if (SendMessage(hCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED)
				matrixShifr.shifrAndDeshifrByOFB();
			else
				matrixShifr._deshifr();

			string decryptedText;
			for (auto& el : matrixShifr.result) {
				for (int val : el) {
					decryptedText += (char)val;
				}
			}
			while (!decryptedText.empty() && decryptedText.back() == '\0') {
				decryptedText.pop_back();
			}

			int outLen = MultiByteToWideChar(CP_ACP, 0, decryptedText.c_str(), -1, NULL, 0);
			wchar_t* outBuffer = new wchar_t[outLen];
			MultiByteToWideChar(CP_ACP, 0, decryptedText.c_str(), -1, outBuffer, outLen);

			SetWindowTextW(hWndShifr, outBuffer);
			delete[] outBuffer;
		}
		break;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}

	default: {
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
		.hbrBackground = CreateSolidBrush(RGB(51, 94, 42)),
		.lpszMenuName = NULL,
		.lpszClassName = L"WndMain",
		.hIconSm = LoadIconW(NULL, IDI_ASTERISK)
	};

	RegisterClassExW(&wc);
	hwnd = CreateWindowExW(0, wc.lpszClassName, L"Лабораторная работа №4 - Матричное шифрование", WS_OVERLAPPEDWINDOW, 300, 100, 500, 335, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;
}