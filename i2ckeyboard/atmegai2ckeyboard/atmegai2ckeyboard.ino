
#include <Wire.h>


static byte msg[7];

// rows are inputs
//static byte rows[] = {11, 10, 9, 12, 13, 8, 7, 6, 5, 4}; // {2,3,4};
static byte rows[] = {4, 5, 6, 7, 8, 13, 12, 9, 10, 11}; // {2,3,4};
const int rowCount = sizeof(rows) / sizeof(rows[0]);

// cols are outputs
static byte cols[] = {A3, A2, A1, A0, 3}; //{8,9,10,11};
const int colCount = sizeof(cols) / sizeof(cols[0]);

static byte keys[colCount][rowCount];

void setup() {
  //Serial.begin(115200);

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event

  for (int x = 0; x < rowCount; x++) {
    //Serial.print(rows[x]); Serial.println(" as input");
    pinMode(rows[x], INPUT);
  }

  for (int x = 0; x < colCount; x++) {
    //Serial.print(cols[x]); Serial.println(" as input-pullup");
    pinMode(cols[x], INPUT_PULLUP);
  }

}

void readMatrix() {
  // iterate the columns
  for (int colIndex = 0; colIndex < colCount; colIndex++) {
    // col: set to output to low
    byte curCol = cols[colIndex];
    pinMode(curCol, OUTPUT);
    digitalWrite(curCol, LOW);

    // row: interate through the rows
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
      byte rowCol = rows[rowIndex];
      pinMode(rowCol, INPUT_PULLUP);
      keys[colIndex][rowIndex] = digitalRead(rowCol);
      pinMode(rowCol, INPUT);
    }
    // disable the column
    pinMode(curCol, INPUT);
  }
  byte * pt = &msg[0];
  byte v = 0;
  v |= (keys[0][0] ? 0x80 : 0);
  v |= (keys[1][0] ? 0x40 : 0);
  v |= (keys[2][0] ? 0x20 : 0);
  v |= (keys[3][0] ? 0x10 : 0);
  v |= (keys[0][1] ? 0x08 : 0);
  v |= (keys[1][1] ? 0x04 : 0);
  v |= (keys[2][1] ? 0x02 : 0);
  v |= (keys[3][1] ? 0x01 : 0);
  *pt++ = v;
  v = 0;
  v |= (keys[0][2] ? 0x80 : 0);
  v |= (keys[1][2] ? 0x40 : 0);
  v |= (keys[2][2] ? 0x20 : 0);
  v |= (keys[3][2] ? 0x10 : 0);
  v |= (keys[0][3] ? 0x08 : 0);
  v |= (keys[1][3] ? 0x04 : 0);
  v |= (keys[2][3] ? 0x02 : 0);
  v |= (keys[3][3] ? 0x01 : 0);
  *pt++ = v;
  v = 0;
  v |= (keys[0][4] ? 0x80 : 0);
  v |= (keys[1][4] ? 0x40 : 0);
  v |= (keys[2][4] ? 0x20 : 0);
  v |= (keys[3][4] ? 0x10 : 0);
  v |= (keys[0][5] ? 0x08 : 0);
  v |= (keys[1][5] ? 0x04 : 0);
  v |= (keys[2][5] ? 0x02 : 0);
  v |= (keys[3][5] ? 0x01 : 0);
  *pt++ = v;
  v = 0;
  v |= (keys[0][6] ? 0x80 : 0);
  v |= (keys[1][6] ? 0x40 : 0);
  v |= (keys[2][6] ? 0x20 : 0);
  v |= (keys[3][6] ? 0x10 : 0);
  v |= (keys[0][7] ? 0x08 : 0);
  v |= (keys[1][7] ? 0x04 : 0);
  v |= (keys[2][7] ? 0x02 : 0);
  v |= (keys[3][7] ? 0x01 : 0);
  *pt++ = v;
  v = 0;
  v |= (keys[0][8] ? 0x80 : 0);
  v |= (keys[1][8] ? 0x40 : 0);
  v |= (keys[2][8] ? 0x20 : 0);
  v |= (keys[3][8] ? 0x10 : 0);
  v |= (keys[0][9] ? 0x08 : 0);
  v |= (keys[1][9] ? 0x04 : 0);
  v |= (keys[2][9] ? 0x02 : 0);
  v |= (keys[3][9] ? 0x01 : 0);
  *pt++ = v;
  v = 0;
  v |= (keys[4][0] ? 0x80 : 0);
  v |= (keys[4][1] ? 0x40 : 0);
  v |= (keys[4][2] ? 0x20 : 0);
  v |= (keys[4][3] ? 0x10 : 0);
  v |= (keys[4][4] ? 0x08 : 0);
  v |= (keys[4][5] ? 0x04 : 0);
  v |= (keys[4][6] ? 0x02 : 0);
  v |= (keys[4][7] ? 0x01 : 0);
  *pt++ = v;
  v = 0x3F;
  v |= (keys[4][8] ? 0x80 : 0);
  v |= (keys[4][9] ? 0x40 : 0);
  *pt++ = v;
}

void printMatrix() {
  /*
    for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
        if (rowIndex < 10)
            Serial.print(F("0"));
        Serial.print(rowIndex); Serial.print(F(": "));

        for (int colIndex=0; colIndex < colCount; colIndex++) {
            Serial.print(keys[colIndex][rowIndex]);
            if (colIndex < colCount)
                Serial.print(F(", "));
        }
        Serial.println("");
    }
    Serial.println("");
    Serial.println("");
    Serial.println("");
  */
  /*
      Serial.println(msg[0], BIN);
      Serial.println(msg[1], BIN);
      Serial.println(msg[2], BIN);
      Serial.println(msg[3], BIN);
      Serial.println(msg[4], BIN);
      Serial.println("");
      Serial.println("");
      Serial.println("");
  */
}

void loop() {
  readMatrix();
  //    printMatrix();
  delay(2);
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(msg, 7);
  //Wire.write("hello "); // respond with message of 6 bytes
}

