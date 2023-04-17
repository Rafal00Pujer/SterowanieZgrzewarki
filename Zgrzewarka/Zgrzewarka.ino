#include <U8x8lib.h> // Biblioteka do ekranu.
#include <AbleButtons.h> // Pomocnicza biblioteka do guzikow.
#include <PololuLedStrip.h> // Biblioteka do diody RGB.

// Pomocniczy enumerator reprezentujacy stany Zgrzewarki.
enum EAppState
{
  Settings_Fire_Delay,
  Settings_Fire_Time_1,
  Settings_Fire_Time_2,
  Settings_Fire_Cooldown,

  Fire_Ready,
  Fire_Delay,
  Fire,
  Fire_Cooldown,

  None
};

// Stale
constexpr uint8_t AButtonPin = 3;  // Pin od guzika wyboru/potwierdzenia.
constexpr uint8_t BButtonPin = 4;  // Pin od guzika wstecz.
constexpr uint8_t FButtonPin = 5;  // Pin od guzika aktywacji zgrzewarki.

constexpr uint8_t PotentiometerPin = A0;  // Pin od potencjometra.

constexpr uint8_t LedActivationPin = 7;  // Pin od diody aktywacji zgrzewarki.
constexpr uint8_t RGBLedPin = 8;  // Pin od diody RGB.

// Kolory Diody RGB UWAGA! Bajty kolorow zielonego i czerownego sa zamienione miejscami. ¯\_(ツ)_/¯
const rgb_color FireReadyColor(10, 0, 0);
const rgb_color FireDelayColor(10, 10, 0);
const rgb_color FireColor(0, 10, 0);
const rgb_color FireNotReadyColor(0, 0, 10);

// W sekundach.
constexpr int MinFireDelayTime = 0;
constexpr int MaxFireDelayTime = 10;

// W sekundach.
constexpr int MinFireTimeSeconds = 1;
constexpr int MaxFireTimeSeconds = 10;

// W 0.1 sekund.
constexpr int MinFireTimeMilis = 0;
constexpr int MaxFireTimeMilis = 9;

// W sekundach.
constexpr int MinFireCooldownTime = 0;
constexpr int MaxFireCooldownTime = 10;
// Koniec Stalych

// Zmienne globalne
// Typ wyswietlacza.
U8X8_SSD1306_128X64_NONAME_HW_I2C Display(/* reset=*/U8X8_PIN_NONE);

// Typ guzika.
using Button = AblePullupClickerButton;
using ButtonList = AblePullupClickerButtonList;

Button AButton(AButtonPin);
Button BButton(BButtonPin);
Button FButton(FButtonPin);

Button* Btns[] = { &AButton, &BButton, &FButton };
ButtonList Buttons(Btns);

// Dioda RGB
PololuLedStrip<RGBLedPin> RGBLed;

int PotentiometerValue;

int FireDelayTime;
int FireTimeSeconds;
int FireTimeMilis;
int FireCooldownTime;

EAppState AppState = EAppState::None;

unsigned long Timer = 0;
// Koniec zmiennych globalnych

void setup() 
{
  Serial.begin(9600);

  pinMode(LedActivationPin, OUTPUT);
  digitalWrite(LedActivationPin, LOW);

  RGBLed.write(&FireNotReadyColor, 1);

  Buttons.begin();

  Display.begin();
  Display.setFont(u8x8_font_chroma48medium8_r);
  Display.drawString(0, 0, "ZGRZEWARKA v0.1");

  SwitchState(EAppState::Settings_Fire_Delay);
}

void loop() 
{
  PotentiometerValue = analogRead(PotentiometerPin);

  Buttons.handle();

  HandleState();
}

