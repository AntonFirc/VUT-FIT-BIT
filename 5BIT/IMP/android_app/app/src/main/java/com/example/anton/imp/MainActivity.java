/*
    IMP - ESP8266: snímání teploty (IoT, WiFi AP pro mobilní telefon)
    Anton Firc (xfirca00)
    original
 */
package com.example.anton.imp;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getTemp();

    }

    public void getTemp() {

        final TextView t = (TextView) findViewById(R.id.temp);
        final TextView time = (TextView) findViewById(R.id.curr_time);
        final TextView hist[] = {null, null, null, null, null};
        /* get history textview-s to an array */
        hist[0] = findViewById(R.id.hist_1);
        hist[1] = findViewById(R.id.hist_2);
        hist[2] = findViewById(R.id.hist_3);
        hist[3] = findViewById(R.id.hist_4);
        hist[4] = findViewById(R.id.hist_5);

        Runnable tempUpdate = new Runnable() {
            String temperature;
            @Override
            public void run() {
                runOnUiThread(new Runnable() {  //on UI thread - textview manipulation
                    @Override
                    public void run() {
                        try {

                            ReadWebPage reader = new ReadWebPage();

                            reader.execute();

                            if ((temperature = reader.get()) == null ) {    //something went wrong when communicating with sensor
                                t.setText("No sensor data!");
                                time.setText("Sensor time: -");
                            }
                            else {  //parse returned string and output it
                                String[] temps = temperature.split("<br>");
                                for (int i = 0; i < 6; i++) {
                                    if (i == 0) {   //first entry = current temperature
                                        String[] split = temps[i].split(" ");
                                        t.setText(split[1] + " °C");
                                        time.setText("Sensor time: " + split[0]);
                                    }
                                    else {  //other entries = history
                                        String[] split = temps[i].split(" ");
                                        hist[i-1].setText(split[0] + "  " + split[1] + " °C");
                                    }
                                }
                            }
                        }
                        catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                });
            }
        };

        /* schedule temperature update every second  */
        ScheduledExecutorService executor = Executors.newScheduledThreadPool(1);
        executor.scheduleAtFixedRate(tempUpdate, 0, 1, TimeUnit.SECONDS);

    }

}


