/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "agent_tiny_demo.h"
#include <pthread.h>
#include <unistd.h>
#ifdef CONFIG_FEATURE_FOTA
#include "ota_port.h"
#endif

#define DEFAULT_SERVER_IPV4 "127.0.0.1" //"180.101.147.115" /*dianxin*/

#define LWM2M_LIFE_TIME     50000

char *g_endpoint_name = "44440003";
#ifdef WITH_DTLS

char *g_endpoint_name_s = "99990009";
char *g_endpoint_name_iots = "99990009";
char *g_endpoint_name_bs = "99990009";
unsigned char g_psk_iot_value[] = {0x58,0xea,0xfd,0xab,0x2f,0x38,0x4d,0x39,0x80,0x69,0x4d,0x1c,0xda,0x69,0xb0,0x43}; //0x33 -> 0x32
unsigned char g_psk_bs_value[] = {0x58,0xea,0xfd,0xab,0x2f,0x38,0x4d,0x39,0x80,0x69,0x4d,0x1c,0xda,0x69,0xb0,0x43};
//unsigned char g_psk_value[16] = {0x58,0xea,0xfd,0xab,0x2f,0x38,0x4d,0x39,0x80,0x69,0x4d,0x1c,0xda,0x69,0xb0,0x43};

#define SERVER_NAME "kubernetes.default.svc"
const char server_name[] = SERVER_NAME;

#define LWM2M_TEST_CA_CRT    \
"-----BEGIN CERTIFICATE-----\r\n"    \
"MIICxjCCAa6gAwIBAgIJAJk1DbZBu8FDMA0GCSqGSIb3DQEBCwUAMBMxETAPBgNV\r\n"    \
"BAMMCE15VGVzdENBMB4XDTE3MTEwMjEzNDI0N1oXDTE5MTEwMjEzNDI0N1owEzER\r\n"    \
"MA8GA1UEAwwITXlUZXN0Q0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\r\n"    \
"AQDshDho6ef1JClDJ24peSsXdFnFO3xIB7+BSp1YPcOvmRECKUG0mLORw3hNm15m\r\n"    \
"8eGOn1iLGE/xKlaZ74/xjyq8f7qIGZCmvZj59m+eiJCAmy8SiUJZtSVoOlOzepJd\r\n"    \
"PoDgcBvDKA4ogZ3iJHMUNI3EdlD6nrKEJF2qe2JUrL0gv65uo2/N7XVNvE87Dk3J\r\n"    \
"83KyCAmeu+x+moS1ILnjs2DuPEGSxZqzf7IQMbXuNWJYAOZg9t4Fg0YjTiAaWw3G\r\n"    \
"JKAoMY4tI3JCqlvwGR4lH7kfk3WsD4ofGlFhxU4nEG0xgnJl8BcoJWD1A2RjGe1f\r\n"    \
"qCijqPSe93l2wt8OpbyHzwc7AgMBAAGjHTAbMAwGA1UdEwQFMAMBAf8wCwYDVR0P\r\n"    \
"BAQDAgEGMA0GCSqGSIb3DQEBCwUAA4IBAQAi+t5jBrMxFzoF76kyRd3riNDlWp0w\r\n"    \
"NCewkohBkwBHsQfHzSnc6c504jdyzkEiD42UcI8asPsJcsYrQ+Uo6OBn049u49Wn\r\n"    \
"zcSERVSVec1/TAPS/egFTU9QMWtPSAm8AEaQ6YYAuiwOLCcC+Cm/a3e3dWSRWt8o\r\n"    \
"LqKX6CWTlmKWe182MhFPpZYxZQLGapti4R4mb5QusUbc6tXbkcX82GjDPTOuAw7b\r\n"    \
"mWpzVd5xnlp7Vz+50u+YaAYUmCobg0hR/AuTrA4GDMlgzTnuZQhF6o8iVkypXOtS\r\n"    \
"Ufz6X3tVVErVVc7UUfzSnupHj1M2h4rzlQ3oqHoAEnXcJmV4f/Pf/6FW\r\n"    \
"-----END CERTIFICATE-----\r\n"

const char lwm2m_test_cas_pem[] = LWM2M_TEST_CA_CRT;
const size_t lwm2m_test_cas_pem_len = sizeof(lwm2m_test_cas_pem);

