
enum DeviceState{
  DISABLED,
  ARMED,
  DISARMED,
  WAITING_FOR_ARM,
  WAITING_FOR_DISARM,
  WAITING_TO_ARM,
  ALARMING
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
unsigned long disabledColor = 0x000000; //off
unsigned long armedColor = 0xFFA500; //orange
unsigned long disarmedColor = 0x00FF00; //green
unsigned long alarmingColor = 0xFF0000; //red
unsigned long wfaColor = 0x0000FF; //blue
unsigned long wfdColor = 0x00FFFF; //cyan
unsigned long wtaColor = 0xFFFF00; //yellow
