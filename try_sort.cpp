/*
Разделим файл на блоки, которые помещаются в доступную память. Обозначим эти блоки Block_1, Block_2, …, Block_(S-1), Block_S. Установим P = 1.
Читаем Block_P в память.

Отсортируем данные в памяти и запишем назад в Block_P. Установим P = P + 1, и если P ≤ S, то читаем Block_P в память и повторяем этот шаг. 
Другими словами, отсортируем каждый блок файла.

Разделим каждый блок на меньшие блоки B_1 и B_2. Каждый из таких блоков занимает половину доступной памяти.
Читаем блок B_1 блока Block_1 в первую половину доступной памяти. Установим Q = 2.
Читаем блок B_1 блока Block_Q во вторую половину доступной памяти.

Объеденим массивы в памяти и отсортируем, запишем вторую половину памяти в блок B_1 блока Block_Q и установим Q = Q + 1, если Q ≤ S, 
читаем блок B_1 блока Block_Q во вторую половину доступной памяти и повторяем этот шаг.

Записываем первую половину доступной памяти в блок B_1 блока Block_1. Так как мы всегда оставляли в памяти меньшую половину элементов и 
провели слияние со всеми блоками, то в этой части памяти хранятся M минимальных элементы всего файла.

Читаем блок B_2 блока Block_S во вторую половину доступной памяти. Установим Q = S −1.
Читаем блок B_2 блока Block_Q в первую половину доступной памяти.

Объеденим массивы в памяти и отсортируем, запишем первую половину доступной памяти в блок B_2 блока Block_Q и установим Q = Q −1. 
Если Q ≥ 1 читаем блок B_2 блока Block_Q в первую половину доступной памяти и повторяем этот шаг.

Записываем вторую половину доступной памяти в блок B_2 блока Block_S. Аналогично, тут хранятся максимальные элементы всего файла.

Начиная от блока B_2 блока Block_1 и до блока B_1 блока Block_S, определим новые блоки в файле и снова пронумеруем их Block_1 to Block_S.
Разделим каждый блок на блоки B_1 и B_2. Установим P = 1.

Заново повторяем весь алгоритм но уже для последовательности расположенной после элемента с номером равным размеру B_1 до элемента
последовательности по номеру (номер последнего элемента последовательности - размер B_1)

Таким образом после каждого прохода последовательность для сортировки будет уменьшаться пока не станет равной размеру доступной памяти
*/

#pragma warning(disable : 4996)

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>

using std::cout;
using std::cin;
using std::endl;
using std::thread;
using std::sort;


uint64_t file_size(const char* filename);

void find_left(uint64_t* mas, FILE* f, const uint64_t s, const uint64_t count_blocks,
	const uint64_t half_block_size, const uint64_t half_block, const uint64_t block_len,
	std::mutex& m, bool& flag, std::condition_variable& cv);

void find_right(uint64_t* mas, FILE* f, const uint64_t s, const uint64_t count_blocks,
	const uint64_t half_block_size, const uint64_t half_block, const uint64_t block_len,
	std::mutex& m, bool& flag, std::condition_variable& cv);

class File_Controller
{
private:
	const char* file_name;
	const char* mode;

public:
	FILE* f;

	File_Controller(const char* name, const char* mode_) : file_name(name), mode(mode_)
	{
		f = fopen(file_name, mode);
	}

	~File_Controller()
	{
		fclose(f);
	}
};

