# 语音控制小车运动APP(基于百度语音识别)

## 项目背景

由于暑期优秀本科生项目需求，开发了一款控制机器人行走的APP，具体要求如下：

* 在第一个界面（连接界面）实现Socket连接，连接成功则跳转到下一个页面(控制界面)。
* 在控制界面中创建5个按钮分别对应前进、后退、左转、右转、捡球，
* 按下每个按钮用TCP的方式发送指令到网络调试助手，比如按下前进，就会发送“FFF”到网络调试助手，按下后退，就会发送“BBB”到网络调试助手，要求在网络调试助手上接收到数据，确保网络调试助手和安卓手机在同一个局域网下(手机电脑连接同一WiFi或者电脑连接手机热点)。
* 实现用语音控制小车运动,达到按钮控制效果。

## 安装

### 安装环境

Android Studio
安卓手机
网络调试助手

### 百度语音识别SDK集成

详细教程请参照：[百度语音识别SDK集成](https://blog.csdn.net/qq_38436214/article/details/106636277)

#### 一些备注

1. 出现错误:20 errorCode : 1 desc : VAD start: start error. mLastRecognitionResult.<br>
   **解决方案1**:在AndroidManifest文件application标签增加android:extractNativeLibs="true"(本项目采用).<br>
   **解决方案2**:或者build.gradle(:app)中minSdkVersion设置小一点低于23（例如21）.<br>
2. 出现提示:无法连接到服务端或接口.<br>
   **解决方案1**:检查一下你的百度语音识别的项目中的语音包名与自己要使用这个模块的项目名称是否一致,如本项目中语音包和项目名称都是com.example.summer3.<br>
   **解决方案2**:检查百度提供的AppID,APIKey,SecretKey与项目中的core的AndroidManifest.xml文件中的APP_ID、API_KEY、SECRET_KEY是否对应,同时检查是否添加了该有的权限.（未加权限可能导致APP闪退以及打开语音按钮时无反应，可在安卓手机端检查是否打开麦克风应用权限，有时候也可能因安卓端未给予麦克风应用权限，导致无法正常运行）<br>
   **解决方案3**:检查是否申领了百度提供的免费语音识别调用(最多可调用15万次).<br>

## 使用

1. 将程序下载至安卓手机上,确保电脑和手机处于**同一局域网下**,打开网络调试助手.
2. 设置网络调试助手IP和端口,APP第一个界面为连接,输入错误的IP或者端口,无法跳转.输入正确的IP和端口,跳转到控制界面.
3. 控制界面中设置了六个按钮和一个卡片显示.卡片中显示当前连接的IP和端口,想要断开连接点击卡片,跳回连接界面.卡片中也显示当前命令状态,点击按钮会显示按钮命令,如点击前进,显示前进,同时发送字符串"FFF"给网络调试助手.
4. 点击最下方按钮,变色说明点击成功.开始长语音识别(如果不再次点击,不会停止识别),在卡片中显示命令,当识别到与前进相关的命令时,向小车发送"FFF"字符串,其他命令同上.

## 设计思路

设计思路大致为：首先获取输入框中输入的IP和端口，点击连接按钮后开启子线程进行连接，连接失败则提示连接失败提示框，连接成功则进行页面切换。在执行连接这个子线程中，让主线程休眠，以便让连接过程顺利从而导致判断连接的标志位发生改变。<br>
切换第二个页面后，利用Buddle对象将IP和端口数据传输进来，在onViewCreated中再次连接，然后五个按钮点击后分别在子线程中执行发送相应的字符串，点击语音按钮时，首先通过设置百度语音识别的json变量，设置为长识别（若要更改，参考[百度语音识别文档](https://cloud.baidu.com/doc/SPEECH/s/Pkgt4wwdx)对json变量进行修改即可），根据命令中是否含有“前”、“后”、“左”、“右”、“捡”来判断是否执行相关命令。再次点击，识别取消。<br>

### 界面

界面的设计为创建两个Fragment，使用navigation来控制两个页面的切换。关于安卓导航方面的知识，可参照b站教程：[安卓导航页面实现](https://www.bilibili.com/video/BV1Jx41197am?spm_id_from=333.999.0.0)以及[安卓导航页面数据传输](https://www.bilibili.com/video/BV1vx411Z71z?spm_id_from=333.999.0.0)

### Socket通信

#### 连接

由于是从输入栏中获取到的字符串类型的数据，因此首先需要对端口的数据进行类型转换。isConnect是标志位，判断是否连接上，连接上就置于True.

```
    //连接服务端
    fun InitConnect(ip: String, port: String) {
        val port1 = port.toInt() //将端口数据进行类型转换
        try {
            socket = Socket(ip, port1)
            socket!!.setSoTimeout(10000)  //设置连接超时限制
            if (socket != null) {    //判断一下是否连上，避免NullPointException
                isConnect = true
            } else {
                InitConnect(ip, port)
            }
        } catch (e: Exception) {
            when (e) {
                is SocketTimeoutException -> {
                    Log.e("连接超时，重新连接", "dd")
                    e.printStackTrace()
                }
                is NoRouteToHostException -> {
                    Log.e("该地址不存在，请检查", "DD");
                    e.printStackTrace()
                }
                is ConnectException -> {
                    Log.e("连接异常或被拒绝，请检查", "DD");
                    e.printStackTrace()
                }
                else -> {
                    e.printStackTrace()
                    Log.e("连接结束", e.toString())
                }
            }
        }
    }
```

#### 数据传输

message 即为需要发送的字符串。

```
 //发送数据
 fun SendMessage(message: String?) {

        val dout = socket?.getOutputStream()     //获取输出流
        try {
            if (dout != null && message != null) {
                //判断输出流或者消息是否为空，为空的话会产生nullpoint错
                dout.write(
                    """${message}
    """.toByteArray(charset("utf-8"))           // 写入需要发送的数据到输出流对象中
                ) // 数据的结尾加上换行符才可让服务器端的readline()停止阻塞
                dout.flush() // 发送数据到服务端
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
```

#### APP闪退解决方案

   由于在执行主线程时不能跳转到其他耗时较长的函数中，例如socket通信的连接和传输数据，所以将这些统统放入子线程处理。但是子线程中处理导致主线程无法等待连接状态确定后再决定是否跳转，采用Thread.sleep(300)使主线程睡眠，让子线程跑完再判断。注意同时子线程中也不能出现关于界面方面的内容

### 语音识别

josn中设置需要的语音识别的类型，关于josn中参数的具体设置及对应功能，参照百度语音识别文档中的输入参数中ASR_START 输入事件参数，本文josn采用

```
 val josn="{\"accept-audio-data\":false,\"disable-punctuation\":false,\"enable.long.speech\":true,\"accept-audio-volume\":true,\"pid\":1537}"//start输入参数

```

```
 toggle.setOnCheckedChangeListener { _, isChecked ->
            if (isChecked) {
                asr!!.send(SpeechConstant.ASR_START,josn, null, 0, 0)
            } else {
                asr!!.send(SpeechConstant.ASR_STOP, null, null, 0, 0)

            }
        }
```

识别结束后，提取识别内容,利用control.contains()函数实现观察字符串中是否包含某个字符，完成多个联想词也能实现命令的效果。

```
  if (name.equals(SpeechConstant.CALLBACK_EVENT_ASR_PARTIAL)) {
            // 识别相关的结果都在这里
            if (params == null || params.isEmpty()) {
                return
            }
            if (params.contains("\"final_result\"")) {
                // 一句话的最终识别结果
                val regrex = "\\[(.*?)," //使用正则表达式抽取我们需要的内容
                val pattern: Pattern = Pattern.compile(regrex)
                val matcher: Matcher = pattern.matcher(params)
                if (matcher.find()) {
                    val a = matcher.group(0)!!.indexOf("[")
                    val b = matcher.group(0)!!.indexOf(",")
                    val control= matcher.group(0)!!.substring(a + 2, b - 3)
                    val qian="前"
                    val hou="后"
                    val zuo="左"
                    val you="右"
                    val jian="捡"
                    textStatus!!.text = control
                    if(control.contains(qian)){
                        thread {
                            SendMessage("FFF")
                        }
                    }
```

## 参考资料

1. [百度语音识别文档](https://cloud.baidu.com/doc/SPEECH/s/Pkgt4wwdx)
2. [安卓开发官方文档](https://developer.android.com/guide?hl=zh-cn)
3. 经典书籍:第一行代码
4. [百度语音识别SDK集成](https://blog.csdn.net/qq_38436214/article/details/106636277)
5. [导航栏和数据传输](https://www.bilibili.com/video/BV1vx411Z71z?spm_id_from=333.999.0.0&vd_source=91dcb1a625786af5363d3b65f306049c)
6. [socket通信kotlin实现](https://zhuanlan.zhihu.com/p/357597364)
7. [判断socket连接的坑](https://d0n9x1n.dev/post/43/)
