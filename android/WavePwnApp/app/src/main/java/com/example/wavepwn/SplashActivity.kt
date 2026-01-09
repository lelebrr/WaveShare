package com.example.wavepwn

import android.content.Context
import android.content.Intent
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.os.Bundle
import android.provider.Settings
import android.view.View
import android.widget.Button
import android.widget.ProgressBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import java.io.IOException
import java.net.HttpURLConnection
import java.net.URL

class SplashActivity : AppCompatActivity() {

    private lateinit var statusText: TextView
    private lateinit var retryButton: Button
    private lateinit var wifiSettingsButton: Button
    private lateinit var progressBar: ProgressBar

    private val deviceUrl = "http://192.168.4.1/"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash)

        statusText = findViewById(R.id.txtStatus)
        retryButton = findViewById(R.id.btnRetry)
        wifiSettingsButton = findViewById(R.id.btnOpenWifiSettings)
        progressBar = findViewById(R.id.progressBar)

        retryButton.setOnClickListener {
            startCheck()
        }

        wifiSettingsButton.setOnClickListener {
            startActivity(Intent(Settings.ACTION_WIFI_SETTINGS))
        }

        startCheck()
    }

    private fun startCheck() {
        statusText.text = getString(R.string.status_checking_connection)
        retryButton.visibility = View.GONE
        wifiSettingsButton.visibility = View.GONE
        progressBar.visibility = View.VISIBLE

        if (!isOnWifi()) {
            showNotOnWifi()
            return
        }

        statusText.text = getString(R.string.status_connecting_device)

        Thread {
            val reachable = tryReachDevice()
            runOnUiThread {
                if (!isFinishing) {
                    if (reachable) {
                        goToDashboard()
                    } else {
                        showDeviceNotFound()
                    }
                }
            }
        }.start()
    }

    private fun isOnWifi(): Boolean {
        val cm = getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        val network = cm.activeNetwork ?: return false
        val caps = cm.getNetworkCapabilities(network) ?: return false
        return caps.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)
    }

    private fun tryReachDevice(timeoutMs: Int = 2000): Boolean {
        return try {
            val url = URL(deviceUrl)
            val conn = url.openConnection() as HttpURLConnection
            conn.connectTimeout = timeoutMs
            conn.readTimeout = timeoutMs
            conn.requestMethod = "GET"
            conn.instanceFollowRedirects = false
            conn.useCaches = false
            conn.connect()
            val code = conn.responseCode
            conn.disconnect()
            code in 200..599
        } catch (e: IOException) {
            false
        }
    }

    private fun showNotOnWifi() {
        statusText.text = getString(R.string.status_not_on_wifi)
        progressBar.visibility = View.GONE
        retryButton.visibility = View.VISIBLE
        wifiSettingsButton.visibility = View.VISIBLE
    }

    private fun showDeviceNotFound() {
        statusText.text = getString(R.string.status_device_not_found)
        progressBar.visibility = View.GONE
        retryButton.visibility = View.VISIBLE
        wifiSettingsButton.visibility = View.VISIBLE
    }

    private fun goToDashboard() {
        startActivity(Intent(this, MainActivity::class.java))
        finish()
    }
}