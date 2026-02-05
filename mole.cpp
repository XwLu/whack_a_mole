#include "mole.h"

// 构造函数
MoleManager::MoleManager(int maxMoles, const int* ledPins, const int* buttonPins) {
  _maxMoles = maxMoles;
  _ledPins = ledPins;
  _buttonPins = buttonPins;
}

// 初始化地鼠模块
void MoleManager::initialize() {
  // 初始化地鼠LED引脚（输出）
  for (int i = 0; i < _maxMoles; i++) {
    pinMode(_ledPins[i], OUTPUT);
    digitalWrite(_ledPins[i], LOW);  // 初始关闭LED
  }
  
  // 初始化地鼠按钮引脚（输入）
  for (int i = 0; i < _maxMoles; i++) {
    pinMode(_buttonPins[i], INPUT_PULLUP);
  }
}

// 关闭所有地鼠LED
void MoleManager::turnOffAllLeds() {
  for (int i = 0; i < _maxMoles; i++) {
    digitalWrite(_ledPins[i], LOW);
  }
}

// 点亮指定地鼠的LED
void MoleManager::turnOnLed(int moleIndex) {
  if (moleIndex >= 0 && moleIndex < _maxMoles) {
    digitalWrite(_ledPins[moleIndex], HIGH);
  }
}

// 熄灭指定地鼠的LED
void MoleManager::turnOffLed(int moleIndex) {
  if (moleIndex >= 0 && moleIndex < _maxMoles) {
    digitalWrite(_ledPins[moleIndex], LOW);
  }
}

// 检测指定地鼠的按钮是否被按下
bool MoleManager::isButtonPressed(int moleIndex) {
  if (moleIndex >= 0 && moleIndex < _maxMoles) {
    bool pressed = digitalRead(_buttonPins[moleIndex]) == LOW;
    if (pressed) {
      Serial.print("button ");
      Serial.print(moleIndex + 1);  // 按钮编号从1开始
      Serial.println(" pressed");
      delay(200);  // 去抖
    }
    return pressed;
  }
  return false;
}

// 检测是否有任意一个地鼠按钮被按下
bool MoleManager::isAnyButtonPressed() {
  for (int i = 0; i < _maxMoles; i++) {
    if (isButtonPressed(i)) {
      return true;
    }
  }
  return false;
}

// 检测是否有3个或更多地鼠按钮被同时按下（复位条件）
bool MoleManager::checkResetCondition() {
  int pressedCount = 0;
  
  // 统计被按下的按钮数量
  for (int i = 0; i < _maxMoles; i++) {
    if (isButtonPressed(i)) {
      pressedCount++;
      
      // 优化：如果已经检测到3个按下的按钮，提前返回
      if (pressedCount >= 2) {
        return true;
      }
    }
  }
  
  return pressedCount >= 3;
}

// 获取最大地鼠数量
int MoleManager::getMaxMoles() const {
  return _maxMoles;
}
