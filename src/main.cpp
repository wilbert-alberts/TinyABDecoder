#include <avr/interrupt.h>
#include <quaddecoder.h>


static uint8_t a;
static uint8_t b;
static bool comInProgress;
static int16_t position;
static int16_t bufferedPosition;


void setupEncIO()
{
  // DDR: 1 is output, 0 is input

  // PB3 is input A
  // PB4 is input B
  DDRB &= ~(_BV(DDB3) | _BV(DDB4)); // enc A en B
  a = (PINB & _BV(PINB3)) >> PINB3;
  b = (PINB & _BV(PINB4)) >> PINB4;
}

void setupSpiIO()
{
  // DDR: 1 is output, 0 is input

  // Setup SPI slave
  DDRB &= ~(_BV(DDB0) | _BV(DDB2)); // clock and data in
  DDRB |= _BV(DDB1);                //  data out
}

void setupEncIRQ()
{
  // Select pin3 and pin4 to act on pin changes
  PCMSK = _BV(PCINT3) | _BV(PCINT4);

  // Enable pin change interrupt
  GIMSK |= _BV(PCIE);
}


ISR(PCINT0_vect)
{
  uint8_t aNew = (PINB & _BV(PINB3)) >> PINB3;
  uint8_t bNew = (PINB & _BV(PINB4)) >> PINB4;

  decodeAB(aNew, bNew, &a, &b, &position);
 
  // Copy position to buffer if no com in progress
  if (!comInProgress)
    bufferedPosition = position;
}

bool slaveSelected()
{
  return (PINB & _BV(PINB5)) >> PINB5;
}

void enableSpiSlave()
{
  comInProgress = true;
  USICR |= _BV(USIWM0);
  USICR &= ~_BV(USIWM1);
}

void disableSpiSlave()
{
  USICR &= ~(_BV(USIWM1) | _BV(USIWM0));
  comInProgress = false;
}

void comm()
{
  while (!slaveSelected())
    ;
  enableSpiSlave();
  // fill data register with low byte first;
  USIDR = (uint8_t)bufferedPosition;

  // Wait until byte has been sent
  while ((USISR & _BV(USIOIF)) == 0)
    ;
  // One byte sent. Send second one
  USIDR = (uint8_t)(bufferedPosition >> 8);

  // Clear counter
  USISR |= _BV(USIOIF);

  while ((USISR & _BV(USIOIF)) == 0)
    ;
  // Clear counter
  USISR |= _BV(USIOIF);

  disableSpiSlave();
  while (slaveSelected())
    ;
}

int main()
{
  cli();
  setupEncIO();
  setupSpiIO();
  setupEncIRQ();
  sei();
  while (1)
  {
    comm();
  }
}