void HandleState()
{
  switch(AppState)
  {
    case Settings_Fire_Delay:
    {
      HandleSettings_Fire_Delay();
      break;
    }

    case Settings_Fire_Time_1:
    {
      HandleSettings_Fire_Time_1();
      break;
    }

    case Settings_Fire_Time_2:
    {
      HandleSettings_Fire_Time_2();
      break;
    }

    case Settings_Fire_Cooldown:
    {
      HandleSettings_Fire_Cooldown();
      break;
    }

    case Fire_Ready:
    {
      HandleFire_Ready();
      break;
    }

    case Fire_Delay:
    {
      HandleFire_Delay();
      break;
    }

    case Fire:
    {
      HandleFire();
      break;
    }

    case Fire_Cooldown:
    {
      HandleFire_Cooldown();
      break;
    }
  }
}

void SwitchState(EAppState NewState)
{
  AppState = NewState;

  Display.clear();
  Display.drawString(0, 0, "ZGRZEWARKA v0.1");

  Buttons.resetClicked();

  // Uruchom nowy stan.
  switch(AppState)
  {
    case Settings_Fire_Delay:
    {
      Display.drawString(0, 1, "USTAWIENIA");
      Display.drawString(0, 2, "Czas Przed");
      Display.drawString(0, 3, "Strzalem");
      Display.setInverseFont(1);
      Display.setCursor(0, 4);
      Display.print(FireDelayTime / 1000);
      Display.setInverseFont(0);
      Display.print("S");

      RGBLed.write(&FireNotReadyColor, 1);

      break;
    }

    case Settings_Fire_Time_1:
    {
      Display.drawString(0, 1, "USTAWIENIA");
      Display.drawString(0, 2, "Czas");
      Display.drawString(0, 3, "Strzalu");
      Display.setInverseFont(1);
      Display.setCursor(0, 4);
      Display.print(FireTimeSeconds);
      Display.setInverseFont(0);
      Display.print(".");
      Display.print(FireTimeMilis / 100);
      Display.print("S");

      RGBLed.write(&FireNotReadyColor, 1);

      break;
    }

    case Settings_Fire_Time_2:
    {
      Display.drawString(0, 1, "USTAWIENIA");
      Display.drawString(0, 2, "Czas");
      Display.drawString(0, 3, "Strzalu");
      Display.setCursor(0, 4);
      Display.print(FireTimeSeconds);
      Display.print(".");
      Display.setInverseFont(1);
      Display.print(FireTimeMilis / 100);
      Display.setInverseFont(0);
      Display.print("S");

      RGBLed.write(&FireNotReadyColor, 1);

      break;
    }

    case Settings_Fire_Cooldown:
    {
      Display.drawString(0, 1, "USTAWIENIA");
      Display.drawString(0, 2, "Czas Po");
      Display.drawString(0, 3, "Strzale");
      Display.setInverseFont(1);
      Display.setCursor(0, 4);
      Display.print(FireCooldownTime / 1000);
      Display.setInverseFont(0);
      Display.print("S");

      RGBLed.write(&FireNotReadyColor, 1);

      break;
    }

    case Fire_Ready:
    {
      Display.drawString(0, 2, "GOTOWY DO");
      Display.drawString(0, 3, "STRZALU!!!");

      RGBLed.write(&FireReadyColor, 1);

      break;
    }

    case Fire_Delay:
    {
      Display.drawString(0, 2, "!! ZA CHWILE !!");
      Display.drawString(0, 3, "!!! STRZELI !!!");

      Timer = millis();

      RGBLed.write(&FireDelayColor, 1);
      
      break;
    }

    case Fire:
    {
      Display.draw2x2String(0, 3, "STRZELA");

      Timer = millis();

      digitalWrite(LedActivationPin, HIGH);

      RGBLed.write(&FireColor, 1);

      break;
    }

    case Fire_Cooldown:
    {
      Display.drawString(0, 2, "Chlodzenie");
      Display.drawString(0, 3, "XD");

      Timer = millis();

      digitalWrite(LedActivationPin, LOW);

      RGBLed.write(&FireNotReadyColor, 1);

      break;
    }
  }
}

// Stany Zgrzewarki

