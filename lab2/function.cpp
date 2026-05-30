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

extern HWND hWndResult;

//------------------------------------
// 1 лабораторная
//------------------------------------ 
// Частотный тест
extern bool frequency_test(const wchar_t* content, int len)
{
	if (len <= 1) return false; // выходим если длина последовательности < 1

	int current_len = len - 1; // удаляем псоледний элемент - \0

	std::vector<int> arr(current_len); // определяем список бит

	// Переводим биты последовательности по формуле xi = 2 * ei - 1
	for (int i = 0; i < current_len; i++)
	{
		if (content[i] == L'0')
			arr[i] = -1;
		else if (content[i] == L'1')
			arr[i] = 1;
		else
			return false;
	}

	int sum_posl = std::accumulate(arr.begin(), arr.end(), 0); // подсчитываем сумму последовательности
	double s = std::fabs(sum_posl) / std::sqrt(current_len); // делим сумму на квадратный корень из кол-ва элементов

	std::wstring wstr = L"S = " + std::to_wstring(s);
	SetWindowTextW(hWndResult, wstr.c_str()); // выводим результат в поле Edit приложения

	return s <= 1.82138636; // возвращаем true или false  в зависимости от подсчитанной статистики
}

// Тест на последовательность одинаковых бит
extern bool posl_equal_bit(wchar_t* content, int len)
{
	if (len <= 1) return false; // выходим если длина последовательности <= 1

	int current_len = len - 1; // удаляем последний элемент - \0
	std::vector<int>arr(current_len); // определяем список бит последовательности

	int sum = 0;
	for (size_t i = 0; i < current_len; i++)
	{
		if (content[i] == L'0')				// конвертируем нашу последоватеьность	
			arr[i] = 0;						// параллельно считая сумму элементов
		else if (content[i] == L'1')		// которая понадобиться при рассчете частоты
		{
			arr[i] = 1;
			sum++;
		}
		else
			return false; // выходим, если вдруг в последовательности попадется непонятный символ
	}

	double pi = static_cast<double>(sum) / current_len; // рассчет частоты с которой единицы встречаются в последовательнсти

	int v = 1; // рассчет количества переходов или границ между цепочками
	for (int i = 0; i < current_len - 1; i++)
	{
		if (content[i] != content[i + 1]) {
			v++;
		}
	}

	//рассчет статистики по формуле
	double s_1 = fabs(v - 2 * current_len * pi * (1 - pi));
	double s_2 = 2 * std::sqrt(2.0 * current_len) * pi * (1 - pi);

	if (s_2 == 0) return false; // если вдруг знаменатель обернется в 0, то выходим

	double s = s_1 / s_2;

	std::wstring wstr = L"S = " + std::to_wstring(s);
	SetWindowTextW(hWndResult, wstr.c_str()); // выводим статистику в поле Edit

	return s <= 1.82138636; // возвращаем true или false в зависимости от результата подсчитанной статистики

}

// Расширенный тест на произвольное отклонение
extern bool deviations(wchar_t* content, int len)
{
	if (len <= 1) return false; // выходим если длина последовательности слишком мала

	int current_len = len - 1; // удаляем \0

	std::vector<int> arr(current_len); // создаем вектор числовых значений

	// конверитуем значения по формуле xi = 2 * ei - 1
	for (int i = 0; i < current_len; i++)
	{
		if (content[i] == L'0')
			arr[i] = -1;
		else if (content[i] == L'1')
			arr[i] = 1;
		else
			return false; // выходим если не соответствующий символ
	}

	std::vector<int> sums(current_len); // создаем список сумм
	sums[0] = arr[0]; // первая сумма равна первому элементу
	for (int i = 1; i < current_len; i++)
	{
		sums[i] = sums[i - 1] + arr[i]; // в соответсвии с алгоритмом прибавляем к текущей сумме следующий элемент
	}

	std::vector<int> new_posl(current_len + 2); // создаем новую последовательность на 2 больше, так как прибавится еще 2 нуля
	new_posl[0] = 0; // первый элемент нулевой и последний тоже
	new_posl[current_len + 1] = 0;

	for (int i = 1; i < current_len + 1; i++)
	{
		new_posl[i] = sums[i - 1];
	}

	int L = -1; // количество 0 в полученной последовательности
	for (int el : new_posl)
		if (el == 0) L++;

	if (L == 0) return false;

	// вычисление статистик

	std::map<int, int> Ej;
	for (int val : new_posl)
	{
		if (val >= -9 && val <= 9 && val != 0)
			Ej[val]++;  // подсчет количества встретившихся состояний
	}

	std::map<int, double> Yj; // статистики
	for (int i = -9; i < 10; i++)
	{
		if (i == 0) continue; // исключаем 0

		Yj[i] = fabs(Ej[i] - L) / (std::sqrt(2.0 * L * (4 * abs(i) - 2))); // рассчет статистик
	}
	std::wstring wstr;
	for (int i = -9; i < 10; i++)
	{
		if (i == 0) continue;
		wstr += L"Y[" + std::to_wstring(i) + L"]\t" + std::to_wstring(Yj[i]) + L"\r\n"; // выводим все статистики в Edit
	}
	SetWindowTextW(hWndResult, wstr.c_str());

	for (int i = -9; i < 10; i++)
	{
		if (i == 0) continue;
		if (Yj[i] <= 1.82138636) continue;
		else return false; // если хоть одна статистика <=, тогда выходим и делаем вывод о том что последовательность не случайная
	}
	return true;
}

//------------------------------------
// 2 лабораторная
//------------------------------------
// Генератор Парка-Миллера
extern std::string parka_millera(__int64 number, __int64 seed)
{
	const __int64 a = 16'807;
	const __int64 m = 2'147'483'647;

	std::string res = "";

	for (__int32 i = 0; i < number; i++)
	{
		seed = a * (seed % (m / a)) - ((m % a) * (seed / (m / a)));
		if (seed < 0) seed += m;
		(seed % 2) ? res += "1" : res += "0";
	}

	return res;
}

// Генератор Стоп-Пошёл
static __int32 LFSR(__int32 x0, __int32 x1, __int32 x2, __int32 x3, __int32 x4, __int64& ShiftRegister)
{
	ShiftRegister = ((((ShiftRegister >> x0)
		^ (ShiftRegister >> x1) ^ (ShiftRegister >> x2) ^ (ShiftRegister >> x3) ^ (ShiftRegister >> x4)
		^ ShiftRegister) & 0x00000001) << 31) | (ShiftRegister >> 1);

	return ShiftRegister & 0x00000001;
}
extern std::string stop_and_go(__int64 number, __int64 seed)
{
	__int32 lfsr1, lfsr2, lfsr3, prev_lfsr1;
	__int64 state1, state2, state3;
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


