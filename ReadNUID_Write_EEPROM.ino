#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
 
constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 4;     // Configurable, see typical pin layout above
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
 
MFRC522::MIFARE_Key key; 
 
// Init array that will store new NUID 
String nuidPICC;
 
void setup() { 
  EEPROM.begin(512);  //Initialize EEPROM
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
 
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
}
 
void loop() {
 
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
 
  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;
 
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
 
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
 
  nuidPICC="";
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    if(rfid.uid.uidByte[i]< 0x10){
      nuidPICC += 0;
      nuidPICC += String(rfid.uid.uidByte[i],HEX);
    }
    else {
      nuidPICC += String(rfid.uid.uidByte[i],HEX);
    }
  }

  for(int i=0;i<8;i++) 
  {
    EEPROM.write(i,nuidPICC[i]);  
  }
  
  EEPROM.commit();
   
  Serial.println(nuidPICC);

  // Halt PICC
  rfid.PICC_HaltA();
 
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
 
