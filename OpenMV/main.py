# 色块监测 例子
#
# 这个例子展示了如何通过find_blobs()函数来查找图像中的色块
# 这个例子查找的颜色是深绿色

import sensor, image, time
from pyb import UART
from pid import PID


uart = UART(3, 115200)
sensor.reset() # 初始化摄像头
sensor.set_pixformat(sensor.RGB565) # 格式为 RGB565.
sensor.set_framesize(sensor.QQVGA) # 使用 QQVGA 速度快一些
sensor.skip_frames(time = 2000) # 跳过2000s，使新设置生效,并自动调节白平衡
sensor.set_auto_gain(False) # 关闭自动自动增益。默认开启的，在颜色识别中，一定要关闭白平衡。
sensor.set_auto_whitebal(False)
#关闭白平衡。白平衡是默认开启的，在颜色识别中，一定要关闭白平衡。
clock = time.clock() # 追踪帧率
x_pid = PID(p=0.5, i=1, imax=100)
h_pid = PID(p=0.05, i=0.1, imax=50)
uart_buf=[]

while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    img = sensor.snapshot() # 从感光芯片获得一张图像
    img.median(1, percentile=0.5)   # 进行中值滤波
    img.lens_corr(1.8)
    max_r=0
    max_c=None
    for c in img.find_circles(threshold = 1700, x_margin = 10, y_margin = 10, r_margin = 10,r_min = 2, r_max = 100, r_step = 2):
        if(c.r()>30):
            continue
        else:
            area = (c.x()-c.r(), c.y()-c.r(), 2*c.r(), 2*c.r()) # 圆的外接矩形框
            statistics = img.get_statistics(roi=area)#像素颜色统计
            if 37<statistics.l_mode()<95 and -62<statistics.a_mode()<-14 and -25<statistics.b_mode()<55:#l_mode()，a_mode()，b_mode()是L通道，A通道，B通道的众数。
                img.draw_rectangle(area, color = (255, 255, 255))
                if c.r()> max_r:
                    max_r=c.r()
                    max_c=c
            else:
                 continue
    if max_c:
         img.draw_circle(max_c.x(), max_c.y(), max_c.r(), color = (255, 0, 0))
         x_error = max_c.x()-img.width()/2   # 调节小车左右
         r_error = 19-max_c.r()             # 调节小车前后
         x_output=x_pid.get_pid(x_error,1)
         r_output=h_pid.get_pid(r_error,1)
         print("左右：%d,前后：%d"%(x_output,r_output))
         uart_buf =bytearray([0x6B,int(x_output),int(r_output),0x6A])
         uart.write(uart_buf)
    else:
        print("NOT FOUND")
        uart_buf =bytearray([0x6A,0,0,0x6C])
        uart.write(uart_buf)

    print(clock.fps()) # 注意: 你的OpenMV连到电脑后帧率大概为原来的一半
    #如果断开电脑，帧率会增加
