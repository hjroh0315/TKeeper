/*
 * Write personal data of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
// You could use a spare Hardware Serial on boards that have it (like Mega)
#include <SoftwareSerial.h>
SoftwareSerial DebugSerial(2, 3); // RX, TX

#include <BlynkSimpleStream.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "8e68cd15e5524577b47bf2b30cb77552";

#define RST_PIN         5           // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

int green=10000;
const int limit=20000;
int sum=30000;
int neededByte = 0x7B;
bool stat=0;
WidgetTerminal Term(V3);
WidgetLED Grn(V1);
WidgetLED Red(V2);
void setup() {
  DebugSerial.begin(9600);
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Blynk.begin(Serial, auth);
  Term.clear();
  Term.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
}

int prevgreen=10000;

void loop()
{
  Blynk.run();
  MFRC522::MIFARE_Key key;
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      goto outside;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      goto outside;
    }
    //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
    if(mfrc522.uid.uidByte[0] == neededByte)
    {
      Term.println("노약자/장애인입니다");
      if(stat==1&&green<limit)green+=2000;
    }
  }
  outside:
  if(millis() % sum >= prevgreen)
  {
    Term.print("Red");Term.println((millis()/1000)%30);if(stat==0){stat=1;green=10000;}
    Red.on();
    Grn.off();
    Blynk.virtualWrite(V0,(((sum)/1000)-(int(millis()/1000))%30));
  }
  else
  {
    prevgreen=green;
    Term.print("Grn");Term.println((millis()/1000)%30);stat=0;
    Red.off();
    Grn.on();
    Blynk.virtualWrite(V0,((green/1000)-((int(millis()/1000))%30)));
  }
  Term.flush();
  delay(250);
}
