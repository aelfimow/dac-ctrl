#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "data_types.h"

#define DAC_LE  _BV(PD7)
#define EPROM_CE    _BV(PE1)
#define EPROM_OE    _BV(PE2)

#define ADDR_LSB_MASK   (0x000000FFUL)

void SetupIO(void);
void SetupAddr(INT32U addr);
void sync(void);

int main(void)
{
    INT32U addr = 0UL;

    SetupIO();

    sei();

    while (TRUE)
    {
        for (addr = 0UL; addr < 131072UL; ++addr)
        {
            SetupAddr(addr);
            PORTE &= (INT8U)~EPROM_CE;
            sync();
            PORTE &= (INT8U)~EPROM_OE;
            sync();
            PORTD &= (INT8U)~DAC_LE;
            sync();
            PORTD |= (INT8U)DAC_LE;
            sync();
            PORTE |= (INT8U)EPROM_OE;
            sync();
            PORTE |= (INT8U)EPROM_CE;
            sync();
        }
    }

    return 0;
}

void SetupIO(void)
{
    DDRC = 0xFFu; /* as output */
    DDRA = 0xFFu; /* as output */
    DDRE |= (INT8U)_BV(PE0); /* as output */
    DDRE |= (INT8U)EPROM_CE; /* as output */
    DDRE |= (INT8U)EPROM_OE; /* as output */
    DDRD |= (INT8U)DAC_LE; /* as output */

    PORTC = 0;
    PORTA = 0;
    PORTD |= (INT8U)DAC_LE;
    PORTE |= (INT8U)EPROM_OE;
    PORTE |= (INT8U)EPROM_CE;

    return;
}

void SetupAddr(INT32U addr)
{
    INT8U a = (INT8U)(addr & ADDR_LSB_MASK);
    INT8U b = (INT8U)((addr / 256UL) & ADDR_LSB_MASK);
    INT8U c = (INT8U)((addr / 65536UL) & ADDR_LSB_MASK);

    PORTC = a;
    sync();
    PORTA = b;
    sync();

    if (0 != (c & 0x01u))
    {
        PORTE |= (INT8U)0x01u;
        sync();
    }
    else
    {
        PORTE &= (INT8U)~0x01u;
        sync();
    }

    return;
}

void sync(void)
{
    INT8U i = 0u;

    for (i = 0u; i < 43u; ++i)
    {
        asm("nop");
    }

    return;
}

