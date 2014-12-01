
enum DeviceState{
  DISARMED,
  ARMED,
  WAITING_FOR_ARM,
  WAITING_FOR_DISARM,
  WAITING_TO_ARM,
  ALARMING,
  DISABLED
};

enum EEPROMVariable{
  DEVSTATE,
  PREVSTATE,
  SEQCOUNTER,
  EVENT
};

struct RGBColor{
  int red;
  int green;
  int blue;
};

//Keypad definition
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

//Color definitions
const unsigned long disabledColor = 0x000000; //off
const unsigned long armedColor = 0xFFA500; //orange
const unsigned long disarmedColor = 0x00FF00; //green
const unsigned long alarmingColor = 0xFF0000; //red
const unsigned long wfaColor = 0x0000FF; //blue
const unsigned long wfdColor = 0x00FFFF; //cyan
const unsigned long wtaColor = 0xFFFF00; //yellow

//The following variables are stored in the Arduino's EEPROM.
//They will remain in memory even when the device is off.
const unsigned int devStateAddr = 0x00;
const unsigned int prevStateAddr = 0x02;
const unsigned int seqCounterAddr = 0x04;
const unsigned int eventAddr = 0x08;

template <class T>
int storeEEPROM (EEPROMVariable ev, const T& val){
  int addr;
  switch (ev){
    case DEVSTATE:
      addr = devStateAddr;
      break;
    case PREVSTATE:
      addr = prevStateAddr;
      break;
    case SEQCOUNTER:
      addr = seqCounterAddr;
      break;
    case EVENT:
      addr = eventAddr;
      break;
  }

  const byte* v = (const byte*) (const void*) &val;
  unsigned int i;
  for (i = 0; i < sizeof(val); i++)
    EEPROM.write(addr++, *v++);
  return i;
}

template <class T>
int loadEEPROM(EEPROMVariable ev, T& val){
  int addr;
  switch (ev){
    case DEVSTATE:
      addr = devStateAddr;
      break;
    case PREVSTATE:
      addr = prevStateAddr;
      break;
    case SEQCOUNTER:
      addr = seqCounterAddr;
      break;
    case EVENT:
      addr = eventAddr;
      break;
  }

  byte* v = (byte*) (void*) &val;
  unsigned int i;
  for (i = 0; i < sizeof(val); i++){
    *v++ = EEPROM.read(addr++);
  }

  return i;

}