#include <SPI.h>
#include <SD.h>

const int chipSelect1 = 10; // Chip Select pre prvý SD Card modul
const int chipSelect2 = 9; // Chip Select pre druhý SD Card modul

const int redLED = 2; // Pripojenie červenej LED diódy
const int greenLED = 3; // Pripojenie zelenej LED diódy
const int orangeLED = 4; // Pripojenie oranžovej LED diódy

void setup() {
  Serial.begin(9600);
  
  // Inicializácia LED diód
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(orangeLED, OUTPUT);
  
  // Inicializácia SD kariet
  if (!SD.begin(chipSelect1) || !SD.begin(chipSelect2)) {
    Serial.println("Chyba inicializácie SD kariet!");
    return;
  }
  Serial.println("SD karty inicializované.");
}

void loop() {
  // Zistíme počet súborov na prvom SD kartovom module
  int fileCount = countFiles(chipSelect1);
  
  // Ak sa nepodarí získať počet súborov, blika oranžová LED dióda
  if (fileCount == -1) {
    blinkLED(orangeLED, 10);
    return;
  }
  
  // Ak nie sú žiadne súbory na prvej SD karte, nič sa nekopíruje
  if (fileCount == 0) {
    return;
  }
  
  // Skopírujeme súbory zo prvej SD karty na druhú
  if (!copyFiles(chipSelect1, chipSelect2)) {
    // Ak sa nepodarí skopírovať súbory, blika červená LED dióda
    blinkLED(redLED, 10);
  } else {
    // Ak sa súbory úspešne skopírujú, blika zelená LED dióda
    blinkLED(greenLED, 10);
  }
}

int countFiles(int chipSelect) {
  File root = SD.open("/");
  int count = 0;
  
  if (!root) {
    Serial.println("Chyba pri otvorení root priečinka!");
    return -1;
  }
  
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      // Koniec priečinka
      break;
    }
    count++;
    entry.close();
  }
  
  root.close();
  return count;
}

bool copyFiles(int sourceChipSelect, int destinationChipSelect) {
  File sourceRoot = SD.open("/");
  File destinationRoot = SD.open("/", FILE_WRITE);
  
  if (!sourceRoot || !destinationRoot) {
    Serial.println("Chyba pri otvorení root priečinkov!");
    return false;
  }
  
  while (true) {
    File sourceFile = sourceRoot.openNextFile();
    if (!sourceFile) {
      // Koniec priečinka
      break;
    }
    File destinationFile = destinationRoot.openNextFile(FILE_WRITE);
    if (!destinationFile) {
      Serial.println("Chyba pri otvorení cieľového súboru!");
      return false;
    }
    while (sourceFile.available()) {
      destinationFile.write(sourceFile.read());
    }
    sourceFile.close();
    destinationFile.close();
  }
  
  sourceRoot.close();
  destinationRoot.close();
  return true;
}

void blinkLED(int pin, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
  }
}
