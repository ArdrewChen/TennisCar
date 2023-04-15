package com.example.summer3

import android.annotation.SuppressLint
import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.core.os.bundleOf
import androidx.fragment.app.Fragment
import androidx.navigation.NavController
import androidx.navigation.Navigation
import androidx.navigation.findNavController
import java.net.ConnectException
import java.net.NoRouteToHostException
import java.net.Socket
import java.net.SocketTimeoutException
import kotlin.concurrent.thread


private const val ARG_PARAM1 = "param1"
private const val ARG_PARAM2 = "param2"


class Home : Fragment() {

    private var param1: String? = null
    private var param2: String? = null
    private var button: Button? = null
    private var controller: NavController? = null
    private var isConnect = false //与服务端连接状态
    private var mcontext: Context? = null
    private var socket: Socket? = null


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
        return inflater.inflate(R.layout.fragment_home, container, false)


    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        button = view.findViewById<Button>(R.id.button)
        button?.addClickScale()
        button?.setOnClickListener {
            controller = Navigation.findNavController(view)
            val text1: EditText = view.findViewById(R.id.editTextTextPersonName)
            val text2: EditText = view.findViewById(R.id.editTextTextPersonName2)
            val port = text1.text.toString()
            val ip = text2.text.toString()
            this.mcontext = activity
            if ((ip == "") || (port == "")) {
                Toast.makeText(mcontext, "请检查输入内容", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            } else {

                if (!isConnect) {
                    thread {
                        InitConnect(ip, port)
                    }
                    Thread.sleep(300)
                }
                if (isConnect) {
                    isConnect = false
                    socket?.close()
                    val bundle = bundleOf("ip" to ip, "port" to port) //利用bundle传递数据
                    view.findNavController().navigate(R.id.action_home2_to_control, bundle)
                } else {
                    Toast.makeText(mcontext, "连接失败", Toast.LENGTH_SHORT).show()
                }

            }
        }

    }

    companion object {

        @JvmStatic
        fun newInstance(param1: String, param2: String) =
            Home().apply {
                arguments = Bundle().apply {
                    putString(ARG_PARAM1, param1)
                    putString(ARG_PARAM2, param2)
                }
            }
    }

    //连接服务端
    fun InitConnect(ip: String, port: String) {
        val port1 = port.toInt()
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

}