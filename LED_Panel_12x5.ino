unsigned long lastMillis = 0;
unsigned long delayMillis = 120;

byte MONITOR_1A = B00000000;
byte MONITOR_1B = B00000000;

byte MONITOR_2A = B00000000;
byte MONITOR_2B = B00000000;

byte MONITOR_3A = B00000000;
byte MONITOR_3B = B00000000;

byte MONITOR_4A = B00000000;
byte MONITOR_4B = B00000000;

byte MONITOR_5A = B00000000;
byte MONITOR_5B = B00000000;

int rowToLight;

// String stringBuffer = "Test buffer";

//char message[300] = "Hello world!";
//int messageLength = 12;
char message[] = "CIAO PAOLO, BUON VIAGGIO";
int messageLength;
int messageIndex;
int cleanBufferLength;

boolean messageBuffer1[160] = {0};
boolean messageBuffer2[160] = {0};
boolean messageBuffer3[160] = {0};
boolean messageBuffer4[160] = {0};
boolean messageBuffer5[160] = {0};

int messageBufferLength = 0;
int bufferReadIndex;

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

void setup() {

  Serial.begin(9600);
  
  rowToLight = 1;

  messageBufferLength = 0;
  bufferReadIndex = 0;
  
  messageLength = sizeof(message);
  messageIndex = messageLength;
  
  cleanBufferLength = 12;

  // pins 2 to 13 and 14 to 18 as output

  // pins 7 to 2 as output
  DDRD = DDRD | B11111100;

  // pins 13 to 8 as output
  DDRB = DDRB | B00111111;

  // pins A4 to A0 (18 to 14) as output
  DDRC = DDRB | B00011111;




  // reset registers (2 to 18)

  // 7 to 2
  PORTD = B00000000;

  // 13 to 8
  PORTB = B00000000;

  // vertical

  // 18 to 14
  PORTC = B00000000;


  lastMillis = millis();
}


void loop() {

  unsigned long elapsed = 0;
  unsigned long milliseconds = millis();
  // int nextChar = Serial.read();
  // stringBuffer += String(nextChar);

  if (messageIndex > 0) {
    loadCharIntoBuffer(message[messageLength - messageIndex]);
    addSpace();
    messageIndex--;
  } else {
    if (cleanBufferLength > 0) {
      addSpace();
      cleanBufferLength--;
    }
  }

  elapsed = milliseconds - lastMillis;

  if (elapsed > delayMillis) {
    Serial.write("Next step\n");
    nextStepBuffer();
    lastMillis = milliseconds;
  }

  lightRow();
  
  delay(4);
}



void lightRow() {

  switch (rowToLight) {
    case 1:
      PORTC = B00000001;
      PORTB = MONITOR_1A;
      PORTD = MONITOR_1B;
      break;
    case 2:
      PORTC = B00000010;
      PORTB = MONITOR_2A;
      PORTD = MONITOR_2B;
      break;
    case 3:
      PORTC = B00000100;
      PORTB = MONITOR_3A;
      PORTD = MONITOR_3B;
      break;
    case 4:
      PORTC = B00001000;
      PORTB = MONITOR_4A;
      PORTD = MONITOR_4B;
      break;
    case 5:
      PORTC = B00010000;
      PORTB = MONITOR_5A;
      PORTD = MONITOR_5B;
      break;
  }
  
  rowToLight++;
  
  if (rowToLight > 5) {
    rowToLight = 1;
  }
}


// copy buffer to monitor
// shift everything by 1
// copy last B to A
// shift buffer in B
void nextStepBuffer() {

  if (messageBufferLength == 0) {
    return;
  }

  if (bufferReadIndex > messageBufferLength) {
    bufferReadIndex = 0;
    messageBufferLength = 0;
    return;
  }

  // shift bits to left (ignoring first 2 bits)
  MONITOR_1A = (MONITOR_1A << 1) & B00111111;
  // shifts last bit from right screen to left
  MONITOR_1A |= MONITOR_1B >> 7;
  // shift bits to left (ignoring last 2 bits)
  MONITOR_1B = MONITOR_1B << 1;
  // shift 1 bit from buffer to right screen part
  MONITOR_1B |= messageBuffer1[bufferReadIndex] ? B00000100 : B00000000;


  MONITOR_2A = (MONITOR_2A << 1) & B00111111;
  MONITOR_2A |= MONITOR_2B >> 7;
  MONITOR_2B = MONITOR_2B << 1;
  MONITOR_2B |= messageBuffer2[bufferReadIndex] ? B00000100 : B00000000;


  MONITOR_3A = (MONITOR_3A << 1) & B00111111;
  MONITOR_3A |= MONITOR_3B >> 7;
  MONITOR_3B = MONITOR_3B << 1;
  MONITOR_3B |= messageBuffer3[bufferReadIndex] ? B00000100 : B00000000;


  MONITOR_4A = (MONITOR_4A << 1) & B00111111;
  MONITOR_4A |= MONITOR_4B >> 7;
  MONITOR_4B = MONITOR_4B << 1;
  MONITOR_4B |= messageBuffer4[bufferReadIndex] ? B00000100 : B00000000;


  MONITOR_5A = (MONITOR_5A << 1) & B00111111;
  MONITOR_5A |= MONITOR_5B >> 7;
  MONITOR_5B = MONITOR_5B << 1;
  MONITOR_5B |= messageBuffer5[bufferReadIndex] ? B00000100 : B00000000;

  bufferReadIndex++;
}

void addSpace() {
  messageBuffer1[messageBufferLength]=0;
  messageBuffer2[messageBufferLength]=0;
  messageBuffer3[messageBufferLength]=0;
  messageBuffer4[messageBufferLength]=0;
  messageBuffer5[messageBufferLength]=0;
  messageBufferLength++;
}

