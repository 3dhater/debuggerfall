# debuggerfall
My own dagerfall

Map: 1000x1000 km

Допустим, информация о ячейках будет лимитированной. Значит можно будет сделать один файл на все 100млн ячеек.
Информация такая:
	позиция
	индексы ячеек соседей
	
Увеличить размер квада - до 2.5 метров.
Размер ячейки в квадах - 100x100 квадов.
Размер ячейки в метрах - 2.5 * 100 = 250 квадратных метров
В ширину\длинну на 1 кв. км. приходится 4 ячейки.
На 1 квадратный км приходится 4x4 = 16 ячеек.
На протяжении карты во всю длинну\ширину нужно 1000км * 4яч = 4000 ячеек
Всего нужно 4000 * 4000 = 16'000'000 ячеек.

