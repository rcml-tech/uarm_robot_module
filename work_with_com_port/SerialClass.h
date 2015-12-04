#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 2000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class Serial
{
private:
	// дескриптор COM-порта
	HANDLE hSerial;
	// статус соединения
	bool connected;
	// различная информация о соединении
	COMSTAT status;
	// последняя ошибка
	DWORD errors;

public:
	// инициализация с указанным COM-портом
	Serial(LPCWSTR portName);
	// закрытие соединения
	//NOTA: по каким-то причинам не удаётся переподключиться
	// нужно перезапускать программу
	~Serial();
	// считываем данные в буфер – если nbChar больше, чем
	// число доступных байт – возвращает только 
	// доступные данные
	// возвращает -1 при ошибке
	int ReadData(char *buffer, unsigned int nbChar);
	// записывает данные из буфера в порт
	// возвращает true при удаче
	bool WriteData(char *buffer, unsigned int nbChar);
	// возвращает статус соединения
	bool IsConnected();


};

#endif // SERIALCLASS_H_INCLUDED