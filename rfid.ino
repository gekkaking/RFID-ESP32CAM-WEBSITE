#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 4;     // Configurable, see typical pin layout above
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Массив инициализации, в котором будет храниться новый NUID
byte nuidPICC[4];

void setup() { 
  Serial.begin(115200);
  SPI.begin(); // Инициализация шины SPI
  rfid.PCD_Init(); // Инициализация MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("Этот код сканирует MIFARE Classic NUI."));
  Serial.print(F("Используя следующий ключ:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {

  // Ищу новые карты
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Убедитесь, что NUID был Инициализирован
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("Тип PICC: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Проверьте, является ли PICC типа Classic MIFARE
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Ваш тег не относится к типу MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("Обнаружена новая карта."));

    // Сохранить NUID в массиве nuidPICC
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("Тег NUID:"));
    Serial.print(F("В 16теричном формате: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("В dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else Serial.println(F("Карта считана ранее."));

  // Остановить PICC
  rfid.PICC_HaltA();

  // Остановить шифрование на PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Вспомогательная процедура для вывода массива байтов в виде шестнадцатеричных значений в Serial.
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Вспомогательная процедура для вывода массива байтов в виде значений dec в Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
