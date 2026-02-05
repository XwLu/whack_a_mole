#include "display.h"

// 段码表，用于显示0-9的数字
const uint8_t DisplayManager::SEGMENT_MAP[] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

// 构造函数
DisplayManager::DisplayManager(uint8_t clk, uint8_t dio) {
  clkPin = clk;
  dioPin = dio;
  brightness = 0x0F; // 默认亮度
}

// 初始化数码管模块
void DisplayManager::initialize() {
  pinMode(clkPin, OUTPUT);
  pinMode(dioPin, OUTPUT);
  digitalWrite(clkPin, HIGH);
  digitalWrite(dioPin, HIGH);
  
  // 初始化TM1637
  start();
  writeByte(0x40); // 自动地址增加模式
  waitAck();
  stop();
  
  start();
  writeByte(0xC0); // 设置起始地址为0
  waitAck();
  stop();
  
  // 设置亮度
  start();
  writeByte(0x80 | brightness);
  waitAck();
  stop();
  
  clear();
}

// 清屏
void DisplayManager::clear() {
  start();
  writeByte(0x40); // 自动地址增加模式
  waitAck();
  stop();
  
  start();
  writeByte(0xC0); // 设置起始地址为0
  waitAck();
  
  // 发送空白数据
  for (int i = 0; i < 4; i++) {
    writeByte(0x00);
    waitAck();
  }
  
  stop();
}

// 显示分数（0-99）
void DisplayManager::displayScore(int score) {
  // 确保分数不超过99
  if (score > 9999) score = 9999;
  
  // 分解数字到各个位
  uint8_t digits[4];
  digits[0] = score / 1000; // 千位
  digits[1] = (score / 100) % 10; // 百位
  digits[2] = (score / 10) % 10; // 十位
  digits[3] = score % 10; // 个位
  
  // 发送数据到TM1637
  start();
  writeByte(0x40); // 自动地址增加模式
  waitAck();
  stop();
  
  start();
  writeByte(0xC0); // 设置起始地址为0
  waitAck();
  
  // 发送各个位的段码
  for (int i = 0; i < 4; i++) {
    writeByte(SEGMENT_MAP[digits[i]]);
    waitAck();
  }
  
  stop();
  
  // 打开显示
  start();
  writeByte(0x80 | brightness);
  waitAck();
  stop();
}

// 设置亮度
void DisplayManager::setBrightness(uint8_t brightness) {
  this->brightness = brightness & 0x0F;
  start();
  writeByte(0x80 | this->brightness);
  waitAck();
  stop();
}

void DisplayManager::start() {
  digitalWrite(clkPin, HIGH);
  digitalWrite(dioPin, HIGH);
  digitalWrite(dioPin, LOW);
  digitalWrite(clkPin, LOW);
}

void DisplayManager::stop() {
  digitalWrite(clkPin, LOW);
  digitalWrite(dioPin, LOW);
  digitalWrite(clkPin, HIGH);
  digitalWrite(dioPin, HIGH);
}

void DisplayManager::writeByte(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(clkPin, LOW);
    digitalWrite(dioPin, (data & 0x01));
    data >>= 1;
    digitalWrite(clkPin, HIGH);
  }
}

bool DisplayManager::waitAck() {
  bool ack = false;
  
  digitalWrite(clkPin, LOW);
  pinMode(dioPin, INPUT);
  
  // 等待ACK
  for (int i = 0; i < 100; i++) {
    if (digitalRead(dioPin) == LOW) {
      ack = true;
      break;
    }
    delayMicroseconds(1);
  }
  
  digitalWrite(clkPin, HIGH);
  digitalWrite(clkPin, LOW);
  pinMode(dioPin, OUTPUT);
  
  return ack;
}
