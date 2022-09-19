# Port TinyMaix to STM32f767

## Chip

| Item         | Parameter     |
| ------------ | ------------- |
| Chip         | STM32F767ZIT6 |
| Arch         | ARM Cortex M7 |
| Freq         | 216M          |
| Flash        | 2048KB        |
| RAM          | 512KB         |
| Acceleration | ARM SIMD      |

## Board

NUCLEO-F767ZI  

<a href="STM32f767ZI.png"><img width=300 src="https://github.com/lemonicesprite/TinyMaix/blob/main/doc/porting/assets/STM32f767ZI.png"/></a>


## Development Environment

system:Ubuntu 22.04.1 LTS
compiler:gcc-arm-none-eabi-7-2018-q2-update
tools: STM32CubeMX 6.6.0 with STM32Cube FW_F7 V1.17.0



#### 使用

```shell
git clone  --recursive  git@github.com:lemonicesprite/stm32f767-tinymaix.git
cd stm32f767-tinymaix
make dl
```

ps: `make dl` 将使自动调用pyocd下载程序



修改`TinyMaix/include/testdemo.h`文件中的`TINYMAIX_DEMOUSED`宏选择使用的demo

```c
#define TINYMAIX_DEMOUSED    DEMO_MNIST


#define DEMO_MNIST          (1)
#define DEMO_CIFAR          (2)
#define DEMO_VWW            (3)
#define DEMO_MBNET          (4)
```





#### 移植

##### 1.使用`stm32cubemx生成初始化代码`

##### 2.重定向printf

在usart.c代码添加以下代码

```
int _write(int fd, char *ptr, int len)
{ 
  return HAL_UART_Transmit(&huart3, (uint8_t *)ptr, len, 0xFFFF); 
}
```

修改Makefile 链接选项添加`-Wl,-u_printf_float`启用浮点数打印支持

```
......
......
......
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -Wl,-u_printf_float
......
......
......
```



##### 3.添加`TinyMaix`

```
git clone git@github.com:sipeed/TinyMaix.git Components/TinyMaix
```

创建自己的配置文件,尽量不改动源码

```
mkdir TinyMaix
cp -r Components/TinyMaix/include TinyMaix/
```

`tm_port.h`文件需根据单片机类型修改，连同include文件夹复制出来进行定制

根据实际情况修改`TinyMaix/includetm_port.h`

移植`TinyMaix/examples/`中的demo



##### 4.添加需要的编译文件至Makefile

```
......
......
......
# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -Wl,-u_printf_float

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


+++ #######################################
+++ # Persionality
+++ #######################################
+++ # TinyMaix sources
+++ TINYMAIX_SOURCES += \
+++ Components/TinyMaix/src/tm_model.c \
+++ Components/TinyMaix/src/tm_layers.c \
+++ Components/TinyMaix/src/tm_stat.c \
+++ Components/TinyMaix/src/tm_layers_fp8.c \
+++ Components/TinyMaix/src/tm_layers_O1.c

+++ # TinyMaix includes
+++ TINYMAIX_INCLUDES += \
+++ -ITinyMaix/include \
+++ -IComponents/TinyMaix/src \
+++ -IComponents/TinyMaix/tools/tmdl \
+++ -IComponents/TinyMaix/examples/cifar10/pic \
+++ -IComponents/TinyMaix/examples/vww/pic/ \
+++ -ITinyMaix/src/mbnet\

+++ TINYMAIX_TEST_SOURCES += \
+++ TinyMaix/src/mnist.c	\
+++ TinyMaix/src/cifar.c	\
+++ TinyMaix/src/vww.c	\
+++ TinyMaix/src/mbnet/label.c \
+++ TinyMaix/src/mbnet/mbnet.c \

+++ C_SOURCES += ${TINYMAIX_SOURCES} ${TINYMAIX_TEST_SOURCES}
+++ CFLAGS += ${TINYMAIX_INCLUDES} 

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
......
......
......
```





#### 测试结果



##### CPU

###### 1.minist demo

```
Start TinyMaix Test...
mnist demo
===tm_run use 3.000 ms
0: 0.000
1: 0.000
2: 0.004
3: 0.004
4: 0.004
5: 0.004
6: 0.000
7: 0.004
8: 0.004
9: 0.992
### Predict output is: Number 9, prob 0.992
```

###### 2.cifar demo

```
Start TinyMaix Test...
cifar demo
===tm_run use 282.000 ms
0: 0.004
1: 0.000
2: 0.980
3: 0.004
4: 0.016
5: 0.004
6: 0.004
7: 0.004
8: 0.004
9: 0.000
### Predict output is: Class 2, bird, prob 0.980
```

###### 3.vww demo

```
Start TinyMaix Test...
vww demo
===tm_run use 890.000 ms
0: 0.031
1: 0.969
### Predict output is: Class 1, person, prob 0.969
```

###### 4.mbnet demo

```
### Predict output is: Class 1, person, prob 0.969
Start TinyMaix Test...
mbnet demo
===tm_run use 1411.000 ms

### Predict output is: Class 292 (tiger, Panthera tigris), Prob 0.793
```



##### SIMD

###### 1.minist demo

```
Start TinyMaix Test...
mnist demo
===tm_run use 3.000 ms
0: 0.000
1: 0.000
2: 0.004
3: 0.004
4: 0.004
5: 0.004
6: 0.000
7: 0.004
8: 0.004
9: 0.992
### Predict output is: Number 9, prob 0.992
```

###### 2.cifar demo

```
Start TinyMaix Test...
cifar demo
===tm_run use 185.000 ms
0: 0.004
1: 0.000
2: 0.980
3: 0.004
4: 0.016
5: 0.004
6: 0.004
7: 0.004
8: 0.004
9: 0.000
### Predict output is: Class 2, bird, prob 0.980
```

###### 3.vww demo

```
Start TinyMaix Test...
vww demo
===tm_run use 640.000 ms
0: 0.031
1: 0.969
### Predict output is: Class 1, person, prob 0.969
```

###### 4.mbnet demo

```
Start TinyMaix Test...
mbnet demo
===tm_run use 869.000 ms

### Predict output is: Class 292 (tiger, Panthera tigris), Prob 0.793
```

