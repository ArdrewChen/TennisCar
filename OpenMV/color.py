# 色块监测 例子
#
# 这个例子展示了如何通过find_blobs()函数来查找图像中的色块
# 这个例子查找的颜色是深绿色

import sensor, image, time
from pyb import UART

# 颜色追踪的例子，一定要控制环境的光，保持光线是稳定的。
green_threshold   = (37, 95, -62, -24, -25, 55)           #(42, 95, -46, 9, 32, 73)
#设置绿色的阈值，括号里面的数值分别是L A B 的最大值和最小值（minL, maxL, minA,
# maxA, minB, maxB），LAB的值在图像左侧三个坐标图中选取。如果是灰度图，则只需
#设置（min, max）两个数字即可。
uart = UART(3, 15200)
sensor.reset() # 初始化摄像头
sensor.set_pixformat(sensor.RGB565) # 格式为 RGB565.
sensor.set_framesize(sensor.QQVGA) # 使用 QQVGA 速度快一些
sensor.skip_frames(time = 2000) # 跳过2000s，使新设置生效,并自动调节白平衡
sensor.set_auto_gain(False) # 关闭自动自动增益。默认开启的，在颜色识别中，一定要关闭白平衡。
sensor.set_auto_whitebal(False)
#关闭白平衡。白平衡是默认开启的，在颜色识别中，一定要关闭白平衡。
clock = time.clock() # 追踪帧率

while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    img = sensor.snapshot() # 从感光芯片获得一张图像
    img.median(1, percentile=0.5)   # 进行中值滤波
    img.lens_corr(1.8)
    for c in img.find_circles(threshold = 1700, x_margin = 10, y_margin = 10, r_margin = 10,r_min = 2, r_max = 100, r_step = 2):
        if(c.r()>30):
            continue
        else:
            #img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
            area = (c.x()-c.r(), c.y()-c.r(), 2*c.r(), 2*c.r()) # 圆的外接矩形框
            statistics = img.get_statistics(roi=area)#像素颜色统计
            if 37<statistics.l_mode()<95 and -62<statistics.a_mode()<-14 and -25<statistics.b_mode()<55:#l_mode()，a_mode()，b_mode()是L通道，A通道，B通道的众数。
                img.draw_rectangle(area, color = (255, 255, 255))
                print(c.r())
                data= str(c.x())+str(c.y())+str(c.r())
                uart.write(data)

            else:
                continue

    print(clock.fps()) # 注意: 你的OpenMV连到电脑后帧率大概为原来的一半
    #如果断开电脑，帧率会增加
