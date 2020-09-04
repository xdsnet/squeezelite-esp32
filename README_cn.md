# Squeezelite-esp32 系列翻译评说之一
拟用多篇翻译为主体的文章介绍一个好玩的基于ESP32的音频应用项目，这是第一篇。
本内容主要引自 [Squeezelite-esp32](https://github.com/sle118/squeezelite-esp32)
是利用ESP32实现无线网络无头音频客户端的，需要配合[罗技Logitech Media Server](http://wiki.slimdevices.com/index.php/Logitech_Media_Server) 来进行播放
本篇主要是翻译[Squeezelite-esp32](https://github.com/sle118/squeezelite-esp32)的Readme文档（文档提取日期2020.9.4），也加入了少量的个人理解与评说，欢迎大家交流。

## 支持的硬件
### SqueezeAMP
本模块可以支持[SqueezeAMP](https://forums.slimdevices.com/showthread.php?110926-pre-ANNOUNCE-SqueezeAMP-and-SqueezeliteESP32) ,它还有一个[github站点](https://github.com/philippe44/SqueezeAMP).

如果你想从新编译，需要使用 `squeezelite-esp32-SqueezeAmp-sdkconfig.defaults`配置文件。

***注意***: 你可以使用预编译的固件`SqueezeAMP4MBFlash`/`SqueezeAMP8MBFlash`,它们已经预设好了硬件IO。你也可以使用通用的`I2S4MBFlash`预编译固件，这时应该按下面例子设置`NVS`参数，以获得完全相同的效果：
- set_GPIO（设置GPIO）: 12=green,13=red,34=jack,2=spkfault
- batt_config（）: channel=7,scale=20.24
- dac_config（DAC配置）: model=TAS57xx,bck=33,ws=25,do=32,sda=27,scl=26,mute=14:0
- spdif_config（）: bck=33,ws=25,do=15

### ESP32-A1S
它也可以工作在[ESP32-A1S](https://docs.ai-thinker.com/esp32-a1s)上，这个模块已经包含了内置的音频解码器和耳机输出。不过如果你想直接连接扬声器，则仍需要一个外部放大器。 

这时模块具有如下的IO设置：
- amplifier（放大器输出）: GPIO21
- key2: GPIO13, key3: GPIO19, key4: GPIO23, key5: GPIO18, key6: GPIO5 (用于dip选择开关来确认)
- key1: 不确定，通常是GPIO36
- jack insertion（插孔插入状态）: GPIO39 (插入后为低)
- LED: GPIO22 (低电平有效) ( **注意**，GPIO需要上拉 )

所以一个可能的配置如下：
- set_GPIO（设置GPIO）: 21=amp,22=green:0,39=jack:0
- dac_config（DAC配置）: model=AC101,bck=27,ws=26,do=25,di=35,sda=33,scl=32
- 一个按钮映射: 
```
[
 {"gpio":5,"normal":{"pressed":"ACTRLS_TOGGLE"}},
 {"gpio":18,"pull":true,"shifter_gpio":5,"normal":{"pressed":"ACTRLS_VOLUP"},"shifted":{"pressed":"ACTRLS_NEXT"}},
 {"gpio":23,"pull":true,"shifter_gpio":5,"normal":{"pressed":"ACTRLS_VOLDOWN"},"shifted":{"pressed":"ACTRLS_PREV"}}
]
```
### T-WATCH2020 by LilyGo
它是一个基于ESP32的智能手表[智能手表](http://www.lilygo.cn/prod_view.aspx?TypeId=50036&Id=1290&FId=t3:50036:3) 。它有一个240x240 ST7789屏幕，以及板上音频，它听任何东西效果都不怎么样，不过确实能工作。这里是一个设备的例子，它需要一组I2C的设置用于DAC（见下）。如果你需要重新编译，则需要一个特殊的编译选项，否则I2S默认选项将使用下面的参数。

- dac_config（DAC配置）: model=I2S,bck=26,ws=25,do=33,i2c=106,sda=21,scl=22
- dac_controlset（DAC控制设置）: { "init": [ {"reg":41, "val":128}, {"reg":18, "val":255} ], "poweron": [ {"reg":18, "val":64, "mode":"or"} ], "poweroff": [ {"reg":18, "val":191, "mode":"and"} ] }
- spi_config（SPI配置）: dc=27,data=19,clk=18
- display_config（显示配置）: SPI,driver=ST7789,width=240,height=240,cs=5,back=12,speed=16000000,HFlip,VFlip
### ESP32-WROVER + I2S DAC
`Squeezelite-esp32`需要一个至少有4MB PSRAM的 esp32芯片， ESP32-WROVER符合这个需求。不过为了音频输出，还需要一个I2S的DAC模块。廉价的PCM5102 I2S DACs就能满足要求。PCM5012 DACs可以通过下面的连接完成工作:
```
PCM5102 I2S --- ESP32-WROVER  
VCC ------------- 3.3V  
3.3V------------- 3.3V  
GND ------------- GND  
FLT ------------- GND  
DMP ------------- GND  
SCL ------------- GND  
BCK ------------- (BCK - 见下文说明)  
DIN ------------- (DO - 见下文说明)  
LCK ------------- (WS - 见下文说明)
FMT ------------- GND  
XMT ------------- 3.3V 
```

使用 `squeezelite-esp32-I2S-4MFlash-sdkconfig.defaults`配置文件来编译。

### SqueezeAmpToo !

这是一个超级酷的项目开源音频硬件项目，具体见[SqueezeAmpToo](https://github.com/rochuck/squeeze-amp-too)

## 配置说明(对编译相关的)
要访问`NVS`,请在webUI中转到`credits`,并选择"shows nvs editor"，来进入`NVS`编辑页，修改`NFS`参数。在下面的语法描述中\<\>表示一个值，\[\] 则描述一个可行参数。 

### I2C
`NVS`参数"i2c_config"用来配置常规用途的i2c对应gpio（比如显示相关的）。如果对应留空（没有配置），则表示禁用相应I2C功能的使用。**注意**，在[SqueezeAMP](https://forums.slimdevices.com/showthread.php?110926-pre-ANNOUNCE-SqueezeAMP-and-SqueezeliteESP32)硬件下，端口(`port`)必须为1。默认速度为400000，不过有些显示器可以到800000或者更多。语法为：
```
sda=<gpio>,scl=<gpio>[,port=0|1][,speed=<speed>]
```
### SPI
`NVS`参数"spi_config"用来配置常规用途的spi对应gpio（比如显示相关的）。 如果对应留空（没有配置），则表示禁用相应SPI功能的使用。对于显示来说，`DC`参数是必须的。语法为：
```
data=<gpio>,clk=<gpio>[,dc=<gpio>][,host=1|2]
``` 
### DAC/I2S
`NVS`参数"dac_config"设置用于与DAC连接的I2S通信用GPIO。你可以在编译时采用默认值，但如果设置了`NVS`相应参数，则`NVS`相应参数优先,除非`SqueezeAMP`和`A1S`模块在运行时强制使用（对应默认值）。如果你的DAC也需要I2C，则必须要重新编译。相应的配置语法为：
```
bck=<gpio>,ws=<gpio>,do=<gpio>[,mute=<gpio>[:0|1][,model=TAS57xx|TAS5713|AC101|I2S][,sda=<gpio>,scl=gpio[,i2c=<addr>]]
```
如果未设置"model"，或者对应值不是已经适配的（上述列表中单列出，可以直接识别的），则使用默认的"I2S"作为默认模式。I2C参数是可选的，仅当你的DAC需要I2C控制时 (参考下面的'dac_controlset'说明部分)才进行设置。**注意**"i2c"参数是采用的十进制，而不是十六进制数表示。

参数"dac_controlset"允许通过I2C发送简单的命令，以便使用JSON语法来初始化、开机和关机：
```
{ init: [ {"reg":<register>,"val":<value>,"mode":<nothing>|"or"|"and"}, ... {{"reg":<register>,"val":<value>,"mode":<nothing>|"or"|"and"} ],
  poweron: [ {"reg":<register>,"val":<value>,"mode":<nothing>|"or"|"and"}, ... {{"reg":<register>,"val":<value>,"mode":<nothing>|"or"|"and"} ],
  poweroff: [ {"reg":<register>,"val":<value>,"mode":<nothing>|"or"|"and"}, ... {{"reg":<register>,"val":<value>,"mode":<nothing>|"or"|"and"} ] }
```
这是标准的JSON表示法，如果你不熟悉，搜索引擎是您的好朋友。**请注意**'...'表示你可以有任意多的条目，它不是语法的一部分。每个部分都是可选的，但仅在'dac_config'中配置，而不在这里设置，则任何内容都失去意义。参数'mode' 允许使用*or*或者 *and*来注册多个。如果仅仅是简单的写操作（输出音频），就不要设置'mode'。 **注意，所有的值都是十进制表示**。你也可以使用[jsonlint](https://jsonlint.com)来验证你的JSON语法是否正确。

**注意**：对于众所周知的配置，这里省略了。
### SPDIF
`NVS`参数"spdif_config"设置用来配置SPDIF接口所需的I2S对应GPIO。

SPDIF是一种非标准方式重用I2S接口的方法，因此尽管只需要一个输出引脚(DO),但控制器必须完整的初始化，因此也必须设置时钟(bck)和字节时钟(ws)。由于i2s和SPDIF是互斥的（功能），如果硬件允许，可以重用相同的IO。

您可以在编译时定义默认值，但nvs参数优先，除非`SqueezeAMP`在运行时强制使用这些参数。

如果留空（没有配置），则表示禁用相应SPDIF功能的使用。你也可以在编译前用"make menuconfig"来定义它们。语法是：
```
bck=<gpio>,ws=<gpio>,do=<gpio>
```
**注意**：对于众所周知的配置，这里省略了。

### Display（显示相关,含适配屏幕介绍）
`NVS`参数"display_config"用来配置可选的显示参数，语法为：
```
I2C,width=<pixels>,height=<pixels>[address=<i2c_address>][,HFlip][,VFlip][driver=SSD1306|SSD1326[:1|4]|SSD1327|SH1106]
SPI,width=<pixels>,height=<pixels>,cs=<gpio>[,back=<gpio>][,speed=<speed>][,HFlip][,VFlip][driver=SSD1306|SSD1322|SSD1326[:1|4]|SSD1327|SH1106|SSD1675|ST7735|ST7789[,rotate]]
```
- back:对于一些旧款设备（ST7735）的LED背光设置，它通过PWM来控制亮度
- VFlip 和 HFlip 是可选的，用来改变显示方向
- rotate:对于非矩形显示设备，移动到纵向模式，**注意**这时宽度和高度必须颠倒
- 默认速度是8000000 (8MHz)，不过有的SPI能工作到26MHz甚至40MHz
- SH1106是128x64分辨率单色I2C/SPI [参考这里]((https://www.waveshare.com/wiki/1.3inch_OLED_HAT))
- SSD1306是128x32分辨率单色I2C/SPI [参考这里](https://www.buydisplay.com/i2c-blue-0-91-inch-oled-display-module-128x32-arduino-raspberry-pi)
- SSD1322是128x128分辨率16级灰度SPI [参考这里](https://www.amazon.com/gp/product/B079N1LLG8/ref=ox_sc_act_title_1?smid=A1N6DLY3NQK2VM&psc=1) - 它常以96x96分辨率工作在VU表或者频谱展示
- SSD1351是128x128分辨率 65k/262k 彩色SPI [参考这里](https://www.waveshare.com/product/displays/lcd-oled/lcd-oled-3/1.5inch-rgb-oled-module.htm)
- SSD1326是256x32分辨率 单色或16级灰度SPI [参考这里](https://www.aliexpress.com/item/32833603664.html?spm=a2g0o.productlist.0.0.2d19776cyQvsBi&algo_pvid=c7a3db92-e019-4095-8a28-dfdf0a087f98&algo_expid=c7a3db92-e019-4095-8a28-dfdf0a087f98-1&btsid=0ab6f81e15955375483301352e4208&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
- SSD1327是256x64分辨率16级灰度多尺寸SPI[参考这里](https://www.buydisplay.com/oled-display/oled-display-module?resolution=159) - 它非常好
- SSD1675是电子墨水（e-ink）屏，是实验性的，因为 因为电子墨水的刷新率很低，不适合LMS持续输出显示
- ST7735是128x160分辨率 65k彩色SPI [参考这里](https://www.waveshare.com/product/displays/lcd-oled/lcd-oled-3/1.8inch-lcd-module.htm). 它需要背光控制
- ST7789是 240x320分辨率 65k (262k未启用) 彩色SPI [参考这里](https://www.waveshare.com/product/displays/lcd-oled/lcd-oled-3/2inch-lcd-module.htm). 它也有240x240显示器。请参见**旋转**以在纵向模式下使用
  
要使用LMS上的显示，请添加[项目文件](https://raw.githubusercontent.com/sle118/squezelite-esp32/master/plugin/repo.xml)。然后，你就可以调整频谱分析仪的尺寸了。你也可以安装优秀的插件"Music Information Screen"，这是非常有用的布局。

`NVS`参数"metadata_config"设置AirPlay和Bluetooth的元数据显示方式。语法是
```
[format=<display_content>][,speed=<speed>][,pause=<pause>]
```
- 'speed'是以毫秒为单位的滚动速度（默认值为33ms）

- 'pause'滚动之间的暂停时间（ms）（默认值为3600ms）

- 'format'可以包含自由文本和3个关键字`%artist%`、`%album%`、`%title%`中的任何一个（或者多个）。使用该格式字符串，关键字被替换为它们的值来构建要显示的字符串。**请注意**，在播放曲目时，关键字后面的纯文本将被删除。例如，如果已设置`format=%artist%-%title%`，并且元数据中没有艺术家(artist)，则只显示`<title>`，而不显示`-<title>`。

### Infrared（红外遥控相关）
您可以使用任何兼容NEC协议（38KHz）的红外接收器。`Vcc`，`GND`和`output`是唯一需要连接的引脚，没有上拉，没有滤波电容，这是一个直接的连接。

IR代码按“原样”发送到LMS，因此只有兼容Boom、Classic或Touch的Logitech SB才能工作。我觉得这个`Slim_Devices_Remote.ir`在“服务器”目录下的LMS可以修改以适应其他代码，但我没有尝试过。

在`AirPlay`和`Bluetooth`模式下，仅支持这些本地远程设备，我没有添加选项来创建您自己的映射

请参阅下面的“设置GPIO”以设置与红外接收器关联的GPIO（在选项“ir”中）。

### Set GPIO(设置GPIO)
参数“set_GPIO”用于将GPIO分配给各种函数（功能）。

GPIO可以在引导时设置为GND或Vcc。这对于从哪些功耗小于40毫安的外部设备来说很方便。不过**请小心**，因为在更改哪个GPIO时并没有进行冲突检查，因此可能会损坏您的板，此处还可能出现GPIO创建冲突。

 \<amp\> 参数用来将播放开始时设置为活动级别（默认1）的GPIO（可以用来表示在播状态,或者控制外部放大器是否工作）。当squeezelite闲置（没有播放时）将重置。在squezelite命令行上设置可以通过-C \<timeout\>设置空闲超时时间

如果您有支持插入的音频插孔（使用：0或：1设置插入时的电平），则可以指定它连接到哪个GPIO。使用参数`jack_mutes_amp`允许在插入耳机（例如）时使放大器静音。

可以设置为绿色和红色状态led以及它们各自的活动状态（：0或：1）

\<ir\>参数设置与ir接收器关联的GPIO。无需添加上拉器或电容器

语法是：
```
<gpio>=Vcc|GND|amp[:1|0]|ir|jack[:0|1]|green[:0|1]|red[:0|1]|spkfault[:0|1][,<repeated sequence for next GPIO>]
```
您可以在编译时定义jack、spk故障指示灯的默认值，但除了在运行时强制使用的部分众所周知的配置，则配置了nvs参数将优先使用（覆盖默认值）。

### LED 
有关如何设置绿色和红色LED，请参见§**set_GPIO** 。此外，还可以使用“led亮度”参数来控制它们的亮度。语法是
```
[green=0..100][,red=0..100]
```
**注意**: 对于众所周知的配置，这里省略了。
### Rotary Encoder（旋转编码器，实现左右声道偏置放大比例）
旋转编码器是支持的，可以支持正交移位与压力。这样的编码器通常有2个脚编码器（A和B），和公共C，必须设置为GND和一个可选的SW开关引脚为输出。在ESP32对应的A、B和SW脚上你可以上拉（接上拉电阻）。最好对A和B脚进行一点过滤（比如接~470nF电容），这有助于消除抖动，这不是由软件实现的（即硬件消抖）。

编码器通常是硬编码的，分别旋钮左，右，以及基于LMS的音量调节与播放切换，它在BT（蓝牙）和AirPlay也是可用的。使用“音量”选项时，可以使其始终硬编码为音量降低/增大/播放切换（即使在LMS中也是如此）。它还支持 'longpress'长按选项模式，在长按时，允许备用按键效果，比如left可以对应前一首，right对应下一首，按SW键对应切换。每长按一次开关，模式之间就会交替（主模式的实际行为取决于'volume'（音量模式））。

可以使用'knobonly'选项（与'volume'和'longpress'一起使用）。这种模式试图提供一个单旋钮全导航，这是有点混乱，由于LMS用户界面的原则。左、右、按都遵守LMS的导航规则，尤其是在音乐库中导航时，都会转到下一个子菜单项。由于没有“播放”、“后退”或“暂停”按钮，这会带来挑战。解决方法如下：
- longpress 是 'Play'，即长按时播放
- 双击是'Back' (Left in LMS's 术语中的左键按下). 即双击是后退
- 快速左右交替按下是 'Pause'，即交替按下是暂停

双击（或左右）的速度可以使用可选参数“knobonly”来设置。这不是一个完美的解决方案，其他想法也值得欢迎。请注意，设置的双击速度越长，界面的响应就越慢。原因是我需要等待延迟，然后再决定是单次还是双击。它还可以使菜单导航显得响应缓慢，感觉是反应迟钝。

使用参数`rotary_config`，语法如下：

```
A=<gpio>,B=<gpio>[,SW=gpio>[[,knobonly[=<ms>]|[,volume][,longpress]]
```
**硬件兼容注意**：所有用于旋转编码器的gpio都有内部上拉，所以通常不需要向编码器提供Vcc。不过，如果你使用的编码器板也有自己的上拉，且比ESP32的强（很可能是这样），那么gpio之间会有串扰，所以你必须带Vcc。看看你的电路板原理图，你会明白这些电路板上拉会在任何其他引脚接地时产生一个实际的下拉。

另请参阅"Buttons"(按钮)部分的**重要注意事项**，并记住，当 'lms_ctrls_raw'模式时（见下文），这些旋钮、音量、长按选项均不适用，原始按钮代码（非操作）只会发送到lms。

### Buttons
按钮使用JSON字符串进行描述，语法如下 
```
[
{"gpio":<num>,		
 "type":"BUTTON_LOW | BUTTON_HIGH",	
 "pull":[true|false],
 "long_press":<ms>, 
 "debounce":<ms>,
 "shifter_gpio":<-1|num>,
 "normal": {"pressed":"<action>","released":"<action>"},
 "longpress": { <same> },
 "shifted": { <same> },
 "longshifted": { <same> },
 },
 { ... },
 { ... },
] 
```
其中（除gpio外，所有参数均为可选参数） 
 - "type": (默认BUTTON_LOW)按下按钮时的逻辑电平
 - "pull": (默认false) 激活内部上拉/下拉
 - "long_press": (默认0) 检测长按对应按键持续时间（ms），0表示禁用
 - "debounce": (默认0)去抖动持续时间（ms）（0=内部默认值50ms）
 - "shifter_gpio": (默认-1) 可以一起按下以创建“shift”的另一个按钮的gpio编号。设置为-1以禁用换档器
 - "normal": ({"pressed":"ACTRLS_NONE","released":"ACTRLS_NONE"})按下/释放按钮时要执行的操作（见下文）
 - "longpress": 长时间按下/释放按钮时采取的操作(见`above`/`below`)
 - "shifted": 按下/释放shift按钮时所采取的行动(见`above`/`below`)
 - "longshifted": 长时间按下/释放shift按钮时采取的操作(见`above`/`below`)

其中\<action\>是要加载的另一个配置的名称（remap，从新映射）或其中之一

```
ACTRLS_NONE, ACTRLS_VOLUP, ACTRLS_VOLDOWN, ACTRLS_TOGGLE, ACTRLS_PLAY, 
ACTRLS_PAUSE, ACTRLS_STOP, ACTRLS_REW, ACTRLS_FWD, ACTRLS_PREV, ACTRLS_NEXT, 
BCTRLS_UP, BCTRLS_DOWN, BCTRLS_LEFT, BCTRLS_RIGHT,
KNOB_LEFT, KNOB_RIGHT, KNOB_PUSH
```
如果创建了这样一个字符串，请使用它来填充一个新的`NVS`参数，该参数的名称应小于16（？）字符。您可以拥有尽可能多的这些配置。然后用默认配置的名称设置配置参数“actrls_config”

例如名为`buttons`的配置：
```
[{"gpio":4,"type":"BUTTON_LOW","pull":true,"long_press":1000,"normal":{"pressed":"ACTRLS_VOLDOWN"},"longpress":{"pressed":"buttons_remap"}},
 {"gpio":5,"type":"BUTTON_LOW","pull":true,"shifter_gpio":4,"normal":{"pressed":"ACTRLS_VOLUP"}, "shifted":{"pressed":"ACTRLS_TOGGLE"}}]
``` 
它定义了2个按钮
- 第一个在GPIO 4,低电平激活，当按下它会触发音量降低命令，当按下超过1000ms时，它会更改为"buttons_remap"的按钮配置
- 第二个在GPIO 5, 低电平激活，当按下它会触发音量上调命令，如果第一个按钮与此按钮同时按下，则生成播放/暂停切换命令。

名为"buttons_remap"的配置
```
[{"gpio":4,"type":"BUTTON_LOW","pull":true,"long_press":1000,"normal":{"pressed":"BCTRLS_DOWN"},"longpress":{"pressed":"buttons"}},
 {"gpio":5,"type":"BUTTON_LOW","pull":true,"shifter_gpio":4,"normal":{"pressed":"BCTRLS_UP"}}]
``` 
它定义了2个按钮
- 首先是GPIO 4，低电平。按下时，会触发向下导航命令。当按下超过1000ms时，它会改变上述按钮的配置（比如恢复为前面定义的按钮配置）
- 第二个在GPIO 5, 低电平激活，当按下它会触发向上导航命令。那个按钮，在那个配置中，没有上档(`shift`)组合选项

下面是一个困难但功能强大的2按钮界面，可以增加你的解码控制能力

*buttons*
```
[{"gpio":4,"type":"BUTTON_LOW","pull":true,"long_press":1000,
 "normal":{"pressed":"ACTRLS_VOLDOWN"},
 "longpress":{"pressed":"buttons_remap"}},
 {"gpio":5,"type":"BUTTON_LOW","pull":true,"long_press":1000,"shifter_gpio":4,
 "normal":{"pressed":"ACTRLS_VOLUP"}, 
 "shifted":{"pressed":"ACTRLS_TOGGLE"}, 
 "longpress":{"pressed":"ACTRLS_NEXT"}}
]
```
*buttons_remap*
```
[{"gpio":4,"type":"BUTTON_LOW","pull":true,"long_press":1000,
 "normal":{"pressed":"BCTRLS_DOWN"},
 "longpress":{"pressed":"buttons"}},
 {"gpio":5,"type":"BUTTON_LOW","pull":true,"long_press":1000,"shifter_gpio":4,
 "normal":{"pressed":"BCTRLS_UP"},
 "shifted":{"pressed":"BCTRLS_PUSH"},
 "longpress":{"pressed":"ACTRLS_PLAY"},
 "longshifted":{"pressed":"BCTRLS_LEFT"}}
]
```
<strong>重要注意事项</strong>: LMS还支持发送“原始”（RAW）按钮代码，它有点复杂，所以容我解释下：按钮既可以由squezeesp32处理并映射到“功能”，如播放/暂停，也可以将它们作为普通（原始）代码发送到LMS（服务端），相应按下/释放/长按等的完整控制逻辑由LMS（端）处理，您对此没有任何控制权。

“原始”（raw）模式的好处是，你可以建立一个尽可能接近Boom（Squeezebox Boom，罗技公司的产品）的播放器，但你不能使用重新映射功能或longress或shift逻辑来做你自己的映射，当你有一组有限的按钮。在'raw'模式下，你真正需要定义的只是gpio和按钮之间的映射。就LMS而言，这些JSON有效负载中的任何其他选项都无关紧要。现在，当您使用BT或AirPlay时，上面描述的完整JSON结构完全适用，所以shift、longpress和remapping选项仍然有效（它们在LMS进行处理）。

没有好的或坏的选择，这是你的选择。使用`NVS`参数`lms-u-ctrls-raw`更改该选项。

### Battery / ADC
`NVS`参数"bat_config"设置用于测量电池/DC电压的ADC1信道。标度是应用于12位ADC的每个样本的浮点数。每隔10秒可测量一次，平均每5分钟测量一次（不是滑动窗口）。语法是：
```
channel=0..7,scale=<scale>,cells=<2|3>
```
**注意**：将参数设置为空以禁用电池读数。对于众所周知的配置，这里省略了（除了SqueezeAMP,其中需要`cells`） 

# 配置（使用相关的）
## 设置WiFi
- 初次使用时，启动esp固件,搜索名为 "squeezelite"的SSID，连接密码也是 "squeezelite"
- 首次连接后，用浏览器访问`192.168.4.1` 
- 等待从设备可见的真正wifi访问点列表填充到网页中。
- 选择一个有效的接入点（比如你家wifi路由器的SSID）并根据需要输入任何凭证（连接密码）
- 一旦建立了连接，记下设备接收到的地址；这是以后配置它时使用的地址（可能这个地址需要在路由器上查找DHCP分配情况）

## 设置squeezelite命令行(可选)
此时，该设备应已禁用其内置接入点，并应连接到已知的WiFi网络。
- 导航至步骤1中记录的地址（新wifi路由器分配的地址）
- 使用预定义选项的列表，选择要启动squeezelite的模式
- 生成命令
- 添加或更改任何附加的命令行选项（例如播放器名等）
- 激活screezelite执行：这告诉设备在启动时自动运行命令
- 更新配置
- 点击"start toggle"按钮。这将强制重新启动（以使配置生效）。
- 拨动开关应设置为'ON'，以确保SquezeLite在启动后处于活动状态（您可能需要尝试它几次）
- 您可以在'credits'下启用对`NVS`参数的访问设置。

## Monitor（监视）

除了esp-idf 串口监视选项之外，您还可以启用telnet服务器（请参阅`NVS`参数），您可以访问WROVER内部发生的大量日志。 

## 更新Squeezelite
- 从固件(firmware)页面，点击"Check for Updates"
- 搜索升级固件
- 选择一个
- 点击"Flash!"
- 系统将重新启动到恢复（Recovery）模式（如果尚未处于该模式），擦除ScreezeLite分区并下载/闪存所选版本
- 您可以选择一个本地文件或拥有一个本地web服务器

## Recovery（恢复）模式
- 从固件(firmware)页面，点击"Recovery"按钮，它将重启到恢复模式，如果可能，将从NVS编辑器获得其他配置选项。

## 附加配置说明(在Web UI上的)
ESP32版的squezelite选项，与常规Linux版本的选项非常相似。 不同点在于:
-  输出是 `-o ["BT -n '<sinkname>' "] | [I2S]`
-  如果您已经使用RESAMPLE（重采样）选项进行编译，那么可以使用 `-R [-u <options>]`这样的`soxr`选项。**注意**任何高于LQ或MQ的内容都会使得CPU过载
- 如果使用RESAMPLE16，且 \<options\>为 `(b|l|m)[:i]`，其中b = basic，基本线性插值，l = 13 个采样，m=21个采样，i=插值滤波器系数

例如，使用一个名为MySpeaker的BT扬声器，接受高达192kHz的音频，将所有数据重采样到44100，并使用中等质量的16位重采样，命令行是：
	
	squeezelite -o "BT -n 'BT <sinkname>'" -b 500:2000 -R -u m -Z 192000 -r "44100-44100"

参考squeezlite命令行，但 keys选项是：

	- Z <rate> : 告知LMS重采样之前支持的最大采样率是多少
	- R (见下)
	- r "<minrate>-<maxrate>"，"<最小频率>-<最大频率>"
	- C <sec> : 设置超时，以关闭放大器GPIO
	- W : 激活WAV和AIFF头解析
  
# 自行编译
## 设置ESP-IDF(Setting up ESP-IDF)
### Docker
您可以使用docker构建squeezelite-esp32（可选）
首先需要构建Docker容器：
```
docker build -t esp-idf .
```
然后运行这个容器:
```
docker run -i -t -v `pwd`:/workspace/squeezelite-esp32 esp-idf
```
上面的命令将把这个repo挂载到docker容器中并启动bash终端
然后按照下面的构建步骤操作 

### 手动安装ESP-IDF
<strong>目前，这个项目的主分支需要这个[IDF](https://github.com/espressif/esp-idf/tree/28f1cdf5ed7149d146ad5019c265c8bc3bfa2ac9) ，并配合gcc 5.2 (工具链生成日期20181001)
如果您想使用gcc和IDF的较新版本（4.0稳定版），请转到cmake-master 分支(branch)</strong>

您可以按照以下说明在Linux或Windows上手动安装IDF（例如使用Linux子系统）：[https://www.instructables.com/id/ESP32-Development-on-Windows-Subsystem-for-Linux/](https://www.instructables.com/id/ESP32-Development-on-Windows-Subsystem-for-Linux/)
你还需要使用ESPDSP的最新版本，或者至少确保你有这个补丁[https://github.com/espressif/esp-dsp/pull/12/commits/8b082c1071497d49346ee6ed55351470c1cb4264](https://github.com/espressif/esp-dsp/pull/12/commits/8b082c1071497d49346ee6ed55351470c1cb4264)。在撰写本文时（2020年8月8日），espressif已经修补了esp dsp，因此不再需要它。

## 编译 Squeezelite-esp32
别忘了在`build_scripts/`中选择一个配置文件并将其重命名，`sdkconfig.defaults`或`sdkconfig`中设置了许多重要的WiFi/BT选项。这些脚本不会重建编解码器库（这是一个冗长的过程-请参见下文）
### Usng make (已经弃用，故翻译中省略掉不译——保持原文方便对照)
MOST IMPORTANT: create the right default config file
- make defconfig
(Note: You can also copy over config files from the build-scripts folder to ./sdkconfig)
Then adapt the config file to your wifi/BT/I2C device (can also be done on the command line)
- make menuconfig
Then

```
# Build recovery.bin, bootloader.bin, ota_data_initial.bin, partitions.bin  
# force appropriate rebuild by touching all the files which may have a RECOVERY_APPLICATION specific source compile logic
	find . \( -name "*.cpp" -o -name "*.c" -o -name "*.h" \) -type f -print0 | xargs -0 grep -l "RECOVERY_APPLICATION" | xargs touch
	export PROJECT_NAME="recovery" 
	make -j4 all EXTRA_CPPFLAGS='-DRECOVERY_APPLICATION=1'
make flash
#
# Build squeezelite.bin
# Now force a rebuild by touching all the files which may have a RECOVERY_APPLICATION specific source compile logic
find . \( -name "*.cpp" -o -name "*.c" -o -name "*.h" \) -type f -print0 | xargs -0 grep -l "RECOVERY_APPLICATION" | xargs touch
export PROJECT_NAME="squeezelite" 
make -j4 app EXTRA_CPPFLAGS='-DRECOVERY_APPLICATION=0'
python ${IDF_PATH}/components/esptool_py/esptool/esptool.py --chip esp32 --port ${ESPPORT} --baud ${ESPBAUD} --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0x150000 ./build/squeezelite.bin  
# monitor serial output
make monitor

```

You can also manually download the recovery & initial boot
```
python ${IDF_PATH}/components/esptool_py/esptool/esptool.py --chip esp32 --port ${ESPPORT} --baud ${ESPBAUD} --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xd000 ./build/ota_data_initial.bin 0x1000 ./build/bootloader/bootloader.bin 0x10000 ./build/recovery.bin 0x8000 ./build/partitions.bin
```
### 使用cmake
Create you config using 使用'idf.py menuconfig'创建你的配置，然后使用'idf.py all'来编译出固件。它将构建恢复（recovery）模式和应用程序（screezelite）本身。否则，如果你只想下载squeezelite到ESP32上，就执行
```
python.exe <idf_path>\components\esptool_py\esptool\esptool.py -p COM<n> -b 921600 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x150000 build\squeezelite.bin
```
使用 'idf monitor'来监视程序(参见esp-idf文档)
## 关于编译的其他杂项注释（说明）
- 在撰写本文时，ESP-IDF在计算i2s的PLL值时存在一个bug，因此您*必须*使用patch目录中的i2s.c文件
- 对于编解码器库，如果要重建它们，请添加`-mlongcalls`，但不应（使用codecs/lib中提供的调用）。如果你真的想重建它们，在github中发起一个问题
- libmad, libflac (没有esp的版本), libvorbis (tremor -也没有esp的版本), alac 可以工作
- libfaad并不真正支持实时，但是如果您想尝试，可按下操作
	- -O3 -DFIXED_POINT -DSMALL_STACK
	- 在configure 和 case ac_files中修改 ac_link ，移除掉 ''
	- 在 cfft.c 和 cffti1, 必须使用
```
			#pragma GCC push_options
			#pragma GCC optimize ("O0")
			#pragma GCC pop_options
```
- 最好用helixaac						
- opus & opusfile （编号与编号文件）
	- 对于编号，ESP-provided库似乎是可以工作的，但编号文件(opusfile)是必须的。 
	- 对于mad和其他选项，编辑configure并更改`$ac_link`以添加`-c`（伪造链接）
	- 修改ac_files，去除''
	- 添加DEPS_CFLAGS和DEPS_LIBS以避免需要pkg-config
	- 对于某些编解码器变体，堆栈消耗可能非常高，因此请在config.h中设置`NONTHREADSAFE_PSEUDOSTACK` 和` GLOBAL_STACK_SIZE=32000`,并且取消设置 `VAR_ARRAYS`
- 设置`IDF_PATH=/home/esp-idf`
- 其它编译定义 #define
	- 使用`no resampling`（不重采样）或者设置`RESAMPLE` (soxr) 或者设置`RESAMPLE16`(快速固定16位重采样)
	- 使用 `LOOPBACK` (强制——mandatory)
	- 使用 `BYTES_PER_FRAME=4` (8现在还不能提供完整功能)
	- `LINKALL` (必选)
	- `NO_FAAD` 除非你想用我们的 `faad`,否则不会有FAAD，这会导致CPU过载
	- `TREMOR_ONLY` (强制——mandatory)	
- libmad已经进行了修补，以避免使用大量堆栈。在1.15.1b中存在一个sycn detection（同步检测）问题，该问题源于最初的堆栈修补程序，但由于已对sync detection进行了一些修复。debian上的1.15.1b-10版本修复了mad,认为它已经达到同步(但实际还没有达到）的问题，因此返回错误的采样率。它的代价是8KB（！）一个简单的签入squezelite/mad.c，`next_frame[0]`是`0xff`，`next_frame[1] &0xf0`是0xf0 的代码就可以做到这一点。。。
- 在最初项目git克隆时，请确保以递归方式进行克隆。例如：
	- `git clone --recursive https://github.com/sle118/squeezelite-esp32.git`
- 如果已经克隆了存储库，并且其中一个子模块（例如telnet）上出现编译错误，请在存储库位置的根目录下运行以下git命令
	-  `git submodule update --init --recursive`
