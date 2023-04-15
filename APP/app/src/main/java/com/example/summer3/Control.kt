package com.example.summer3

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import android.widget.ToggleButton
import androidx.cardview.widget.CardView
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.navigation.NavController
import androidx.navigation.Navigation
import androidx.navigation.findNavController
import com.baidu.speech.EventListener
import com.baidu.speech.EventManager
import com.baidu.speech.EventManagerFactory
import com.baidu.speech.asr.SpeechConstant
import java.io.IOException
import java.net.ConnectException
import java.net.NoRouteToHostException
import java.net.Socket
import java.net.SocketTimeoutException
import java.util.regex.Matcher
import java.util.regex.Pattern
import kotlin.concurrent.thread


// TODO: Rename parameter arguments, choose names that match
// the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
private const val ARG_PARAM1 = "param1"
private const val ARG_PARAM2 = "param2"

/**
 * A simple [Fragment] subclass.
 * Use the [Control.newInstance] factory method to
 * create an instance of this fragment.
 */
class Control : Fragment(), EventListener {
    // TODO: Rename and change types of parameters
    private var param1: String? = null
    private var param2: String? = null
    private var up: Button? = null
    private var down: Button? = null
    private var left: Button? = null
    private var right: Button? = null
    private var pick: Button? = null
    private var card: CardView? = null
    private var textStatus: TextView? = null
    private var isConnect = false //与服务端连接状态
    private var socket: Socket? = null
    private var controller: NavController? = null
    private var asr: EventManager? = null //语音识别核心库
    private var mcontext: Context? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        arguments?.let {
            param1 = it.getString(ARG_PARAM1)
            param2 = it.getString(ARG_PARAM2)
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_control, container, false)
    }

    @SuppressLint("SetTextI18n")
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val ip = arguments?.getString("ip")
        val port = arguments?.getString("port")
        pick = view.findViewById(R.id.button4)
        up = view.findViewById(R.id.button5)
        left = view.findViewById(R.id.button6)
        down = view.findViewById(R.id.button7)
        right = view.findViewById(R.id.button8)
        card = view.findViewById(R.id.card_view)
        textStatus = view.findViewById(R.id.textView4)
        val textip: TextView = view.findViewById(R.id.textView6)
        val textport: TextView = view.findViewById(R.id.textView7)

        this.mcontext = activity
        textport.text = port
        textip.text = ip
        thread {                                //开启子线程
            ip?.let { port?.let { it1 -> InitConnect(it, it1) } }
        }
        textStatus?.text = "已连接"
        card?.addClickScale()
        card?.setOnClickListener {
            socket?.close()
            textStatus?.text = "已断开"
            controller = Navigation.findNavController(view)
            view.findNavController().navigate(R.id.action_control_to_home2)
        }
        up?.addClickScale()
        up?.setOnClickListener {
            thread {
                SendMessage("FFF")
            }
            textStatus?.text = "前进"
        }
        down?.addClickScale()
        down?.setOnClickListener {
            thread {
                SendMessage("BBB")
            }
            textStatus?.text = "后退"
        }
        left?.addClickScale()
        left?.setOnClickListener {
            thread {
                SendMessage("LLL")
            }
            textStatus?.text = "左转"
        }
        right?.addClickScale()
        right?.setOnClickListener {
            thread {
                SendMessage("RRR")
            }
            textStatus?.text = "右转"
        }
        pick?.addClickScale()
        pick?.setOnClickListener {
            thread {
                SendMessage("PPP")
            }
            textStatus?.text = "捡球"
        }
        asr = EventManagerFactory.create(mcontext, "asr")
        //注册自己的输出事件类
        asr!!.registerListener(this)//  EventListener 中 onEvent方法
    }




    //向服务端发送信息
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

    //连接服务端
    fun InitConnect(ip: String, port: String) {
        val port1 = port.toInt()
        try {
            socket = Socket(ip, port1)
            socket!!.soTimeout = 10000  //设置连接超时限制
            if (socket != null) {    //判断一下是否连上，避免NullPointException
                isConnect = true
            } else {
                InitConnect(ip, port)   //没连上就重试一次
            }
        } catch (e: Exception) {
            when (e) {
                is SocketTimeoutException -> {
                    Log.e("连接超时，重新连接", "dd")
                    e.printStackTrace()
                }
                is NoRouteToHostException -> {
                    Log.e("该地址不存在，请检查", "DD")
                    e.printStackTrace()
                }
                is ConnectException -> {
                    Log.e("连接异常或被拒绝，请检查", "DD")
                    e.printStackTrace()
                }
                else -> {
                    e.printStackTrace()
                    Log.e("连接结束", e.toString())
                }
            }
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    fun View.addClickScale(scale: Float = 0.9f, duration: Long = 10) {
        this.setOnTouchListener { _, event ->
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    this.animate().scaleX(scale).scaleY(scale).setDuration(duration).start()
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                    this.animate().scaleX(1f).scaleY(1f).setDuration(duration).start()
                }
            }
            // 点击事件处理，交给View自身
            this.onTouchEvent(event)
        }
    }

    override fun onEvent(
        name: String?,
        params: String?,
        data: ByteArray?,
        offset: Int,
        length: Int
    ) {
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
                    else if(control.contains(hou)){
                        thread {
                            SendMessage("BBB")
                        }
                    }
                    else if(control.contains(zuo)){
                        thread {
                            SendMessage("LLL")
                        }
                    }
                    else if(control.contains(you)){
                        thread {
                            SendMessage("RRR")
                        }
                    }
                    else if(control.contains(jian)){
                        thread {
                            SendMessage("PPP")
                        }
                    }
                    else{
                        return
                    }
                }
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        //发送取消事件
        asr!!.send(SpeechConstant.ASR_CANCEL, "{}", null, 0, 0)
        //退出事件管理器
        // 必须与registerListener成对出现，否则可能造成内存泄露
        asr!!.unregisterListener(this)
    }
}
