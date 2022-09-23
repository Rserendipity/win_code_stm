#include "led.h"
#include "buzzer.h"
#include "key.h"
#include "delay.h"
#include "dht.h"
#include "oled.h"
#include "esp8266.h"
#include "iot.h"
#include "string.h"
#define SIZE 2

#define API_KEY "eyJpYXQiOjE2NjM1NDg2MDIsImFsZyI6IkhTMjU2IiwiZXhwIjoxOTc4OTA4NjAyfQ.eyJpZCI6Mjk3MH0.e9AX0mSzXgy1DkQgmdgwGhoW253md_Q96J4z9kMgvPM:"
// API_KEY需要修改

void recv_handler(char *buf, int len)
{
    if (g_control == 0 || g_control == 1 || g_control == 2)
    {
        if (strstr(buf, "true"))
        {
            led_on(g_control);
        }
        else
        {
            led_off(g_control);
        }
    }
    if (g_control == 3)
    {
        if (strstr(buf, "true"))
        {
            buzzer_on();
        }
        else
        {
            buzzer_off();
        }
    }
}

/// <summary>
/// wifi 控制LED和蜂鸣器，不按下按键 2，会一直停留在这个函数，按下按键 1会切换到另一个wifi函数
/// </summary>
/// <param name="key_status"> 按键状态的地址，用于修改按键状态 </param>
/// <param name="wifi_status"> wifi状态的地址，用于确认是否需要初始化wifi </param>
void wifi_led_buzzer(int *key_status, int *wifi_status)
{
    // 如果wifi已经被初始化了，就不需要初始化了，if里不会执行
    if (*wifi_status == 0)
    {
        led_on(0);
        esp8266_link_wifi("Jessica", "19890418");
        //手机热点需要修改(热点名称,热点密码)
        led_off(0);
        *wifi_status = 1;
    }

    // 不知道下面这一段是干嘛的
    // OLED_ShowCHinese1(0, 3);
    // led_on(0);
    // esp8266_link_wifi("Jessica", "19890418");
    // //手机热点需要修改(热点名称,热点密码)
    // // delay_ms(1000);
    // led_off(0);
    // OLED_Clear();
    // OLED_ShowCHinese2(0, 3);

    set_wifi_recv_handler(recv_handler);
    //用来分析云服务器传输回来的数据

    while (1)
    {
        g_control = 0; // led 0
        download_sensor_status("119.29.98.16", "3091", "8122", API_KEY);

        g_control = 1; // led 1
        download_sensor_status("119.29.98.16", "3091", "8157", API_KEY);

        g_control = 2; // led 2
        download_sensor_status("119.29.98.16", "3091", "8163", API_KEY);

        g_control = 3; // buzzer
        download_sensor_status("119.29.98.16", "3091", "8184", API_KEY);

        *key_status = key_stat();
        if (*key_status != 1) // 按键为1，结束这个函数
        {
            return;
        }
    }
}

/// <summary>
/// wifi 上传dht传感器的数据，同时会把温度显示在OLED上，不按下按键 1，会一直停留在这个函数，按下按键 2会切换到另一个wifi函数
/// </summary>
/// <param name="key_status"> 参数是按键状态的地址，用于修改按键状态 </param>
/// <param name="wifi_status"> wifi状态的地址，用于确认是否需要初始化wifi </param>
void wifi_upload_dht_data(int *key_status, int *wifi_status)
{
    int sensor_id_arrary[SIZE] = {8187, 8188}; //需要修改成自己的传感器的ID号
    double sensor_data_arrary[SIZE] = {0, 0};  //不是真实数据
    char temper[5];

    // 如果wifi已经被初始化了，就不需要初始化了，if里不会执行
    if (*wifi_status == 0)
    {
        led_on(0);
        esp8266_link_wifi("Jessica", "19890418");
        //手机热点需要修改(热点名称,热点密码)
        led_off(0);
        *wifi_status = 1;
    }

    // 应该可以去掉，我不确定，明天要试试
    // OLED_ShowCHinese1(0, 3);
    // led_on(0);
    // esp8266_link_wifi("Jessica", "19890418");
    // //手机热点需要修改(热点名称,热点密码)
    // led_off(0);
    // OLED_Clear();
    // OLED_ShowCHinese2(0, 3);

    while (1)
    {
        get_dht_value(temper);
        sensor_data_arrary[0] = (double)temper[2];
        sensor_data_arrary[1] = (double)temper[0];
        upload_device_datas("119.29.98.16", "3091", API_KEY, sensor_id_arrary, sensor_data_arrary, SIZE);

        OLED_ShowTEmp(0, 5);
        OLED_ShowHUm(0, 0);
        OLED_ShowTIttle(60, 5);
        OLED_ShowTIttle1(60, 0);
        // OLED_ShowCHinese3(45,5);
        OLED_ShowNum(37, 0, temper[0], 2, 16);
        OLED_ShowNum(37, 5, temper[2], 2, 16);
        *key_status = key_stat();
        if (*key_status != 2) // 按键为1，结束这个函数
        {
            return;
        }
    }
}

int main(void)
{
    int key_status = 1;  // 按键的状态，默认为按键 1按下
    int wifi_status = 0; // wifi的状态，没有初始化的时候为0，初始化以后为 1

    // 内容初始化
    led_init();
    key_init();
    buzzer_init();
    delay_init();
    OLED_Init();
    dht_init();
    esp8266_init();
    OLED_Clear();

    picture_1(); // 感觉不需要这一行，显示LOGO的？

    while (1)
    {
        key_status = key_stat(); //获取按键状态
        switch (key_status)
        {
        case 1:                                         // 按键 1
            wifi_led_buzzer(&key_status, &wifi_status); // wifi 控制LED和蜂鸣器
            break;
        case 2:                                              // 按键 2
            wifi_upload_dht_data(&key_status, &wifi_status); // wifi 上传dht传感器的数据，同时会把温度显示在OLED上
            break;

        default:
            // 这里不需要写东西
            // 为了语法要求，要写default
            // 理论上说不会进入这里的
            break;
        }
    }
}
