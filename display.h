#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

// TM1637数码管管理类
class DisplayManager {
private:
  uint8_t clkPin;
  uint8_t dioPin;
  uint8_t brightness;
  
  void start();
  void stop();
  void writeByte(uint8_t data);
  bool waitAck();
  
  // 段码表，用于显示0-9的数字
  static const uint8_t SEGMENT_MAP[];

public:
  // 构造函数
  DisplayManager(uint8_t clk, uint8_t dio);
  
  // 初始化数码管模块
  void initialize();
  
  // 清屏
  void clear();
  
  // 显示分数（0-99）
  void displayScore(int score);
  
  // 设置亮度
  void setBrightness(uint8_t brightness);
};

#endif // DISPLAY_H