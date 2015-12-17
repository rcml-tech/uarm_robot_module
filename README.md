# Модуль RCML для uArm uFactory
-------------------------------
Данный модуль позволяет программировать **uArm** роботов на RCML. Программа на RCML исполняется на ПК, связь с uArm осуществляется через интерфейс **USB**.
Модуль имеет конфигурационный файл, в котором указывается количество используемых uArm роботов и COM-порт для каждого робота, осуществляющий коммуникацию. Так же в конфигурационном файле можно указать должен-ли модуль запускать роботов в режиме отладки.<br> Если значение **is_debug** установлено равным 1, то роботы будт запускаться в режиме отладки.<br>

*Например мы используем одного робота uArm, который подключен к порту COM3 и мы хотим запустить его в режиме отладки, тогда config.ini будет иметь следующее содержимое:*
```ini
    [main]
	count_robots = 1
	is_debug = 0
	robot_port_0 = com3
```
*Стоит обратить внимание, что config.ini должен находиться в одной директории с файлом модуля.*

*При превышении указанных ограничений вызываемая функция бросает исключение, которое можно обработать с помошью конструкции языка RCML try/catch.*

####Доступные функции робота:
Определение  | Описание | Пример
move_to(**X**, **Y**, **Z**)  | перемещает захват робота в точку с координатами  **X**, **Y**, **Z** |  @Robot->move_to (10, 10, 10); перемещает захват робота в точку с координатами X=10, Y=10, Z=10
pump_on() | включает помпу захвата | @Robot->pump_on(); включает помпу захвата
pump_off() | вкылючает помпу захвата | @Robot->pump_off();  выключает помпу захвата
find_x() | возвращает значение координаты X точки, в которой находится захват робота | X = @Robot->find_x(); получает значение координаты X точки, в которой находится захват робота и сохраняет ее в переменную X
find_y() | возвращает значение координаты X точки, в которой находится захват робота | Y = @Robot->find_y(); получает значение координаты Y точки, в которой находится захват робота и сохраняет ее в переменную Y
find_z() | возвращает значение координаты X точки, в которой находится захват робота | z = @Robot->find_z(); получает значение координаты Z точки, в которой находится захват робота и сохраняет ее в переменную Z
move_to_at_once(**x**, **Y**, **Z**) |  перемещает захват робота в точку с координатами  **X**, **Y**, **Z** |  @Robot->move_to_at_once (10, 10, 10); перемещает захват робота в точку с координатами X=10, Y=10, Z=10.
move(**x**, **Y**, **Z**) | перемещает захват робота из точки с текущими координатами **X_текущее**, **Y_текущее**, **Z_текущее** в точку с координатами **X_текущее + X**, **Y_текущее + Y**, **Z_текущее + Z** |  @Robot->move(1, 1, 1); изменит координаты точки в которой находится захват робота на указанные значения (X_текущее + 1, Y_текущее + 1, Z_текущее + 1)
move_to_1(**x**, **Y**, **Z**, **T**) | перемещает захват робота в точку с координатами  **X**, **Y**, **Z** с задержками между промежуточными шагами на время **T**. Время указывается в миллисекундах | @Robot->move_to_1(1, 1, 1, 50); переместит захват робота в точку с координатами X=1, Y=1, Z=1, с 50 миллисекундной задержкой между промежуточными перемещениями.
move_to_2(**x**, **Y**, **Z**, **T**, **R**, **Th4**) | перемещает захват робота в точку с координатами  **X**, **Y**, **Z** с задержками между промежуточными шагами на время **T**. Так же поворачивает захват роботоа вокруг оси на **Th4** градусов. | @Robot->move_to_2(1, 1, 1, 1, 1, 30); переместит захват робота в точку с координатами X=1, Y=1, Z=1, с 50 миллисекундной задержкой между промежуточными перемещениями. Так же повернет захват робота вокруг оси на 30 градусов.
write_angles(**Th1**, **Th2**, **Th3**, **Th4**) | изменяет углы положения сервоприводов на указанные значения **Th1**, **Th2**, **Th3**, **Th4** | @Robot->write_angles(10, 10, 10, 10); изменит углы положения сервоприводов на 10 градусов
rotate_servo4(**Th4**) | изменит угол положения сервопривода захвата робота на указанное значение **Th4** | @Robot->rotate_servo4(15); изменит угол положения сервопривода захвата робота на 15 градусов.

####Оси робота:

Ось| Верхняя граница | Нижняя граница | Описание 
------------  | :-----------------: | :--------------: | ---------------
locked | 1 | 0 | Бинарная ось. Если её значение равно 1, то робот считается заблокированным и игнорирует изменение значений по любым другим осям.
pump | 1 | 0 | Бинарная ось. Если её значение равно 1, то будет включена помпа робота. При значении равном 0 помпа будет отключена.
servo1 | 180 | 0 | ось изменения угла первого сервопривода.
servo2 | 180 | 0 | ось изменения угла второго сервопривода.
servo3 | 180 | 0 | ось изменения угла третьего сервопривода.
servo4 | 180 | 0 | ось изменения угла четвертого сервопривода.

####Компиляция
Для компиляции модуля потребуются библиотеки:
- [SimpleIni](https://github.com/brofield/simpleini)
- [uArm C# SDK, Arduino: Firmata-EEPROM ](http://developer.ufactory.cc/quickstart/csharp/)
- [uArm C# SDK] (https://github.com/uArm-Developer/UArmForCSharp/releases)
- [Arduino: Firmata-EEPROM] (https://github.com/uArm-Developer/FirmataEEPROM)

