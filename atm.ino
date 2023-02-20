#include <Keypad.h>
#include <LiquidCrystal.h>
#define WITHDRAW_LIMIT 3000
#define LANGUAGE_NO 3
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'0','*','#'}
};

char messages[][16]={
"1. BALANCE     ",
"1. SOLD        ",
"1. SOLDE       ",

"2. WITHDRAW    ", 
"2. RETRAGERE   ",
"2. RETIRER     ",

"3. DEPOSIT     ",
"3. DEPUNERE    ",
"3. DEPOSER     ",

"*  CANCEL      ", 
"*  ANULARE     ",
"*  ANNULATION  ",

"BALANCE        ",
"SOLD           ",
"SOLDE          ",

"AMOUNT         ",
"TOTAL          ",
"MONTANT        ",

"INSUFFICIENT   ",
"FONDURI        ",
"INSUFFISANCE   ",

"FUNDS!         ",
"INSUFICIENTE!  ",
"DE FONDS!      ",

"SUCCESS!       ",
"SUCCES!        ",
"SUCCES!        ",

"ONLY MULTIPLES ",
"DOAR MULTIPLI  ",
"SEULEMENT DES  ",

"OF 10!         ",
"DE 10!         ",
"MULTIPLES DE 10",

"LIMIT EXCEEDED ",
"LIMITA DEPASITA",
"LIMITE DEPASSEE",
};

char insertedPIN[5];
char inputAmount[7];

typedef struct _account{
  char PIN[5];
  long balance;
}Account;

Account accounts[2] = {{"1234",100},{"5678",250}};
Account* currentAccount = &accounts[0];

int viewLanguage = 0;
int viewOption = 0;
int languageIndex=0;
volatile int accountToLoad = 0;

bool insertPinStage = true;
bool chooseLanguageStage = false;
bool chooseOptionStage = false;
bool withdrawStage = false;
bool depositStage = false;
bool balanceStage = false;
bool confirmGoBack = false;

byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {9, 8, A5};

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

Keypad keyPad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned long previousMillis = 0;
const long interval = 800;

void returnToInitState(){
  insertPinStage = true;
  chooseLanguageStage = false;
  chooseOptionStage = false;
  withdrawStage = false;
  depositStage = false;
  balanceStage = false;
  confirmGoBack = false;
  viewLanguage = 0;
  viewOption = 0;
  languageIndex=0;
  accountToLoad = 0;
  memset(inputAmount,0,strlen(inputAmount));
  memset(insertedPIN,0,strlen(insertedPIN));
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ENTER PIN");
}

void loadAccount1(){
  accountToLoad = 1;
}

void loadAccount2(){
  accountToLoad = 2;
}

void returnToOptions(){
  withdrawStage = depositStage = false;
  chooseOptionStage = true;
  viewOption = 0;
  memset(inputAmount,0,strlen(inputAmount));
}


void setup() {
  lcd.begin(16, 2);
  lcd.print("ENTER PIN");
  pinMode(20, INPUT);
  pinMode(21, INPUT);
  attachInterrupt(digitalPinToInterrupt(20), loadAccount1,RISING); 
  attachInterrupt(digitalPinToInterrupt(21), loadAccount2,RISING); 
}