int main()
{
	const uint64_t nums_count_f_in = file_size("input.bin") / sizeof(uint64_t);
	const uint64_t block_len = 500000;
	const uint64_t half_block = 250000;
	const uint64_t block_size = block_len * sizeof(uint64_t);
	const uint64_t half_block_size = half_block * sizeof(uint64_t);
	const uint64_t count_blocks = nums_count_f_in / block_len + 1;

	uint64_t * mas_1 = new uint64_t[block_len];
	uint64_t * mas_2 = new uint64_t[block_len];
	uint64_t delta = nums_count_f_in % block_len;

	File_Controller f_in("input.bin", "rb");
	File_Controller f_temp("temp.bin", "r+b");
	File_Controller f_out("output.bin", "wb");

	for (uint64_t i = 0; i < count_blocks; i++)
	{
		fread(mas_1, sizeof(uint64_t), block_len, f_in.f);
		if (i == count_blocks - 1 && delta)
		{
			for (uint64_t j = delta; j < block_len; j++)
				mas_1[j] = -1;
		}
		sort(mas_1, mas_1 + block_len - 1);
		fwrite(mas_1, sizeof(uint64_t), block_len, f_temp.f);
	}

	std::mutex mx;
	bool flag = true;
	std::condition_variable cv;
	uint64_t s = count_blocks;
	while (s > 1)
	{
		thread m1(find_left, mas_1, f_temp.f, s, count_blocks, half_block_size, half_block, 
			block_len, std::ref(mx), std::ref(flag), std::ref(cv));
		thread m2(find_right, mas_2, f_temp.f, s, count_blocks, half_block_size, half_block,
			block_len, std::ref(mx), std::ref(flag), std::ref(cv));

		m1.join();
		m2.join();

		s--;
		fseek(f_temp.f, (count_blocks - s) * half_block_size, SEEK_SET);
		for (uint64_t i = 0; i < s; i++)
		{
			fseek(f_temp.f, (count_blocks - s) * half_block_size + i * block_size, SEEK_SET);
			fread(mas_1, sizeof(uint64_t), block_len, f_temp.f);
			sort(mas_1, mas_1 + block_len - 1);
			fseek(f_temp.f, (count_blocks - s) * half_block_size + i * block_size, SEEK_SET);
			fwrite(mas_1, sizeof(uint64_t), block_len, f_temp.f);
		}
	}
	fseek(f_temp.f, 0, SEEK_SET);
	for (uint64_t i = 0; i < count_blocks; i++)
	{
		fread(mas_1, sizeof(uint64_t), block_len, f_temp.f);
		if (i == count_blocks - 1 && delta)
		{
			fwrite(mas_1, sizeof(uint64_t), delta, f_out.f);
		}
		else
		{
			fwrite(mas_1, sizeof(uint64_t), block_len, f_out.f);
		}
	}
	delete[] mas_1;
	delete[] mas_2;

	return 0;
}

uint64_t file_size(const char* filename)
{
	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	uint64_t size = (int)ftell(f);
	fclose(f);
	return size;
}

void find_left(uint64_t* mas, FILE* f, const uint64_t s, const uint64_t count_blocks,
	const uint64_t half_block_size, const uint64_t half_block, const uint64_t block_len,
	std::mutex& m, bool& flag, std::condition_variable& cv)
{
	std::unique_lock<std::mutex> locker(m);
	while (!flag)
		cv.wait(locker);

	fseek(f, (count_blocks - s) * half_block_size, SEEK_SET);
	fread(mas, sizeof(uint64_t), half_block, f);

	flag = false;
	cv.notify_one();

	for (uint64_t i = 1; i < s; i++)
	{
		while (!flag)
			cv.wait(locker);

		fseek(f, (count_blocks - s + 2 * i) * half_block_size, SEEK_SET);
		fread(mas + half_block, sizeof(uint64_t), half_block, f);
		sort(mas, mas + block_len - 1);
		fseek(f, (count_blocks - s + 2 * i) * half_block_size, SEEK_SET);
		fwrite(mas + half_block, sizeof(uint64_t), half_block, f);

		flag = false;
		cv.notify_one();
	}
	while (!flag)
		cv.wait(locker);

	fseek(f, (count_blocks - s) * half_block_size, SEEK_SET);
	fwrite(mas, sizeof(uint64_t), half_block, f);

	flag = false;
	cv.notify_one();
}

void find_right(uint64_t* mas, FILE* f, const uint64_t s, const uint64_t count_blocks,
	const uint64_t half_block_size, const uint64_t half_block, const uint64_t block_len,
	std::mutex& m, bool& flag, std::condition_variable& cv)
{
	std::unique_lock<std::mutex> locker(m);
	while (flag)
		cv.wait(locker);

	fseek(f, (-1) * (count_blocks - s + 1) * half_block_size, SEEK_END);
	fread(mas + half_block, sizeof(uint64_t), half_block, f);

	flag = true;
	cv.notify_one();

	for (uint64_t i = 1; i < s; i++)
	{
		while (flag)
			cv.wait(locker);

		fseek(f, (-1) * (count_blocks - s + 1 + 2 * i) * half_block_size, SEEK_END);
		fread(mas, sizeof(uint64_t), half_block, f);
		sort(mas, mas + block_len - 1);
		fseek(f, (-1) * (count_blocks - s + 1 + 2 * i) * half_block_size, SEEK_END);
		fwrite(mas, sizeof(uint64_t), half_block, f);

		flag = true;
		cv.notify_one();
	}
	while (flag)
		cv.wait(locker);

	fseek(f, (-1) * (count_blocks - s + 1) * half_block_size, SEEK_END);
	fwrite(mas + half_block, sizeof(uint64_t), half_block, f);

	flag = true;
	cv.notify_one();
}
