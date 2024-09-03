# esp32-c3-lvgl-study

## 说明

该项目基于 [ESP32-C3-LCDkit](https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32c3/esp32-c3-lcdkit/index.html) 开发板、`esp32-c3-lcdkit` 的 [knob_panel](https://github.com/espressif/esp-dev-kits/tree/master/esp32-c3-lcdkit/examples/knob_panel) 例程。

原计划是基于该例程，实现简单的 wifi + 蓝牙 + 手表功能，学习 lvgl 等各个模组的使用，但是受限于开发板资源，无法并用，内部 flash 只有 4M，所以砍掉了 WiFi 和蓝牙，单纯学习 lvgl。

目前实现开机动画、表盘、单日数据、设置背光、语言切换、关于界面，由于该屏幕没有触摸功能，只能通过旋钮编码器控制，所以体验不佳，纯娱乐学习项目，并且没有 sensor 所以心率、步数等皆为假数据。

部分图片如下：

![](/docs/img/00.png)

主界面由 https://testingcf.jsdelivr.net/gh/nixgnauhcuy/blog_cdn@1.0/img/other/people.png PS 制作完成，个人感觉蛮好看的。

![](/docs/img/01.png)

![](/docs/img/02.png)