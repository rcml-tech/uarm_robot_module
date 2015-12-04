#include "SerialClass.h"

Serial::Serial(LPCWSTR portName)
{
	// пока ещё не подключились
	this->connected = false;

	// пытаемся подключиться к порту посредством CreateFile
	printf("%s\n", (LPCWSTR)(portName));
	this->hSerial = CreateFile((LPCWSTR) (portName),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// проверяем – было ли соединение успешным
	if (this->hSerial == INVALID_HANDLE_VALUE)
	{
		//если не было – выводим ошибку
		if (GetLastError() == ERROR_FILE_NOT_FOUND){

			// печатаем ошибку
			printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else
	{
		// подключились – теперь устанавливаем параметры
		DCB dcbSerialParams = { 0 };

		// считываем текущие параметры
		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			// если невозможно – говорим о неудаче
			printf("failed to get current serial parameters!");
		}
		else
		{
			// устанавливаем параметры для соединения с Arduino
			dcbSerialParams.BaudRate = CBR_9600;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;

			// применяем параметры
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				printf("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				// отлично! Мы успешно подлючились :)
				this->connected = true;
				//ждём пока Arduino перезагрузится
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}

}

Serial::~Serial()
{
	// проверяем статус подключение
	if (this->connected)
	{
		// отключаемся
		this->connected = false;
		// закрываем дескриптор порта
		CloseHandle(this->hSerial);
	}
}

int Serial::ReadData(char *buffer, unsigned int nbChar)
{
	// число считываемых байт
	DWORD bytesRead;
	// число байт, которое мы действительно хотим считать
	unsigned int toRead;

	// используем ClearCommError для получения информации о статусе последовательного порта
	ClearCommError(this->hSerial, &this->errors, &this->status);

	// проверяем – есть ли информация для считывания
	if (this->status.cbInQue>0)
	{
		// если количество данных меньше требуемого – считываем
		// столько, сколько есть
		if (this->status.cbInQue>nbChar)
		{
			toRead = nbChar;
		}
		else
		{
			toRead = this->status.cbInQue;
		}

		// Пытаемся считать нужное количество данных и 
		//возвращаем число считанных байт при удачном завершении.
		if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) && bytesRead != 0)
		{
			return bytesRead;
		}

	}

	// ничего не считали или где-то была ошибка – возвращаем -1
	return -1;

}


bool Serial::WriteData(char *buffer, unsigned int nbChar)
{
	DWORD bytesSend;

	// пытаемся записать значение буфера buffer в COM-порт
	if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
	{
		// если не получилось – получаем код ошибки и возвращаем false
		ClearCommError(this->hSerial, &this->errors, &this->status);

		return false;
	}
	else
		return true;
}

bool Serial::IsConnected()
{
	// просто возвращаем статус соединения :)
	return this->connected;
}