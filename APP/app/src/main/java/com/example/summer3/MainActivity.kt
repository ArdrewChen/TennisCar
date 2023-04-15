package com.example.summer3

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.navigation.NavController
import androidx.navigation.Navigation
import androidx.navigation.ui.NavigationUI
import java.net.Socket

class MainActivity : AppCompatActivity() {
    private var controller: NavController?=null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        controller=Navigation.findNavController(this,R.id.fragmentContainerView)
        NavigationUI.setupActionBarWithNavController(this, controller!!)
    }

    override fun onSupportNavigateUp(): Boolean {
        controller=Navigation.findNavController(this,R.id.fragmentContainerView)
        return controller!!.navigateUp()
//        return super.onSupportNavigateUp()
    }
}