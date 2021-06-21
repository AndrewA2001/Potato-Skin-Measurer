#include <SD.h>
#include <SPI.h>
#include <SharpIR.h>

//file information
File file;
File myFile;


int count;
float sum = 0;
float initial_distance;

//File for collecting the data file where the data is stored

String File_name_part = "DF"; //  IT IS VERY IMPORTANT THAT THIS NAME IS KEPT AS SHORT AS POSSIBLE, FOR SOME REASON LONG NAMES MAKE THE OPENING OF THE FILE CORRUPT

String countFile;
String ending = ".csv";
int flag = 0;
int flag2 = 0; //used to place titles above columns in the data file

//File for the current version the number count

String version_file = "version2.csv";
int data_count = 0;
int x;
float value;

//global variables

const int DatabuttonPin = 6;     // Button increases the file the data is being written to
const int Measurement_Button = 7;
const int erase_wire = 12;

int DigitalbuttonState = 0;
int Measurement_Button_State;



//void return_total_value();
float measurement;
//float measurements_per_cycle = 100;
float measurements_per_cycle = 500;


//debounce1
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int lastButtonState = LOW;

//debounce2:

unsigned long lastDebounceTime2 = 0;  // the last time the output pin was toggled
unsigned long debounceDelay2 = 50;    // the debounce time; increase if the output flickers
int lastButtonState2 = LOW;

//debounce 3: For the reset Wire

unsigned long lastDebounceTime3 = 0;  // the last time the output pin was toggled
unsigned long debounceDelay3 = 50;    // the debounce time; increase if the output flickers
const int Wire_pin = 12; //pin for the wire


boolean wire_state = 1; //current reading of the pin
boolean last_wire_state = 1; //previous reading of the wire
boolean reading3;














//object for the sensor that measures all of the sensor
SharpIR sensor( SharpIR::GP2Y0A21YK0F, A0 );


//******************************FUNCTIONS**********************************

//returns the distance value in cm of the infared sensor

float return_total_value()
{
  for (int x = 0; x < measurements_per_cycle; x++)
  {
    measurement = sensor.getDistance();
    sum = sum + measurement;
    //delay(20);
  }
  sum = sum / measurements_per_cycle;
  //Serial.println(sum);
  //account for accuracy error
  sum = sum - .2;
  return sum;
  sum = 0;

}

//unimportant function for now but could be used in the future
bool readLine(File & f, char* line, size_t maxLen) {
  for (size_t n = 0; n < maxLen; n++) {
    int c = f.read();
    if ( c < 0 && n == 0) return false;  // EOF
    if (c < 0 || c == '\n') {
      line[n] = 0;
      return true;
    }
    line[n] = c;
  }
  return false; // line too long
}

