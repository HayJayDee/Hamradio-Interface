#include <Keypad.h>

#define ROWS 4
#define COLS 4

const char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

byte dtmfSize = 0;
char* dtmf = 0;

void setup() {
  Serial.begin(9600);
  dtmf = (char*)malloc(1);
  dtmf[0] = 0;
}

void loop() {
  char key = keypad.waitForKey();
  if(key == 'A') {
    for(byte i = 0; i < dtmfSize; i++){
      // TODO: Play dtmf tone
    }
    // TODO: Play dtmf tone
    Serial.println(dtmf);
    clear_dtmf();
  }else if(key == 'C'){
    // Clear DTMF
    clear_dtmf();
  }else if(key == 'B' || key == 'D'){
    // Don't do anything just for catching else statement
  }else {
    if(dtmfSize < 0xfe){  // We dont want to risk an overflow of the size byte
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
