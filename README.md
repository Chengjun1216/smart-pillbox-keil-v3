# 智能药盒 Keil 工程 V3.0

这是从原始仓库整理出来的 Keil 工程备份，适合直接在 Keil MDK 中打开和编译。

## 目录结构

- `SmartPillbox.uvprojx`：Keil 工程文件
- `SmartPillbox.uvoptx`：调试选项文件
- `SmartPillbox.uvguix.M`：视图配置
- `main.c` / `main.h`：主逻辑
- `oled.c` / `oled.h`：OLED 显示
- `dht11.c` / `dht11.h`：温湿度采集
- `ds1302.c` / `ds1302.h`：RTC
- `jr6001.c` / `jr6001.h`：语音模块接口
- `key.c` / `key.h`：按键扫描
- `ws2812.c` / `ws2812.h`：WS2812 / 蜂鸣器控制
- `README.md`：说明文档

## 目标 MCU

- STM32F103C8T6
- Keil MDK / STM32F1 标准库

## 说明

这个工程是从 `smart_pillbox_v3/smart_pillbox3` 整理而来。原项目已包含一份 Keil 工程配置，适合直接在本地 MDK 环境使用。

如果你本地使用的是 Keil 5/AC6，建议确保安装了：
- STM32F10x Standard Peripherals Library
- CMSIS / Device headers
- 目标芯片正确配置为 `STM32F103C8`

## 编译建议

1. 打开 `SmartPillbox.uvprojx`
2. 选择编译器：ARM Compiler 6 或 MDK 默认
3. 配置宏定义：
   - `USE_STDPERIPH_DRIVER`
   - `STM32F10X_MD`
4. 编译并下载到芯片

## 注意

- DS1302 需要外接纽扣电池以保持时间
- DHT11 需确认是否带上拉电阻
- OLED 供电必须使用 3.3V
- JR6001 语音模块的硬件未完全验证，代码保留但需按实际板子检查