#define LWM2M_TEST_CLI_CRT    \
"-----BEGIN CERTIFICATE-----\r\n"    \
"MIIC6jCCAdKgAwIBAgIBAjANBgkqhkiG9w0BAQsFADATMREwDwYDVQQDDAhNeVRl\r\n"    \
"c3RDQTAeFw0xNzExMDIxMzQyNDhaFw0xOTExMDIxMzQyNDhaMC0xGjAYBgNVBAMT\r\n"    \
"EU1hY0Jvb2stQWlyLmxvY2FsMQ8wDQYDVQQKEwZjbGllbnQwggEiMA0GCSqGSIb3\r\n"    \
"DQEBAQUAA4IBDwAwggEKAoIBAQC8GptpL25hv1Qa3jCn4VLvDRH/SrHg9wXvqRkz\r\n"    \
"HuiKMxYT30m4+kcaXv350CJrkV+8lR24wdN7DBVewpCUnyUBbzkLccy1LUzunZ3z\r\n"    \
"nm37j6cautD3rlC9gsC9d0uJ745FLx5t/6f1jMk9rWxn+4iSGAnkWC3mVaQxP1zQ\r\n"    \
"q8GI97uob9HNb0OH6ygHJAcKOWB+85a29LIMa1uo/lT3hMr8sBg2vX+1F/gTusmW\r\n"    \
"xVoQc9XJxBCs995qsH0UkZIuOY0XZp9/qFfcZv2QmslG8DojIIHKcujzu8bItE2M\r\n"    \
"OyL5NlWLvN6qg59hHzF4+D+T+8GkhhKWSC+xdY14eQ5fB4S5AgMBAAGjLzAtMAkG\r\n"    \
"A1UdEwQCMAAwCwYDVR0PBAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMCMA0GCSqG\r\n"    \
"SIb3DQEBCwUAA4IBAQBLV4ZfhiKiFVnL/xO0MRGSKr3xd0LK64SW8Iw5DYkc0jNX\r\n"    \
"sDrRbj2I/KJ/Rc4AeKT751L+C+KBzYpFgiLrxDmt/5pmgiFH51hPQtL7kRC0z2NY\r\n"    \
"EY/P+u4IFVSo+b1hHYU7y+OMj6/Vvd4x0ETS4rHWI4mPDfGfvClEVLOktgRKrMU5\r\n"    \
"9aTltF4U0FBUlYZTQBNBUFwBzj1+0lxK4EdhRmmWJ+uW9rgkQxpnUdbCPGvUKFRp\r\n"    \
"3AbdHBAU9H2zVd2VZoJu6r7LMp6agxu0rYLgmamRAt+8rnDXvy7H1ZNdjT6fTbUO\r\n"    \
"omVBMyJAc1+10gjpHw/EUD58t5/I5tZrnrANPgIs\r\n"    \
"-----END CERTIFICATE-----\r\n"

const char lwm2m_test_cli_pem[] = LWM2M_TEST_CLI_CRT;
const size_t lwm2m_test_cli_pem_len = sizeof(lwm2m_test_cli_pem);


