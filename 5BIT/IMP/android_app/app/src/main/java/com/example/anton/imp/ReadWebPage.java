/*
    IMP - ESP8266: snímání teploty (IoT, WiFi AP pro mobilní telefon)
    Anton Firc (xfirca00)
    original
 */
package com.example.anton.imp;

import android.os.AsyncTask;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;

public class ReadWebPage extends AsyncTask<Void, Double, String> {

    @Override
    protected String doInBackground(Void... none)
    {
        String temperature = null;
        BufferedReader br = null;
        StringBuilder sb = null;

        try {

            //get connection to ESP8266 HTTP server at http://192.168.4.1
            URLConnection conn = new URL("http://192.168.4.1").openConnection();
            conn.setConnectTimeout(1000);
            conn.setReadTimeout(1000);
            br = new BufferedReader(new InputStreamReader(conn.getInputStream())); //open reader on server

            String line;

            sb = new StringBuilder();

            while ((line = br.readLine()) != null) {    //get content from server
                sb.append(line);
                temperature = line;
            }
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {   //if connection timeout, continue
            System.out.println("No sensor data!");
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return temperature; //return string read from server
    }

}
