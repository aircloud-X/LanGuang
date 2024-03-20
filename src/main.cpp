#include <Arduino.h>
#include <Wire.h>    // 引入Wire库，用于I2C通信
#include <U8g2lib.h> // 引入U8g2库
#include <WiFi.h>
#include <RtcDS1302.h>
#include <time.h>

const char *ssid = "TP-LINK_SWD";
const char *password = "40834083";

ThreeWire myWire(17, 16, 5); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))

char datestring[26];
char *printDateTime(const RtcDateTime &dt)
{
  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  // Serial.print(datestring);
  return datestring;
}

RtcDateTime getTime()
{
  const long gmtOffset_sec = 8 * 3600;
  configTime(gmtOffset_sec, 0, "ntp1.aliyun.com");
  time_t now = time(nullptr);
  while (now < 100000)
  {
    delay(100);
    now = time(nullptr);
  }
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  return RtcDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

// 设置SSD1306 OLED的分辨率和I2C地址
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDRESS 0x78

#define ANALOG_PIN_0 A0 // 定义A0引脚
#define ANALOG_PIN_1 A1 // 定义A1引脚

// 使用U8g2库创建一个屏幕对象
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R2, /* reset=*/U8X8_PIN_NONE);

int detect_AD1[40];
int detect_AD2[40];

int sum_AD1 = 0;
int sum_AD2 = 0;

float average_AD1, average_AD2;

// size_t cycle;

// #define windowsize 10
// #define numReadings 10
// int sensorValues[10];
// int sortedValues[10];
// float calculate_medianValuesum()
// {
//   float output_tmp = 0;
//   int medianValuesum = 0;
//   // 读取传感器值
//   for (size_t j = 0; j < windowsize; j++)
//   {
//     for (int i = 0; i < numReadings; i++)
//     {
//       sensorValues[i] = analogRead(ANALOG_PIN_0);
//       sortedValues[i] = sensorValues[i];
//       delay(1); // 适当延迟以避免干扰
//     }
//     // 对读数进行排序
//     for (int i = 0; i < numReadings - 1; i++)
//     {
//       for (int j = i + 1; j < numReadings; j++)
//       {
//         if (sortedValues[i] > sortedValues[j])
//         {
//           int temp = sortedValues[i];
//           sortedValues[i] = sortedValues[j];
//           sortedValues[j] = temp;
//         }
//       }
//     }
//     // 取中值
//     medianValuesum += sortedValues[numReadings / 2];
//   }
//   output_tmp = (float)medianValuesum / (float)windowsize;
//   return output_tmp;
// }

void setup()
{
  // 初始化I2C总线
  Wire.begin();
  // 初始化屏幕对象
  u8g2.begin();
  // 初始化串口
  Serial.begin(115200);

  // Serial.print("Connecting to ");
  // Serial.println(ssid);

  // WiFi.begin(ssid, password);

  // int attempts = 0;
  // while (WiFi.status() != WL_CONNECTED && attempts < 10)
  // {
  //   delay(500);
  //   Serial.print(".");
  //   attempts++;
  // }

  // if (WiFi.status() == WL_CONNECTED)
  // {
  //   Serial.println("");
  //   Serial.println("WiFi connected");
  //   Serial.println("Clock init");

  //   Rtc.Begin();

  //   if (!Rtc.IsDateTimeValid())
  //   {
  //     Serial.println("RTC lost confidence in the DateTime!");
  //     RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //     Rtc.SetDateTime(compiled);
  //   }

  //   if (Rtc.GetIsWriteProtected())
  //   {
  //     Serial.println("RTC was write protected, enabling writing now");
  //     Rtc.SetIsWriteProtected(false);
  //   }

  //   if (!Rtc.GetIsRunning())
  //   {
  //     Serial.println("RTC was not actively running, starting now");
  //     Rtc.SetIsRunning(true);
  //   }

  //   RtcDateTime now = Rtc.GetDateTime();
  //   if (!now.IsValid())
  //   {
  //     Serial.println("RTC lost confidence in the DateTime!");
  //   }

  //   Rtc.SetDateTime(getTime());

  //   Serial.print("Current time: ");
  //   Serial.println(printDateTime(now));
  // }
  // else
  // {
  //   Serial.println("Failed to connect to WiFi. Check your SSID and password.");
  // }
}