#define LWM2M_TEST_CLI_KEY    \
"-----BEGIN RSA PRIVATE KEY-----\r\n"    \
"MIIEowIBAAKCAQEAvBqbaS9uYb9UGt4wp+FS7w0R/0qx4PcF76kZMx7oijMWE99J\r\n"    \
"uPpHGl79+dAia5FfvJUduMHTewwVXsKQlJ8lAW85C3HMtS1M7p2d855t+4+nGrrQ\r\n"    \
"965QvYLAvXdLie+ORS8ebf+n9YzJPa1sZ/uIkhgJ5Fgt5lWkMT9c0KvBiPe7qG/R\r\n"    \
"zW9Dh+soByQHCjlgfvOWtvSyDGtbqP5U94TK/LAYNr1/tRf4E7rJlsVaEHPVycQQ\r\n"    \
"rPfearB9FJGSLjmNF2aff6hX3Gb9kJrJRvA6IyCBynLo87vGyLRNjDsi+TZVi7ze\r\n"    \
"qoOfYR8xePg/k/vBpIYSlkgvsXWNeHkOXweEuQIDAQABAoIBAHnFV7peRDzvGUlT\r\n"    \
"cXgcvA2ZDn+QIVsbTzJ466FWbv+YVsCCmj0veHwv5oakIMQ2Fh4FAnqqr3dGuUbg\r\n"    \
"+avc4p3tHKa2Aul+7ADE9I3TkCt8MZdyPPk6VXZ5gMCmy7X96MIM4Mwg5uBlRZmx\r\n"    \
"/S3Lffvlp/G0y/ICmwpulG1Z4y4A5Vc0Qf7fBO03Ekl31oReARnB6ex7RnDHH1mW\r\n"    \
"RyLWNqyu9BhUbFpIyFPWDSkBcajNIbQ6qVJfGLm5Y2xVhwdqbyvY8M06uuMKz/IR\r\n"    \
"SYfdIpiC4PpQZQzzXMn/6LTKWcCe0T+dBcWTZHC3C2abrC7+5fwFobs2xoUaCwz0\r\n"    \
"1CclogECgYEA6Jdv+2VSYIBLbS0VIe07JiZaQNd1QNg63MK/y7oqAKEzYvpWzJel\r\n"    \
"owPdBU3GxZH6vUUF7sCABcjumEDazoqTtzHQBo0xYpJrjmAL0ANNGVvF09pJK2eq\r\n"    \
"yotxJJAS5/lQNSgWOxGVc6qu6ZpgeIXVLIx816yq04h10yVgZ2Lm3+ECgYEAzwj5\r\n"    \
"/UVpN/ak6PwZ+Tq/8qOYjY2ABylRmP+T0Rkqmwh2B5Sp9oXjkDQwWseY0Wybhd8F\r\n"    \
"kO6BUCMUApnB3uU0baawVbDUSrt43SkkKV9m3pA35wA3pYw1a56QIEFr56npFYBS\r\n"    \
"sn9yl/ZtNvnuwmrHWOq8HdwPJsWREyO61yknn9kCgYB1PdixpSo4AJOErePoHRfi\r\n"    \
"rBR0eObez+Aj5Xsea3G+rYMkkkHskUhp+omPodvfPS1h+If8CEbAI7+5OX/R+uJo\r\n"    \
"xpAwrT1Gjb3vn5R0vyU+8havKmoVmgTqYg2fO4x8KBz5HoLONZfbHR9cG3gjaHrD\r\n"    \
"IPHRGXVmeXPDAiUtGBp+oQKBgQDDRIAkNPdMZUCczknhG1w3Cb20pKUAHCRt3YAZ\r\n"    \
"U1cv6gcIl1rGvPko5VBGDsM/ouP8m6CwVYN5hdw1p7eG9z8/vFvMNn/EDJWuYkNN\r\n"    \
"EkH/4J4ZLcdOSLOJ0X+2LH4Nfd/s+58D49i9IxtXItviWruyTZMnxooz01tFZgmv\r\n"    \
"LY3F4QKBgDirafhlJqFK6sa8WesHpD5+lm3Opzi4Ua8fAGHy2oHN3WCEL74q691C\r\n"    \
"fA0P2UrzYiF7dXf4fgK9eMMQsdWS4nKyCSqM6xE4EAhAHUTYzY3ApNjI3XFDIrKC\r\n"    \
"oQefIOLum2UyWFuEoUtrEfc5fxktiQohCwuAvwC59EwhmsNlECA8\r\n"    \
"-----END RSA PRIVATE KEY-----\r\n"


const char lwm2m_test_cli_key[] = LWM2M_TEST_CLI_KEY;
const size_t lwm2m_test_cli_key_len = sizeof(lwm2m_test_cli_key);
#endif

static void *g_phandle = NULL;
static atiny_device_info_t g_device_info;
static atiny_param_t g_atiny_params;

