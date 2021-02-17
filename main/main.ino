#include <Keypad.h>

#define ROWS 4
#define COLS 4
#define DTMF_TONES 12

const char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

typedef struct DTMF_TONE {
  uint16_t freq_1;
  uint16_t freq_2;
  char key;
} DTMF_TONE;

const DTMF_TONE dtmf_tones[DTMF_TONES] = {
  {697, 1209, '1'},
  {697, 1336, '2'},
  {697, 1477, '3'},
  {770, 1209, '4'},
  {770, 1336, '5'},
  {770, 1477, '6'},
  {852, 1209, '7'},
  {852, 1336, '8'},
  {852, 1477, '9'},
  {941, 1209, '*'},
  {941, 1336, '0'},
  {941, 1477, '#'},
};

#define WAVE_TABLE_SIZE 200

unsigned char waveTable[WAVE_TABLE_SIZE] = { 
    64,  66,  68,  69,  71,  73,  75,  77,  79,  80,  82,  84,  86,  88,  89, 
    91,  93,  94,  96,  97,  99, 101, 102, 103, 105, 106, 107, 109, 110, 111, 
    112, 113, 114, 115, 116, 117, 118, 119, 120, 120, 121, 121, 122, 122, 123, 
    123, 123, 123, 124, 124, 124, 124, 124, 123, 123, 123, 123, 122, 122, 121, 
    121, 120, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 107, 
    106, 105, 103, 102, 101,  99,  97,  96,  94,  93,  91,  89,  88,  86,  84, 
    82,  80,  79,  77,  75,  73,  71,  69,  68,  66,  64,  62,  60,  58,  56, 
    54,  53,  51,  49,  47,  45,  43,  42,  40,  38,  37,  35,  33,  32,  30, 
    28,  27,  26,  24,  23,  21,  20,  19,  18,  16,  15,  14,  13,  12,  11, 
    10,   9,   9,   8,   7,   7,   6,   6,   5,   5,   4,   4,   4,   4,   4, 
    04,   4,   4,   4,   4,   4,   5,   5,   6,   6,   7,   7,   8,   9,   9, 
    10,  11,  12,  13,  14,  15,  16,  18,  19,  20,  21,  23,  24,  26,  27, 
    28,  30,  32,  33,  35,  37,  38,  40,  42,  43,  45,  47,  49,  51,  53, 
    54, 56, 58, 60, 62 };

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};  // TODO: Change some pins to analog pins to make place for R2R ladder

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

byte dtmfSize = 0;
char* dtmf = 0;

byte freq1_pos = 0;
byte freq2_pos = 0;

byte freq1_offset = 0;
byte freq2_offset = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(sizeof(byte));
  dtmf = (char*)malloc(1);
  dtmf[0] = 0;

  for(uint8_t i = 0; i < 8; i++){
    pinMode(i + 2, OUTPUT);
  }
  generate_tone(1000, 100, 200);

}

void loop() {
  return;
  char key = keypad.waitForKey();
  Serial.println(key);
  if(key == 'A') {
    Serial.println(dtmf);
    for(byte i = 0; i < dtmfSize; i++){
      // TODO: Play dtmf tone
      for(byte j = 0; j < DTMF_TONES; j++){
        if(dtmf_tones[j].key == dtmf[i]){
          generate_tone(1000, dtmf_tones[j].freq_1, dtmf_tones[j].freq_2);
          break;
        }
      }
    }
    // TODO: Play dtmf tone
    clear_dtmf();
  }else if(key == 'C'){
    // Clear DTMF
    clear_dtmf();
  }else if(key == 'B' || key == 'D'){
    // Don't do anything just for catching else statement
  }else {
    if(dtmfSize >= 0xfe){  // We dont want to risk an overflow of the size byte
      return;
    }
    dtmfSize++;
    dtmf = (char*)realloc(dtmf, dtmfSize + 1);
    dtmf[dtmfSize] = 0;
    dtmf[dtmfSize - 1] = key;
  }
}

void clear_dtmf(){
  dtmf = (char*)realloc(dtmf, 1);
  dtmf[0] = 0;
  dtmfSize = 0;
}

void write_value(uint8_t value){
  for(uint8_t i = 0; i < 8; i++){
    digitalWrite(i + 2, (value >> i) & 1);
  }
}

void generate_tone(uint32_t time_delay, uint16_t freq1, uint16_t freq2){
  Serial.println(freq1);
  Serial.println(freq2);
  
  freq1_pos = 0;
  freq2_pos = 0;
  freq1_offset = (byte)round((freq1 * WAVE_TABLE_SIZE) / 10000);
  freq2_offset = (byte)round((freq2 * WAVE_TABLE_SIZE) / 10000);

  // Enable Timer
  noInterrupts();
  TCCR1A = 0;
  TCNT1 = 0;
  TIMSK1 = 0;

  OCR1A = 24;
  TCCR1B = (1 << WGM12);
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
ISR(TIMER1_COMPA_vect) {
  byte value = waveTable[freq1_pos] + waveTable[freq2_pos];
  write_value(value);

  freq1_pos += freq1_offset;
  freq2_pos += freq2_offset;

  if(freq1_pos >= WAVE_TABLE_SIZE){
    freq1_pos -= WAVE_TABLE_SIZE;
  }

  if(freq2_pos >= WAVE_TABLE_SIZE){
    freq2_pos -= WAVE_TABLE_SIZE;
  }
  
}
