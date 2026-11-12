#include <REGX51.H>

sbit LED = P1^3;
volatile unsigned int totalTicks = 0;       // 3600 шагов по 50 мс = 180 с
volatile unsigned char phaseTicks = 40;     // 40 шагов по 50 мс = 2 с включения
volatile bit finished = 0;

void timer0_isr(void) interrupt 1 using 1
{
    TH0 = 0x3C;                              // Перезагрузка таймера: 3CB0h
    TL0 = 0xB0;

    if (totalTicks < 3600) totalTicks++;
    if (totalTicks >= 3600) {                // Завершить работу через 180 с
        TR0 = 0;
        LED = 1;                             // Светодиод выключен (активный ноль)
        finished = 1;
        return;
    }

    if (phaseTicks > 0) phaseTicks--;
    if (phaseTicks == 0) {
        LED = !LED;                          // Переключить светодиод
        phaseTicks = (LED == 0) ? 40 : 20;  // 2 с включён / 1 с выключен
    }
}

void main(void)
{
    LED = 0;                                 // Начать с включённого светодиода
    TMOD = (TMOD & 0xF0) | 0x01;            // Таймер 0, режим 1 (16 бит)
    TH0 = 0x3C;
    TL0 = 0xB0;
    TF0 = 0;
    ET0 = 1;                                // Разрешить прерывание таймера 0
    EA = 1;                                 // Глобальное разрешение прерываний
    TR0 = 1;                                // Запустить таймер 0

    while (!finished) { }                   // Ожидать окончания трёх минут
    while (1) { }                           // Оставить устройство в покое
}