bool readVals(int* v1) {
  char line[40], *ptr, *str;
  if (!readLine(file, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  *v1 = strtol(line, &ptr, 10);
  if (ptr == line) return false;  // bad number if equal
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  return str != ptr;  // true if number found
}

/*
   fuction is run when the program first intializes
   to read through a separate csv that can be used to change the number of the data
   file being written too

*/


//reads the version file
void update_data_file()
{

  String number;
  //change to count.csv


  if (SD.exists(version_file))
  {
    file = SD.open(version_file, FILE_READ);
    if (!file) {
      Serial.println("open error");
      return;
    }
    while (readVals(&x)) {

      Serial.println("value of X:  ");
      Serial.println(x);

    }

    if (flag == 0)
    {
      data_count = 1;
      file = SD.open(version_file, FILE_WRITE);
      file.println(x++);
      file.close();

      flag = 1;
    }

    //prevents the same file from being written too twice


    Serial.println("Done");

    number = String(x);
    countFile = File_name_part + number + ending;
    Serial.println("The Final Name of the File is:   ");
    Serial.println(countFile);





    file.close();

    initial_distance = return_total_value();



    file = SD.open(countFile, FILE_WRITE);

    if (!file) {
      Serial.println("open error");
      return;
    }

    //only runs on the booting up of the program

    file.println("Count,Thickness");

    Serial.print("Updated number will occure");


    file.close();
    Serial.print("function Successfully Run");
  }
  else
  {
    file = SD.open(version_file, FILE_WRITE);
    file.print("0");
    file.close();
    x = 0;
  }

}

//changes the file the data is being written too

void change_data_file()
{

  myFile = SD.open(version_file, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to Version File");

    x++;
    myFile.println(x);
    // close the file:
    myFile.close();
  }
  //will update the version file and the name of the data file
  update_data_file();
}

//this function writes a value to the respective data file taken from the measuring device
void write_value(float val)
{
  String var, var2;
  data_count++;

  var = String(data_count);
  var2 = String(val);
  myFile = SD.open(countFile, FILE_WRITE);
  Serial.print("Printing the Data Count");
  Serial.print(data_count);

  myFile.println(var + "," + var2);
  myFile.close();

}


//This function clears out contents of the varieties of the data file

void clear_SD_card()
{
  String Delete_File;
  String Letter; //used to convert int n to a string

  //runs through all the files stored on the SD card

  for (int n = 0; n < (x + 1); n++) //x is the number that is attached to the end of the most recent file so this ensures everything is deleted after that
  {
    Letter = String(n);
    Delete_File = File_name_part + Letter + ending;
    Serial.println("File to Delete");
    Serial.println(Delete_File);

    if (SD.exists(Delete_File))
    {
      SD.remove((Delete_File));
    }
  }

  //removes the file containing all the versions

  SD.remove(version_file);

  file = SD.open(version_file, FILE_WRITE);
  file.println("0");
  file.close();

  Serial.println("SD card Succesfuly Reset");

  x = 0;

  //If the File exists then delete it


  //D the name of the version file and write a zero to initalize it once again

}




//Gets rid of all the files generated to make space for
////  void destroy_all_files{
////    //removes whatever the version of the
////    SD.remove(version_file);
////
////
////    for(int i = 0; i < 250 ; i++)
////    {
////
////
////
////
////
////    }
//
//
//
//   //writes a zero back in the version file
//
//   myFile = SD.open(version_file, FILE_WRITE);
//    // if the file opened okay, write to it:
//    if (myFile) {
//      Serial.println("Writing to Version File");
//      myFile.println("0");
//      myFile.close();
//    }
//  }











//****************************MAIN********************************************

void setup() {

  initial_distance = return_total_value();


  //settinhg up the buttons

  pinMode(DatabuttonPin, INPUT);
  pinMode(Measurement_Button_State, INPUT);
  pinMode(Wire_pin, INPUT_PULLUP);

  //Wire to erase all the data

  pinMode(erase_wire, INPUT_PULLUP);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(8)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  update_data_file();
}

//this function opens the count file to determine name of the data file to represent the name that has been generaed

void loop() {

  int reading = digitalRead(Measurement_Button);
  int reading2 = digitalRead(DatabuttonPin);

  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != Measurement_Button_State) {
      Measurement_Button_State = reading;

      // only toggle the LED if the new button state is HIGH
      if (Measurement_Button_State == HIGH) {
        Serial.print("Measurement Button Pressed");
        Serial.println("initial value");
        Serial.println(initial_distance);
        value = (initial_distance - (return_total_value())-.24); //the .2 is to account for any difference that occurs in the measurment
        Serial.println("printing value: ");
        Serial.println(value);
        write_value(value);
        myFile.close();
      }
    }
  }
  lastButtonState = reading;
  //This button will be pressed if the user desires to performa a different run from before

  //debounce 2
  if (reading2 != lastButtonState2)
  {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay2) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading2 != DigitalbuttonState) {
      DigitalbuttonState = reading2;

      // only toggle the LED if the new button state is HIGH
      if (DigitalbuttonState == HIGH) {
        Serial.print("Digital Button Pressed");
        data_count = 0;
        change_data_file();
      }
    }
  }
  lastButtonState2 = reading2;
  //This button will be pressed if the user de

  //Debounce 3



  reading3 = digitalRead(Wire_pin);
  //  Serial.println("Reading:");
  //  Serial.println(reading);


  // read the state of the switch into a local variable:

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading3 != last_wire_state) {
    // reset the debouncing timer
    lastDebounceTime3 = millis();
  }

  if ((millis() - lastDebounceTime3) > debounceDelay3) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading3 != wire_state) {
      wire_state = reading3;

      // only toggle the LED if the new button state is HIGH
      if (wire_state == 0) {
        clear_SD_card();
      }
    }
  }

  // set the LED:

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  last_wire_state = reading3;




}
