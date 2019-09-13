  #include <SPI.h>
  #include <MFRC522.h>

  #define RST_PIN         9  
  #define SS_PIN          10  

MFRC522 mfrc522(SS_PIN, RST_PIN);  
MFRC522::MIFARE_Key key;
  
char choice;
  
void setup() {
   Serial.begin(9600);
   while (!Serial);
   SPI.begin();
   mfrc522.PCD_Init();
   delay(4);
   mfrc522.PCD_DumpVersionToSerial();
   Serial.println(F("1.Read\n2.Write\n3.Erase\n4.Clone"));
}

void loop() {
  displayMenu();
}

void displayMenu(){
  
  choice = Serial.read();
  switch(choice){
    case '1': choice1();
      break;
    case '2': choice2();
      break;
    case '3': choice3();
      break;
    case '4': choice4();
      break;
    default:
      break;            
    }
}

void choice1(){
  Serial.print("Read UID: ");


  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  byte len;
  MFRC522::StatusCode status;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  Serial.println(F("Card UID:"));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
  Serial.println(F("\nPICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  Serial.print(F("Data: "));

  byte buffer1[18];
  block = 4;
  len = 18;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //Print user data
  for (uint8_t i = 0; i < 16; i++)
  {
      Serial.write(buffer1[i]);
  }
  
  byte buffer3[18];
  block = 5;
  len = 18;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 5, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer3, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //Print user data continued
  for (uint8_t i = 0; i < 16; i++)
  {
      Serial.write(buffer3[i]);
  }
  Serial.print(" ");

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Print user
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] );
  }


  Serial.println(F("\n**End Reading**\n"));

  delay(1000);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();


  Serial.print("1.Read\n2.Write\n3.Erase\n4.Clone");
  displayMenu();
}


void choice2(){
  Serial.print(F("Initiate Write data"));
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  byte buffer[34];
  MFRC522::StatusCode status;
  byte block;
  byte len;

  Serial.setTimeout(20000L) ;  // wait until 20 seconds for input from serial
  // Ask personal data: Name
  Serial.println(F("Type first and last name followed by #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; //Read name from serial monitor
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     //pad with spaces

  block = 1;
  //Authenticate block
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  //Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  block = 2;
  //Authenticate
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  //Ask for personal data: User data
  Serial.println(F("Enter desired input data followed by a #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; //Read user input from serial monitor
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     //Pad with spaces

  block = 4;
  //Authenticate block
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  block = 5;
  //Authenticate block
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

  Serial.print("1.Read \n2.Write\n3.Erase\n4.Clone");
  displayMenu();
}

void choice3(){
  Serial.print("Erase Data:");

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.println(F("**Card Detected:**"));

  Serial.println(F("Card UID:"));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
  Serial.println(F("\nPICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  Serial.println("**Card Selected**");

  int blocks[] = {1, 2, 4, 5};
  for (int block : blocks){
  byte blockcontent[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int largestModulo4Number=block/4*4;
  int trailerBlock=largestModulo4Number+3;
       
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("PCD_Authenticate() failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
     }
    
  status =  mfrc522.MIFARE_Write(block, blockcontent, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("MIFARE_Write() failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
     }
  Serial.println(F("Data has been erased"));
   }    
  Serial.println(" ");
  mfrc522.PICC_HaltA(); //Halt PICC
  mfrc522.PCD_StopCrypto1();  //Stop encryption on PCD

  Serial.println(F("1.Read\n2.Write\n3.Erase\n4.Clone"));
  displayMenu();
}

void choice4(){
  Serial.print("\nInitiate Cloning procedure");
  Serial.print("\nPlace RFID tag that is to be cloned");

  
  Serial.print("Read UID: ");


  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  byte len;
  MFRC522::StatusCode status;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  Serial.println(F("Card UID:"));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
  Serial.println(F("\nPICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  Serial.print(F("Data: "));

  byte buffer1[18];
  block = 4;
  len = 18;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  String userData = "";
  
  //Print and read user data
  for (uint8_t i = 0; i < 16; i++)
  {
      userData += buffer1[i];
      Serial.write(buffer1[i]);
  }

//  strcpy((char *)buffer1,"0123");
//  String myString = String((char *)buffer1);
//  Serial.print("This is myString: " + myString);
  
  byte buffer3[18];
  block = 5;
  len = 18;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 5, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer3, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //Print and read user data continued
  for (uint8_t i = 0; i < 16; i++)
  {
      userData += buffer3[i];
      Serial.write(buffer3[i]);
  }
  Serial.print(" ");

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  String user = "";
  //Print user
  for (uint8_t i = 0; i < 16; i++) {
    user += buffer2[i];
    Serial.write(buffer2[i] );
  }

  Serial.println("UserData: ");
  Serial.println(userData);
  
  Serial.println("User: ");
  Serial.println(user);
  
  Serial.println(F("\n**End Reading**\n"));
  
  delay(5000);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();


//  Serial.print("1.Read\n2.Write\n3.Erase\n4.Clone");
//  displayMenu();

  Serial.print("\nRemove this tag and place the other one");
  delay(5000);
  
  //erase data to make sure there's nothing in there
  choice3();

  //write the copied data from the first tag
    Serial.print(F("Initiate Write data"));
//  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  byte buffer[34];
//  MFRC522::StatusCode status;
//  byte block;
//  byte len;

  //Serial.setTimeout(20000L) ;  // wait until 20 seconds for input from serial
  // Ask personal data: Name
  //Serial.println(F("Type first and last name followed by #"));
  //len = Serial.readBytesUntil('#', (char *) buffer, 30) ; //Read name from serial monitor
  for (byte i = user.length(); i < 30; i++) buffer[i] = ' ';     //pad with spaces

  block = 1;
  //Authenticate block
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  //Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  block = 2;
  //Authenticate
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  //Ask for personal data: User data
  Serial.println(F("Enter desired input data followed by a #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; //Read user input from serial monitor
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     //Pad with spaces

  block = 4;
  //Authenticate block
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  block = 5;
  //Authenticate block
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

  Serial.print("1.Read \n2.Write\n3.Erase\n4.Clone");
  displayMenu();
  
}
