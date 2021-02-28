package com.example.arduino_wifi_app;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.charset.Charset;

public class MainActivity extends AppCompatActivity {

    private EditText editText_ip, editText_port;
    private String ipAddress;
    private int port =0;
    private Socket My_socket;
    private DataOutputStream os;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        editText_ip = (EditText) findViewById(R.id.ipaddress);
        editText_port = (EditText) findViewById(R.id.port);

        final Button My_button_UP = findViewById(R.id.button_UP);
        final Button My_button_DOWN = findViewById(R.id.button_DOWN);
        final Button My_button_RIGHT = findViewById(R.id.button_RIGHT);
        final Button My_button_LEFT = findViewById(R.id.button_LEFT);
        final Button My_button_STOP = findViewById(R.id.button_STOP);

        findViewById(R.id.ok_button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                ipAddress = editText_ip.getText().toString();
                port = Integer.parseInt(editText_port.getText().toString());

                new Thread(new Runnable() {
                    @Override
                    public void run() {

                        try {
                            My_socket = new Socket(ipAddress, port);
                            os = new DataOutputStream(My_socket.getOutputStream());

                        } catch (UnknownHostException e) {
                            System.err.println("Unknown host");
                        } catch (IOException e) {
                            System.err.println("Couldn't get Input/Output");
                        }
                    }
                }).start();


                My_button_UP.setEnabled(true);
                My_button_DOWN.setEnabled(true);
                My_button_RIGHT.setEnabled(true);
                My_button_LEFT.setEnabled(true);
                My_button_STOP.setEnabled(true);
            }

        });


        My_button_UP.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            os.write("cmd=F".getBytes(Charset.forName("UTF-8")));
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });


        My_button_DOWN.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            os.write("cmd=B".getBytes(Charset.forName("UTF-8")));
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });


        My_button_RIGHT.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            os.write("cmd=R".getBytes(Charset.forName("UTF-8")));
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });


        My_button_LEFT.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            os.write("cmd=L".getBytes(Charset.forName("UTF-8")));
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });

        My_button_STOP.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            os.write("cmd=S".getBytes(Charset.forName("UTF-8")));
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });

    }
}