void loadCharIntoBuffer(char inputChar) {
  
  int i;
  for (i = 0; i < 3; i++) {
    int charIndex = messageBufferLength + i;
    messageBuffer1[charIndex] = bitmapForCharAndRow(inputChar, 0, 2 - i);
    messageBuffer2[charIndex] = bitmapForCharAndRow(inputChar, 1, 2 - i);
    messageBuffer3[charIndex] = bitmapForCharAndRow(inputChar, 2, 2 - i);
    messageBuffer4[charIndex] = bitmapForCharAndRow(inputChar, 3, 2 - i);
    messageBuffer5[charIndex] = bitmapForCharAndRow(inputChar, 4, 2 - i);
  }
  
  messageBufferLength += 3;
}


int bitmapForCharAndRow(char inputChar, int row, int col) {

  int charMap[][5] = {
    {2,5,7,5,5}, // a 0
    {6,5,6,5,6}, // b 1
    {3,4,4,4,3}, // c 2
    {6,5,5,5,6}, // d 3
    {7,4,6,4,7}, // e 4
    {7,4,6,4,4}, // f 5
    {3,4,5,5,3}, // g 6
    {5,5,7,5,5}, // h 7
    {2,2,2,2,2}, // i 8
    {1,1,1,1,6}, // j 9
    {3,4,6,6,3}, // k 10
    {4,4,4,4,7}, // l 11
    {5,7,5,5,5}, // m 12
    {5,5,7,5,5}, // n 13
    {2,5,5,5,2}, // o 14
    {6,5,6,4,4}, // p 15
    {3,4,6,6,3}, // q 16
    {6,5,6,5,5}, // r 17
    {3,4,6,6,3}, // s 18
    {7,2,2,2,2}, // t 19
    {5,5,5,5,7}, // u 20
    {5,5,5,5,2}, // v 21
    {3,4,6,6,3}, // w 22
    {5,5,2,5,5}, // x 23
    {3,4,6,6,3}, // y 24
    {3,4,6,6,3}, // z 25
    {0,0,0,0,0}, // [space] 26
    {7,5,5,5,7}, // 0 27
    {2,6,2,2,2}, // 1 28
    {2,5,1,2,7}, // 2 29
    {6,1,6,1,6}, // 3 30
    {5,5,7,1,1}, // 4 31
    {7,4,7,1,7}, // 5 32
    {7,4,7,5,7}, // 6 33
    {3,1,1,1,1}, // 7 34
    {7,5,7,5,7}, // 8 35
    {7,5,7,1,7}, // 9 36
    {0,0,0,0,2}, // . 37
    {0,0,2,0,2}, // : 38
    {0,0,0,2,4}, // , 39
    {0,0,7,0,0}, // - 40
    {2,2,2,0,2}  // ! 41
  };

  int mapVal, charInt;
  
  charInt = (int) inputChar;

  switch (charInt) {
    case 33: // !
      mapVal = charMap[41][row];
      break;
    case 44: // ,
      mapVal = charMap[39][row];
      break;
    case 45: // -
      mapVal = charMap[40][row];
      break;
    case 46: // .
      mapVal = charMap[37][row];
      break;
    case 48: // 0
      mapVal = charMap[27][row];
      break;
    case 49: // 1
      mapVal = charMap[28][row];
      break;
    case 50: // 2
      mapVal = charMap[29][row];
      break;
    case 51: // 3
      mapVal = charMap[30][row];
      break;
    case 52: // 4
      mapVal = charMap[31][row];
      break;
    case 53: // 5
      mapVal = charMap[32][row];
      break;
    case 54: // 6
      mapVal = charMap[33][row];
      break;
    case 55: // 7
      mapVal = charMap[34][row];
      break;
    case 56: // 0
      mapVal = charMap[35][row];
      break;
    case 57: // 0
      mapVal = charMap[36][row];
      break;
    case 58: // :
      mapVal = charMap[38][row];
      break;
    case 65: // A
      mapVal = charMap[0][row];
      break;
    case 66: // B
      mapVal = charMap[1][row];
      break;
    case 67: // C
      mapVal = charMap[2][row];
      break;
    case 68: // D
      mapVal = charMap[3][row];
      break;
    case 69: // E
      mapVal = charMap[4][row];
      break;
    case 70: // F
      mapVal = charMap[5][row];
      break;
    case 71: // G
      mapVal = charMap[6][row];
      break;
    case 72: // G
      mapVal = charMap[7][row];
      break;
    case 73: // I
      mapVal = charMap[8][row];
      break;
    case 74: // L
      mapVal = charMap[9][row];
      break;
    case 75: // M
      mapVal = charMap[10][row];
      break;
    case 76:
      mapVal = charMap[11][row];
      break;
    case 77:
      mapVal = charMap[12][row];
      break;
    case 78:
      mapVal = charMap[13][row];
      break;
    case 79:
      mapVal = charMap[14][row];
      break;
    case 80:
      mapVal = charMap[15][row];
      break;
    case 81:
      mapVal = charMap[16][row];
      break;
    case 82:
      mapVal = charMap[17][row];
      break;
    case 83:
      mapVal = charMap[18][row];
      break;
    case 84: // T
      mapVal = charMap[19][row];
      break;
    case 85: // U
      mapVal = charMap[20][row];
      break;
    case 86: // V
      mapVal = charMap[21][row];
      break;
    case 87: // W
      mapVal = charMap[21][row];
      break;
    case 88: // X
      mapVal = charMap[21][row];
      break;
    case 89: // Y
      mapVal = charMap[21][row];
      break;
    case 90: // Z
      mapVal = charMap[21][row];
      break;
    default:
      mapVal = charMap[26][row];
      break;
  }

  return (mapVal >> col) & 1;
}
