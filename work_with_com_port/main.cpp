#include <iostream>
#include <windows.h>
#include "SerialClass.h"

#define COM_PORT L"COM7"

#define BUF_SIZE 256

int main()
{
	Serial com7(COM_PORT);
	// обнуляем буфер
	char buf[BUF_SIZE];
	for (int i = 0; i<BUF_SIZE; i++)
		buf[i] = '\0';
	// считываем и выводим данные из порта
	char str[9];
	unsigned char *message;
	
	char start_byte_1 = 0xAA;
	char start_byte_2 = 0xFF;
	char rotation_byte_1 = 0x0;
	char rotation_byte_2 = 0xA;
	char stretch_byte_1 = 0x0;
	char stretch_byte_2 = 0x0;
	char heigth_byte_1 = 0x0;
	char heigth_byte_2 = 0x0;
	char hand_rotation_byte_1 = 0x0;
	char hand_rotation_byte_2 = 0x0;
	char grip_byte = 0x0;
	//char grip_byte_2 = 0x0;

	str[0] = rotation_byte_1;
	str[1] = rotation_byte_2;
	str[2] = stretch_byte_1;
	str[3] = stretch_byte_2;
	str[4] = heigth_byte_1;
	str[5] = heigth_byte_2;
	str[6] = hand_rotation_byte_1;
	str[7] = hand_rotation_byte_2;
	str[8] = grip_byte;

	char start_bytes[2];
	start_bytes[0] = start_byte_2;
	start_bytes[1] = start_byte_1;
	com7.WriteData(start_bytes,sizeof(unsigned char)*2);
	com7.WriteData(str,sizeof(str));
	Sleep(200);
	com7.ReadData(buf, BUF_SIZE - 1);
	std::cout << buf << std::endl;
	Sleep(1000);
	
	str[8] = 0x0;
	str[1] = 0x0;
	com7.WriteData(str,sizeof(str));
	Sleep(200);
	com7.ReadData(buf, BUF_SIZE - 1);
	std::cout << buf << std::endl;

	return 0;
}