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
// API_KEY��Ҫ�޸�

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
/// wifi ����LED�ͷ������������°��� 2����һֱͣ����������������°��� 1���л�����һ��wifi����
/// </summary>
/// <param name="key_status"> ����״̬�ĵ�ַ�������޸İ���״̬ </param>
/// <param name="wifi_status"> wifi״̬�ĵ�ַ������ȷ���Ƿ���Ҫ��ʼ��wifi </param>
void wifi_led_buzzer(int *key_status, int *wifi_status)
{
    // ���wifi�Ѿ�����ʼ���ˣ��Ͳ���Ҫ��ʼ���ˣ�if�ﲻ��ִ��
    if (*wifi_status == 0)
    {
        led_on(0);
        esp8266_link_wifi("Jessica", "19890418");
        //�ֻ��ȵ���Ҫ�޸�(�ȵ�����,�ȵ�����)
        led_off(0);
        *wifi_status = 1;
    }

    // ��֪��������һ���Ǹ����
    // OLED_ShowCHinese1(0, 3);
    // led_on(0);
    // esp8266_link_wifi("Jessica", "19890418");
    // //�ֻ��ȵ���Ҫ�޸�(�ȵ�����,�ȵ�����)
    // // delay_ms(1000);
    // led_off(0);
    // OLED_Clear();
    // OLED_ShowCHinese2(0, 3);

    set_wifi_recv_handler(recv_handler);
    //���������Ʒ������������������

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
        if (*key_status != 1) // ����Ϊ1�������������
        {
            return;
        }
    }
}

/// <summary>
/// wifi �ϴ�dht�����������ݣ�ͬʱ����¶���ʾ��OLED�ϣ������°��� 1����һֱͣ����������������°��� 2���л�����һ��wifi����
/// </summary>
/// <param name="key_status"> �����ǰ���״̬�ĵ�ַ�������޸İ���״̬ </param>
/// <param name="wifi_status"> wifi״̬�ĵ�ַ������ȷ���Ƿ���Ҫ��ʼ��wifi </param>
void wifi_upload_dht_data(int *key_status, int *wifi_status)
{
    int sensor_id_arrary[SIZE] = {8187, 8188}; //��Ҫ�޸ĳ��Լ��Ĵ�������ID��
    double sensor_data_arrary[SIZE] = {0, 0};  //������ʵ����
    char temper[5];

    // ���wifi�Ѿ�����ʼ���ˣ��Ͳ���Ҫ��ʼ���ˣ�if�ﲻ��ִ��
    if (*wifi_status == 0)
    {
        led_on(0);
        esp8266_link_wifi("Jessica", "19890418");
        //�ֻ��ȵ���Ҫ�޸�(�ȵ�����,�ȵ�����)
        led_off(0);
        *wifi_status = 1;
    }

    // Ӧ�ÿ���ȥ�����Ҳ�ȷ��������Ҫ����
    // OLED_ShowCHinese1(0, 3);
    // led_on(0);
    // esp8266_link_wifi("Jessica", "19890418");
    // //�ֻ��ȵ���Ҫ�޸�(�ȵ�����,�ȵ�����)
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
        if (*key_status != 2) // ����Ϊ1�������������
        {
            return;
        }
    }
}

int main(void)
{
    int key_status = 1;  // ������״̬��Ĭ��Ϊ���� 1����
    int wifi_status = 0; // wifi��״̬��û�г�ʼ����ʱ��Ϊ0����ʼ���Ժ�Ϊ 1

    // ���ݳ�ʼ��
    led_init();
    key_init();
    buzzer_init();
    delay_init();
    OLED_Init();
    dht_init();
    esp8266_init();
    OLED_Clear();

    picture_1(); // �о�����Ҫ��һ�У���ʾLOGO�ģ�

    while (1)
    {
        key_status = key_stat(); //��ȡ����״̬
        switch (key_status)
        {
        case 1:                                         // ���� 1
            wifi_led_buzzer(&key_status, &wifi_status); // wifi ����LED�ͷ�����
            break;
        case 2:                                              // ���� 2
            wifi_upload_dht_data(&key_status, &wifi_status); // wifi �ϴ�dht�����������ݣ�ͬʱ����¶���ʾ��OLED��
            break;

        default:
            // ���ﲻ��Ҫд����
            // Ϊ���﷨Ҫ��Ҫдdefault
            // ������˵������������
            break;
        }
    }
}
