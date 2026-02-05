#ifndef MOLE_H
#define MOLE_H

#include <Arduino.h>

// 地鼠管理类
class MoleManager {
public:
  // 构造函数
  MoleManager(int maxMoles, const int* ledPins, const int* buttonPins);
  
  // 初始化地鼠模块
  void initialize();
  
  // 关闭所有地鼠LED
  void turnOffAllLeds();
  
  // 点亮指定地鼠的LED
  void turnOnLed(int moleIndex);
  
  // 熄灭指定地鼠的LED
  void turnOffLed(int moleIndex);
  
  // 检测指定地鼠的按钮是否被按下
  bool isButtonPressed(int moleIndex);
  
  // 检测是否有任意一个地鼠按钮被按下
  bool isAnyButtonPressed();
  
  // 检测是否有3个或更多地鼠按钮被同时按下（复位条件）
  bool checkResetCondition();
  
  // 获取最大地鼠数量
  int getMaxMoles() const;
  
private:
  int _maxMoles;
  const int* _ledPins;
  const int* _buttonPins;
};

#endif // MOLE_H