void loop()
{
  // // 读取A0引脚的模拟输入值并打印
  // int analogValue0 = analogRead(ANALOG_PIN_0);
  // Serial.print(">A0 Value:");
  // Serial.println(analogValue0);

  // // 读取A1引脚的模拟输入值并打印
  // int analogValue1 = analogRead(ANALOG_PIN_1);
  // Serial.print(">A1 Value:");
  // Serial.println(analogValue1);
  // 清除屏幕
  u8g2.clearBuffer();

  // 设置字体
  u8g2.setFont(u8g2_font_6x12_tf);
  // pinMode(15, INPUT);
  // int Detect_People = digitalRead(15);
  // if (Detect_People == 1)
  // {
  //   // u8g2.setCursor(0, 10);        // 设置文本位置
  //   // u8g2.print("Detect People!"); // 打印文本
  //   // Serial.print("Detect People!\n");
  // }
  // else
  // {
  //   // u8g2.setCursor(0, 20);    // 设置文本位置
  //   // u8g2.print("NO People!"); // 打印文本
  //   // Serial.print("NO People!\n");
  // }

  for (size_t i = 0; i < 40; i++)
  {
    detect_AD1[i] = analogRead(ANALOG_PIN_0);
    detect_AD2[i] = analogRead(ANALOG_PIN_1);

    sum_AD1 += detect_AD1[i];
    sum_AD2 += detect_AD2[i];
    delay(10);
  }

  average_AD1 = (float)sum_AD1 / 40.0;
  average_AD2 = (float)sum_AD2 / 40.0;

  sum_AD1 = 0;
  sum_AD2 = 0;

  Serial.print(">A0 Value:");
  Serial.println(average_AD1);
  Serial.print(">A1 Value:");
  Serial.println(average_AD2);

  u8g2.setCursor(0, 10);    // 设置文本位置
  u8g2.print("A0 Value: "); // 打印文本
  u8g2.print(average_AD1);  // 打印文本

  u8g2.setCursor(0, 20);    // 设置文本位置
  u8g2.print("A1 Value: "); // 打印文本
  u8g2.print(average_AD2);  // 打印文本

  RtcDateTime now = Rtc.GetDateTime();
  u8g2.setFont(u8g2_font_5x8_mf);
  u8g2.setCursor(0, 30);          // 设置文本位置
  u8g2.print(printDateTime(now)); // 打印文本

  // 刷新显示
  u8g2.sendBuffer();

  // 延迟一段时间
  delay(100);
}

// //可用于修改时间

// #include <U8g2lib.h> // 引入U8g2库

// #include <WiFi.h>
// #include <RtcDS1302.h>
// #include <time.h>

// const char *ssid = "TP-LINK_SWD";
// const char *password = "40834083";

// ThreeWire myWire(17, 16, 5); // IO, SCLK, CE
// RtcDS1302<ThreeWire> Rtc(myWire);

// #define countof(a) (sizeof(a) / sizeof(a[0]))

// void printDateTime(const RtcDateTime &dt)
// {
//   char datestring[26];

//   snprintf_P(datestring,
//              countof(datestring),
//              PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
//              dt.Month(),
//              dt.Day(),
//              dt.Year(),
//              dt.Hour(),
//              dt.Minute(),
//              dt.Second());
//   Serial.print(datestring);
// }

// RtcDateTime getTime()
// {
//   const long gmtOffset_sec = 8 * 3600;
//   configTime(gmtOffset_sec, 0, "ntp1.aliyun.com");
//   time_t now = time(nullptr);
//   while (now < 100000)
//   {
//     delay(100);
//     now = time(nullptr);
//   }
//   struct tm timeinfo;
//   getLocalTime(&timeinfo);
//   return RtcDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
// }

// void setup()
// {
//   Serial.begin(115200);

//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, password);

//   int attempts = 0;
//   while (WiFi.status() != WL_CONNECTED && attempts < 10)
//   {
//     delay(500);
//     Serial.print(".");
//     attempts++;
//   }

//   if (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.println("Failed to connect to WiFi. Check your SSID and password.");
//     while (1)
//       ; // 等待无限循环
//   }

//   Serial.println("");
//   Serial.println("WiFi connected");

//   Rtc.Begin();

//   if (!Rtc.IsDateTimeValid())
//   {
//     Serial.println("RTC lost confidence in the DateTime!");
//     RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
//     Rtc.SetDateTime(compiled);
//   }

//   if (Rtc.GetIsWriteProtected())
//   {
//     Serial.println("RTC was write protected, enabling writing now");
//     Rtc.SetIsWriteProtected(false);
//   }

//   if (!Rtc.GetIsRunning())
//   {
//     Serial.println("RTC was not actively running, starting now");
//     Rtc.SetIsRunning(true);
//   }

//   RtcDateTime now = Rtc.GetDateTime();
//   if (!now.IsValid())
//   {
//     Serial.println("RTC lost confidence in the DateTime!");
//   }

//   Rtc.SetDateTime(getTime());

//   Serial.print("Current time: ");
//   printDateTime(now);
//   Serial.println();
//   // pinMode(14, OUTPUT);
//   // digitalWrite(14, 1);
// }

// void loop()
// {
//   RtcDateTime now = Rtc.GetDateTime();

//   printDateTime(now);
//   Serial.println();

//   if (!now.IsValid())
//   {
//     Serial.println("RTC lost confidence in the DateTime!");
//   }

//   delay(1000); // one second
//   // digitalWrite(14, 0);
//   // delay(1000); // one second
// }
