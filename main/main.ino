#include <Arduino.h>
#include <Keypad.h>

#define ROWS 4
#define COLS 4
#define DTMF_TONES 12
#define WAVE_TABLE_SIZE 200
#define R2R_OFFSET 2
#define PTT 13
#define DELAY A7
#define SENSE A6
#define AUDIO_IN A5

typedef struct DTMF_TONE
{
  uint16_t freq_1;
  uint16_t freq_2;
  char key;
} DTMF_TONE;

const DTMF_TONE dtmf_tones[DTMF_TONES] = {
    {14, 24, '1'},
    {14, 27, '2'},
    {14, 30, '3'},
    {15, 24, '4'},
    {15, 27, '5'},
    {15, 30, '6'},
    {17, 24, '7'},
    {17, 27, '8'},
    {17, 30, '9'},
    {19, 24, '*'},
    {19, 27, '0'},
    {19, 30, '#'},
};

uint8_t waveTable[WAVE_TABLE_SIZE] = {
    64, 66, 68, 69, 71, 73, 75, 77, 79, 80, 82, 84, 86, 88, 89,
    91, 93, 94, 96, 97, 99, 101, 102, 103, 105, 106, 107, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 120, 121, 121, 122, 122, 123,
    123, 123, 123, 124, 124, 124, 124, 124, 123, 123, 123, 123, 122, 122, 121,
    121, 120, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 107,
    106, 105, 103, 102, 101, 99, 97, 96, 94, 93, 91, 89, 88, 86, 84,
    82, 80, 79, 77, 75, 73, 71, 69, 68, 66, 64, 62, 60, 58, 56,
    54, 53, 51, 49, 47, 45, 43, 42, 40, 38, 37, 35, 33, 32, 30,
    28, 27, 26, 24, 23, 21, 20, 19, 18, 16, 15, 14, 13, 12, 11,
    10, 9, 9, 8, 7, 7, 6, 6, 5, 5, 4, 4, 4, 4, 4,
    04, 4, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 9,
    10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 23, 24, 26, 27,
    28, 30, 32, 33, 35, 37, 38, 40, 42, 43, 45, 47, 49, 51, 53,
    54, 56, 58, 60, 62};

const char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};

uint8_t rowPins[ROWS] = {10, 11, 12, A4};
uint8_t colPins[COLS] = {A3, A2, A1, A0}; // TODO: Change some pins to analog pins to make place for R2R ladder

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

uint8_t dtmfSize = 0;
char *dtmf = 0;

uint32_t freq1_pos = 0;
uint32_t freq2_pos = 0;

uint32_t freq1_offset = 0;
uint32_t freq2_offset = 0;

void setup()
{
  dtmf = (char *)malloc(1);
  dtmf[0] = 0;

  for (uint8_t i = 0; i < 8; i++)
  {
    pinMode(i + R2R_OFFSET, OUTPUT);
  }
  pinMode(PTT, OUTPUT);
  pinMode(DELAY, INPUT);
  pinMode(SENSE, INPUT);
  pinMode(AUDIO_IN, INPUT);
}

void loop()
{
  char key = keypad.getKey();
  if (key == 'A')
  {
    setPTT(HIGH);
    delay(1200);
    for (uint8_t i = 0; i < dtmfSize; i++)
    {
      for (uint8_t j = 0; j < DTMF_TONES; j++)
      {
        if (dtmf_tones[j].key == dtmf[i])
        {
          generate_tone(1000, dtmf_tones[j].freq_1, dtmf_tones[j].freq_2);
          delay(400);
          break;
        }
      }
    }
    delay(200);
    setPTT(LOW);
    clear_dtmf();
  }
  else if (key == 'C')
  {
    // Clear DTMF
    clear_dtmf();
  }
  else if (key == 'B')
  {
    // Generate tone to open relay stations
    setPTT(HIGH);
    delay(1200);
    generate_tone(1000, 37, 0); // 1750 Hz tone
    delay(300);
    setPTT(LOW);
  }
  else if (key == 'D')
  {
    // NOTING
  }
  else if (key != 0)
  {
    if (dtmfSize >= 0xfe)
    { // We dont want to risk an overflow of the size byte
      return;
    }
    dtmfSize++;
    dtmf = (char *)realloc(dtmf, dtmfSize + 1);
    dtmf[dtmfSize] = 0;
    dtmf[dtmfSize - 1] = key;
  }

  int audio_in = analogRead(AUDIO_IN);

  if (audio_in >= analogRead(SENSE))
  {
    setPTT(HIGH);

    unsigned long now = millis();
    while (millis() - now < map(analogRead(DELAY), 0, 1024, 10, 2000))
    {
      audio_in = analogRead(AUDIO_IN);
      int sense = analogRead(SENSE);
      if (audio_in >= sense)
      {
        now = millis();
      }
    }

    setPTT(LOW);
  }
}

void setPTT(uint8_t mode)
{
  digitalWrite(PTT, mode);
}

void clear_dtmf()
{
  dtmf = (char *)realloc(dtmf, 1);
  dtmf[0] = 0;
  dtmfSize = 0;
}

void write_value(uint8_t value)
{
  for (int8_t i = 0; i < 8; i++)
  {
    digitalWrite(i + R2R_OFFSET, ((value >> (7 - i)) & 1) ? HIGH : LOW);
  }
}

void generate_tone(uint32_t time_delay, uint32_t freq1, uint32_t freq2)
{
  freq1_pos = 0;
  freq2_pos = 0;
  freq1_offset = freq1;
  freq2_offset = freq2;

  // Enable Timer
  noInterrupts();

  // Initialize TIMER1 registers to known state.
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIMSK1 = 0;

  // Set TIMER1 to 10KHz with reference to 16MHz Arduino clock.
  OCR1A = 24;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10);

  // Enable TIMER1 interrupt.
  TIMSK1 |= (1 << OCIE1A);
  interrupts();

  delay(time_delay);

  // Disable Timer
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIMSK1 = 0;
  interrupts();
}

// Timer Interrupt
ISR(TIMER1_COMPA_vect)
{
  uint8_t value = waveTable[freq1_pos];

  if (freq2_offset > 0)
    value += waveTable[freq2_pos];

  write_value(value);

  freq1_pos += freq1_offset;
  freq2_pos += freq2_offset;

  if (freq1_pos >= WAVE_TABLE_SIZE)
  {
    freq1_pos -= WAVE_TABLE_SIZE;
  }

  if (freq2_pos >= WAVE_TABLE_SIZE)
  {
    freq2_pos -= WAVE_TABLE_SIZE;
  }
}