void HandleSettings_Fire_Delay()
{
  int NewFireDelayTime = map(PotentiometerValue, 100, 923, MinFireDelayTime, MaxFireDelayTime);
  NewFireDelayTime = constrain(NewFireDelayTime, MinFireDelayTime, MaxFireDelayTime) * 1000;

  if (NewFireDelayTime != FireDelayTime)
  {
    FireDelayTime = NewFireDelayTime;

    Display.drawString(0, 4, "                ");
    Display.setInverseFont(1);
    Display.setCursor(0, 4);
    Display.print(FireDelayTime / 1000);
    Display.setInverseFont(0);
    Display.print("S");
  }

  if (AButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Time_1);
  }
}

void HandleSettings_Fire_Time_1()
{
  int NewFireTimeSeconds = map(PotentiometerValue, 100, 923, MinFireTimeSeconds, MaxFireTimeSeconds);
  NewFireTimeSeconds = constrain(NewFireTimeSeconds, MinFireTimeSeconds, MaxFireTimeSeconds);

  if (NewFireTimeSeconds != FireTimeSeconds)
  {
    FireTimeSeconds = NewFireTimeSeconds;

    Display.drawString(0, 4, "                ");
    Display.setInverseFont(1);
    Display.setCursor(0, 4);
    Display.print(FireTimeSeconds);
    Display.setInverseFont(0);
    Display.print(".");
    Display.print(FireTimeMilis / 100);
    Display.print("S");
  }

  if (BButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Delay);
  }

  if (AButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Time_2);
  }
}

void HandleSettings_Fire_Time_2()
{
  int NewFireTimeMilis = map(PotentiometerValue, 100, 923, MinFireTimeMilis, MaxFireTimeMilis);
  NewFireTimeMilis = constrain(NewFireTimeMilis, MinFireTimeMilis, MaxFireTimeMilis) * 100;

  if (NewFireTimeMilis != FireTimeMilis)
  {
    FireTimeMilis = NewFireTimeMilis;

    Display.drawString(0, 4, "                ");
    Display.setCursor(0, 4);
    Display.print(FireTimeSeconds);
    Display.print(".");
    Display.setInverseFont(1);
    Display.print(FireTimeMilis / 100);
    Display.setInverseFont(0);
    Display.print("S");
  }

  if (BButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Time_1);
  }

  if (AButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Cooldown);
  }
}

void HandleSettings_Fire_Cooldown()
{
  int NewFireCooldownTime = map(PotentiometerValue, 100, 923, MinFireCooldownTime, MaxFireCooldownTime);
  NewFireCooldownTime = constrain(NewFireCooldownTime, MinFireCooldownTime, MaxFireCooldownTime) * 1000;

  if (NewFireCooldownTime != FireCooldownTime)
  {
    FireCooldownTime = NewFireCooldownTime;

    Display.drawString(0, 4, "                ");
    Display.setInverseFont(1);
    Display.setCursor(0, 4);
    Display.print(FireCooldownTime / 1000);
    Display.setInverseFont(0);
    Display.print("S");
  }

  if (BButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Time_2);
  }

  if (AButton.resetClicked())
  {
    SwitchState(EAppState::Fire_Ready);
  }
}

void HandleFire_Ready()
{
  if (BButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Delay);
  }

  if (AButton.resetClicked())
  {
    SwitchState(EAppState::Settings_Fire_Delay);
  }

  if (FButton.resetClicked())
  {
    SwitchState(EAppState::Fire_Delay);
  }
}

void HandleFire_Delay()
{
  const unsigned long CurrentMillis = millis();

  if(CurrentMillis - Timer >= FireDelayTime)
  {
    SwitchState(EAppState::Fire);
  }
}

void HandleFire()
{
  const unsigned long CurrentMillis = millis();
  const int FireTime = (FireTimeSeconds * 1000) + FireTimeMilis;

  if(CurrentMillis - Timer >= FireTime)
  {
    SwitchState(EAppState::Fire_Cooldown);
  }
}

void HandleFire_Cooldown()
{
  const unsigned long CurrentMillis = millis();

  if(CurrentMillis - Timer >= FireCooldownTime)
  {
    SwitchState(EAppState::Fire_Ready);
  }
}
// Koniec stanow Zgrzewarki.