void loop() {
  unsigned long currentMillis = millis();
  char key = keyPad.getKey();
  if(accountToLoad == 1){
    currentAccount = &accounts[0];
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Account 1 loaded.");
    delay(1000);
    returnToInitState();  
  }
  else if(accountToLoad == 2){
    currentAccount = &accounts[1];
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Account 2 loaded.");
    delay(1000);
    returnToInitState(); 
  }
  else if(insertPinStage){
    int pinLen = strlen(insertedPIN);
    lcd.setCursor(pinLen,1);
    if (key!= NO_KEY){
      if (key == '*')
      { 
        if(pinLen > 0){
        insertedPIN[pinLen-1]='\0';
        lcd.setCursor(pinLen-1,1);
        lcd.print(" ");
        } 
      }
      else if (key=='#')
      {
        if (strcmp(currentAccount->PIN,insertedPIN) == 0){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("CORRECT");
          delay(800);
          insertPinStage = false;
          chooseLanguageStage = true;
        }
        else{
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("INCORRECT PIN");
          delay(800);
          returnToInitState();
        }
      }
      else
      {
        if(pinLen < 4){
          lcd.print(key);
          insertedPIN[pinLen] = key;
        }
      }
    }
  }
  else if(chooseLanguageStage){
    if (key == '1' || key == '2' || key == '3'){
      languageIndex = key - '0' -1;
      chooseLanguageStage = false;
      chooseOptionStage = true;
    } else if (key == '*'){
      viewLanguage = 0;
      chooseLanguageStage = false;
      returnToInitState();
    }
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (viewLanguage == 0){
        //first screen
        lcd.setCursor(0,0);
        lcd.print("CHANGE LANGUAGE");
        lcd.setCursor(0,1);
        lcd.print("1. ENGLISH     ");
        viewLanguage++;
      }
      else if (viewLanguage == 1){
        //second screen
        lcd.setCursor(0,0);
        lcd.print("2. ROMANA      ");
        lcd.setCursor(0,1);
        lcd.print("3. FRANCAIS    ");
        viewLanguage++;
      }
      else if (viewLanguage == 2){
        //third screen - go back
        lcd.setCursor(0,0);
        lcd.print("PRESS * TO GO ");
        lcd.setCursor(0,1);
        lcd.print("BACK          ");
        viewLanguage = 0;
      } 
    }
  }
  else if(chooseOptionStage){
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      if(viewOption == 0){
        //first screen
        lcd.setCursor(0,0);
        lcd.print(messages[0 + languageIndex]);
        lcd.setCursor(0,1);
        lcd.print(messages[LANGUAGE_NO + languageIndex]);
        viewOption ++;
      } else {
      //second screeN
        lcd.setCursor(0,0);
        lcd.print(messages[2*LANGUAGE_NO + languageIndex]);
        lcd.setCursor(0,1);
        lcd.print(messages[3*LANGUAGE_NO + languageIndex]);
        viewOption = 0;
      }
    }
    if (key == '*'){
      chooseOptionStage = false;
      chooseLanguageStage = true;
      viewLanguage = 0;
      viewOption = 0;
    }else if (key == '1'){
      chooseOptionStage = false;
      balanceStage = true;

      lcd.clear();
      lcd.print(messages[4*LANGUAGE_NO + languageIndex]);
      lcd.setCursor(0,1);
      lcd.print(currentAccount->balance);
      lcd.print(" RON");

    }else if (key == '2'){
      chooseOptionStage = false;
      withdrawStage = true;

      lcd.clear();
      lcd.print(messages[5*LANGUAGE_NO + languageIndex]);

    }else if(key == '3'){
      chooseOptionStage = false;
      depositStage = true;

      lcd.clear();
      lcd.print(messages[5*LANGUAGE_NO + languageIndex]);
    }
  }
  else if(balanceStage){
    if (key == '*'){
      balanceStage = false;
      chooseOptionStage = true;
      viewOption = 0;
    }
  }
  else if(withdrawStage || depositStage){
    int amountLen = strlen(inputAmount);
    lcd.setCursor(amountLen,1);
    if (key == '*')
    { 
      if(amountLen > 0){
        inputAmount[amountLen-1]='\0';
        lcd.setCursor(amountLen-1,1);
        lcd.print(" ");
      } 
      else if (amountLen == 0)
      {
        returnToOptions();
      }
    }else if (key=='#'){
      long amount;
      if (amountLen>0){
        sscanf(inputAmount,"%ld",&amount);
        if (withdrawStage){
          if(amount > WITHDRAW_LIMIT){
            lcd.clear();
            lcd.print(messages[11*LANGUAGE_NO + languageIndex]);
            lcd.setCursor(0,1);
            lcd.print("MAX ");
            lcd.print(WITHDRAW_LIMIT);
            delay(1200);
            lcd.clear();
            lcd.print(messages[5*LANGUAGE_NO + languageIndex]);
            lcd.setCursor(0,1);
            lcd.print(inputAmount);
          }else if(amount <= currentAccount->balance){
            currentAccount->balance = currentAccount->balance - amount;
            lcd.clear();
            lcd.print(messages[8*LANGUAGE_NO + languageIndex]);
            delay(800);
            returnToOptions();
          }else{
            lcd.clear();
            lcd.print(messages[6*LANGUAGE_NO + languageIndex]);
            lcd.setCursor(0,1);
            lcd.print(messages[7*LANGUAGE_NO + languageIndex]);
            delay(800);
            lcd.clear();
            lcd.print(messages[5*LANGUAGE_NO + languageIndex]);
            lcd.setCursor(0,1);
            lcd.print(inputAmount);
          }
        }
        else if (depositStage){
          if(amount % 10 == 0){
            currentAccount->balance = currentAccount->balance + amount;
            lcd.clear();
            lcd.print(messages[8*LANGUAGE_NO + languageIndex]);
            delay(800);
            returnToOptions();
          }else{
            lcd.clear();
            lcd.print(messages[9*LANGUAGE_NO + languageIndex]);
            lcd.setCursor(0,1);
            lcd.print(messages[10*LANGUAGE_NO + languageIndex]);
            delay(800);
            lcd.clear();
            lcd.print(messages[5*LANGUAGE_NO + languageIndex]);
            lcd.setCursor(0,1);
            lcd.print(inputAmount);
          }
        }
      }
    }else
    {
      if(amountLen < 6){
        lcd.print(key);
        inputAmount[amountLen] = key;
      }
    }
  }
}
