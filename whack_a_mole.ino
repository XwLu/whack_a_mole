// Arduino打地鼠游戏
// 基于guide.md的需求开发
// 使用模块化设计，分离地鼠和数码管功能

// 引入自定义模块
#include "mole.h"
#include "display.h"

// ============== 配置参数 ==============
const int MAX_MOLES = 5;              // 最大地鼠数量
const unsigned long GAME_DURATION = 90000;       // 游戏持续时间（毫秒）
const unsigned long INITIAL_MOLE_TIME = 4000;    // 初始地鼠显示时间（毫秒）
const unsigned long MIN_MOLE_TIME = 2000;        // 最小地鼠显示时间（毫秒）
const unsigned long TIME_REDUCTION_PERIOD = 70000; // 时间缩短周期（毫秒，30秒内从初始时间缩短到最小时间）

// TM1637数码管配置
const int DISPLAY_CLK_PIN = 6;  // TM1637 CLK引脚
const int DISPLAY_DIO_PIN = 7;  // TM1637 DIO引脚

// 地鼠按钮和LED引脚定义
// 每个地鼠使用两个独立引脚：一个用于LED控制，一个用于按钮检测
const int moleLedPins[MAX_MOLES] = {12, A0, A1, A2, 2};    // LED控制引脚（输出）
const int moleButtonPins[MAX_MOLES] = {13, A3, 4, 5, 3};  // 按钮引脚（输入）

// ============== 全局变量 ==============
int score = 0;
unsigned long gameStartTime = 0;
unsigned long currentMoleStartTime = 0;
unsigned long currentMoleDisplayTime = INITIAL_MOLE_TIME;  // 当前地鼠应该显示的时间
int currentMoleIndex = -1;
bool gameRunning = false;

// 创建模块实例
MoleManager moleManager(MAX_MOLES, moleLedPins, moleButtonPins);
DisplayManager displayManager(DISPLAY_CLK_PIN, DISPLAY_DIO_PIN);

// ============== 辅助函数 ==============
// 计算当前地鼠应该显示的时间（动态变化）
unsigned long calculateMoleDisplayTime() {
  if (!gameRunning) {
    return INITIAL_MOLE_TIME;  // 游戏未运行时返回初始时间
  }
  
  unsigned long gameTimeElapsed = millis() - gameStartTime;
  
  // 如果游戏时间超过30秒，返回最小显示时间
  if (gameTimeElapsed >= TIME_REDUCTION_PERIOD) {
    return MIN_MOLE_TIME;
  }
  
  // 计算当前应该的显示时间（线性减少）
  // 时间变化范围：INITIAL_MOLE_TIME -> MIN_MOLE_TIME
  // 时间在TIME_REDUCTION_PERIOD内线性变化
  unsigned long timeRange = INITIAL_MOLE_TIME - MIN_MOLE_TIME;
  unsigned long currentTimeReduction = (timeRange * gameTimeElapsed) / TIME_REDUCTION_PERIOD;
  unsigned long currentMoleTime = INITIAL_MOLE_TIME - currentTimeReduction;
  
  // 确保时间不小于最小值
  return max(currentMoleTime, MIN_MOLE_TIME);
}

// 数码管测试函数：从5555倒计时显示到0000
void displayCountdown() {
  for (int i = 5555; i >= 0; i -= 1111) {
    displayManager.displayScore(i);
    delay(1000);  // 每个数字显示100毫秒
  }
  // 测试完成后显示初始分数0
  displayManager.displayScore(0);
  delay(500);  // 停留在0显示500毫秒
}

// ============== 游戏控制函数 ==============
void resetGame() {
  score = 0;
  gameRunning = false;
  
  // 关闭所有地鼠LED
  moleManager.turnOffAllLeds();
  
  // 显示初始分数
  displayManager.displayScore(0);
  
  // 打印复位信息
  Serial.println("Ready");
  
  // 不再自动开始游戏，等待玩家按下任意按钮
  delay(3000);
}

void endGame() {
  gameRunning = false;
  
  // 关闭所有地鼠LED
  moleManager.turnOffAllLeds();
  
  // 持续显示最终分数
  displayManager.displayScore(score);
  
  // 打印游戏结束信息
  Serial.print("Over ");
  Serial.println(score);
}

void showNextMole() {
  // 关闭当前地鼠的LED
  if (currentMoleIndex >= 0 && currentMoleIndex < MAX_MOLES) {
    moleManager.turnOffLed(currentMoleIndex);
  }
  
  // 随机选择下一个地鼠，确保与当前不同
  int newMoleIndex;
  do {
    newMoleIndex = random(MAX_MOLES);
  } while (newMoleIndex == currentMoleIndex && MAX_MOLES > 1); // 当只有一个地鼠时跳过检查
  currentMoleIndex = newMoleIndex;
  
  // 打印地鼠刷新信息
  Serial.print("new mole show ");
  Serial.println(currentMoleIndex + 1);  // 灯的编号从1开始
  
  // 计算当前地鼠应该显示的时间
  currentMoleDisplayTime = calculateMoleDisplayTime();
  
  // 点亮新地鼠的LED
  moleManager.turnOnLed(currentMoleIndex);
  
  // 记录地鼠出现时间
  currentMoleStartTime = millis();
}

void checkMoleButtons() {
  // 检查当前活跃的地鼠按钮
  if (currentMoleIndex >= 0 && currentMoleIndex < MAX_MOLES) {
    // 直接读取对应地鼠的按钮引脚状态
    bool buttonPressed = moleManager.isButtonPressed(currentMoleIndex);
    
    if (buttonPressed) {
      // 玩家成功打到地鼠
      score++;
      displayManager.displayScore(score);
      
      // 打印击中信息
      Serial.print("total hit ");
      Serial.println(score);
      
      // 关闭当前地鼠LED
      moleManager.turnOffLed(currentMoleIndex);
      
      // 成功击中后延迟0.5秒
      delay(500);
      
      // 显示下一个地鼠
      showNextMole();
    }
  }
}

// ============== 初始化函数 ==============
void setup() {
  // 初始化串口通信
  Serial.begin(9600);
  
  // 初始化地鼠模块
  moleManager.initialize();
  
  // 初始化数码管模块
  displayManager.initialize();
  
  // 显示初始分数
  displayManager.displayScore(0);
  
  // 初始化随机数生成器
  randomSeed(analogRead(A5));  // 使用未连接的模拟引脚作为随机源

  Serial.println("Inited");
}

// ============== 主循环 ==============
void loop() {
  // 检查是否有3个或更多地鼠按钮被同时按下（复位功能）
  if (moleManager.checkResetCondition()) {
    resetGame();
    delay(200);  // 消抖
  }
  
  // 游戏未运行时（等待开始）
  if (!gameRunning) {
    // 检查是否有任意按钮按下以开始游戏
    if (moleManager.isAnyButtonPressed()) {
      // 打印游戏开始信息
      Serial.println("Go");
      // 延迟0.5秒开始游戏
      delay(500);
      // 开始游戏
      gameStartTime = millis();
      gameRunning = true;
      // 测试数码管：从5555倒计时显示到0000
      displayCountdown();
      showNextMole();
    }
  }
  // 游戏运行中
  else {
    // 检查游戏时间是否结束
    if (millis() - gameStartTime > GAME_DURATION) {
      endGame();
      return;
    }
    
    // 检查当前地鼠是否超时
    if (millis() - currentMoleStartTime > currentMoleDisplayTime) {
      showNextMole();
    }
    
    // 检查地鼠按钮
    checkMoleButtons();
  }
}