void ack_callback(atiny_report_type_e type, int cookie, data_send_status_e status)
{
    ATINY_LOG(LOG_DEBUG, "type:%d cookie:%d status:%d\n", type, cookie, status);
}
/*lint -e550*/
void app_data_report(void)
{
    uint8_t buf[5] = {0, 1, 6, 5, 9};
    data_report_t report_data;
    int ret = 0;
    int cnt = 0;
    report_data.buf = buf;
    report_data.callback = ack_callback;
    report_data.cookie = 0;
    report_data.len = sizeof(buf);
    report_data.type = APP_DATA;
    (void)ret;
	sleep(10);
    while(1)
    {
        report_data.cookie = cnt;
        cnt++;
        ret = atiny_data_report(g_phandle, &report_data);
        ATINY_LOG(LOG_DEBUG, "data report ret: %d\n", ret);
        ret = atiny_data_change(g_phandle, DEVICE_MEMORY_FREE);
        ATINY_LOG(LOG_DEBUG, "data change ret: %d\n", ret);
        sleep(30);
    }
}
/*lint +e550*/
#if 0
UINT32 creat_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
    UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "app_data_report";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)app_data_report;
    task_init_param.uwStackSize = 0x1000;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;

}
#endif


void agent_tiny_entry(void)
{
    pthread_t tidp;
    unsigned int uwRet = 0;
    atiny_param_t *atiny_params;
    atiny_security_param_t  *iot_security_param = NULL;
    atiny_security_param_t  *bs_security_param = NULL;

    atiny_device_info_t *device_info = &g_device_info;

#ifdef CONFIG_FEATURE_FOTA
    hal_init_ota();
#endif

#ifdef WITH_DTLS
    device_info->endpoint_name = g_endpoint_name_s;
#else
    device_info->endpoint_name = g_endpoint_name;
#endif
#ifdef CONFIG_FEATURE_FOTA
    device_info->manufacturer = "Lwm2mFota";
    device_info->dev_type = "Lwm2mFota";
#else
    device_info->manufacturer = "Agent_Tiny";
#endif
    atiny_params = &g_atiny_params;
    atiny_params->server_params.binding = "UQ";
    //atiny_params->server_params.life_time = LWM2M_LIFE_TIME;
    atiny_params->server_params.life_time = 20;
    atiny_params->server_params.storing_cnt = 0;

    atiny_params->server_params.bootstrap_mode = BOOTSTRAP_FACTORY;
    atiny_params->server_params.hold_off_time = 10;

    //pay attention: index 0 for iot server, index 1 for bootstrap server.
    iot_security_param = &(atiny_params->security_params[0]);
    bs_security_param = &(atiny_params->security_params[1]);


    iot_security_param->server_ip = DEFAULT_SERVER_IPV4;
    bs_security_param->server_ip = DEFAULT_SERVER_IPV4;

#ifdef WITH_DTLS
#if 0
    iot_security_param->server_port = "5684";
    bs_security_param->server_port = "5684";

    iot_security_param->psk_Id = g_endpoint_name_iots;
    iot_security_param->psk = (char *)g_psk_iot_value;
    iot_security_param->psk_len = sizeof(g_psk_iot_value);

    bs_security_param->psk_Id = g_endpoint_name_bs;
    bs_security_param->psk = (char *)g_psk_bs_value;
    bs_security_param->psk_len = sizeof(g_psk_bs_value);
#endif    
    iot_security_param->server_port = "5684";
    bs_security_param->server_port = "5684";

    iot_security_param->security_type = CLOUD_SECURITY_TYPE_CA;
    iot_security_param->cloud_security_u.ca.ca_crt = LWM2M_TEST_CA_CRT;
    iot_security_param->cloud_security_u.ca.client_crt = LWM2M_TEST_CLI_CRT;
    iot_security_param->cloud_security_u.ca.client_key = LWM2M_TEST_CLI_KEY;

	bs_security_param->security_type = CLOUD_SECURITY_TYPE_PSK;
    bs_security_param->cloud_security_u.psk.psk_Id = g_endpoint_name_bs;
    bs_security_param->cloud_security_u.psk.psk = (char *)g_psk_bs_value;
    bs_security_param->cloud_security_u.psk.psk_len = sizeof(g_psk_bs_value);
#else
    iot_security_param->server_port = "5683";
    bs_security_param->server_port = "5683";

    iot_security_param->security_type = CLOUD_SECURITY_TYPE_NONE;

    bs_security_param->security_type = CLOUD_SECURITY_TYPE_NONE;
#endif

    if(ATINY_OK != atiny_init(atiny_params, &g_phandle))
    {
        return;
    }

//    uwRet = creat_report_task();
    if((pthread_create(&tidp, NULL, app_data_report, NULL)) == -1)
    {
        return;
    }

    (void)atiny_bind(device_info, g_phandle);
}
