#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "./JSON_Kernel/cJSON.h"

#include "emotor.h"

#define d_FILE_PATH             "./fs_data/test_config"

void doit(char *text)
{
    char *out;
    cJSON *json;

    json=cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else {
        out=cJSON_Print(json);
        cJSON_Delete(json);
        printf("%s\n",out);
        free(out);
    }
}

void dofile(char *filename)
{
    FILE *f;
    long len;
    char *data;

    f=fopen(filename,"rb");

    fseek(f,0,SEEK_END);
    len=ftell(f);

    fseek(f,0,SEEK_SET);
    data=(char*)malloc(len+1);fread(data,1,len,f);data[len]='\0';fclose(f);

    doit(data);
    free(data);
}

void configTest(void) {
    cJSON *config;
    char *out;
    FILE *pFile = NULL;
    DIR *pFolder = NULL;
    
    config = cJSON_CreateObject();
    cJSON_AddItemToObject(config, "IP", cJSON_CreateString("127.0.0.1"));
    cJSON_AddItemToObject(config, "Port", cJSON_CreateNumber(12345));
    
    out = cJSON_Print(config);
    cJSON_Delete(config);
    
    pFolder = opendir("./fs_data");
    if (pFolder == NULL) {
        system("mkdir ./fs_data");
    }
    else {
        closedir(pFolder);
    }
    
    pFile = fopen(d_FILE_PATH, "w+");    
    fprintf(pFile, "%s", out);
    fclose(pFile);
    
    free(out);
    
    dofile(d_FILE_PATH);
}


char *initPayload = "{"
  "\"jb_udp\": \"emwjuicebox.cloudapp.net:8042\","
  "\"jb_https\": \"emwjuicebox.cloudapp.net:443\","
  "\"api_base\": \"http://private-3a806b-uptapimocks.apiary-mock.com/v1/cstls/\","
  "\"profile\": {"
    "\"level\": \"device type default\","
    "\"id\": \"2a4d4706-b83d-4d77-bd54-a0b816d07f9f\","
    "\"name\": \"Cstls profile\"}"
"}";

char *runPayload = "{"
    "\"language\": \"en\","
    "\"region\": \"us\","
    "\"currency\": \"USD\","
    "\"rates\": [{\"start\":1519801200,\"display\":\"$3.00/hour + $.20/kWh\"},{\"start\":1520801200,\"display\":\"FREE\"}]"
"}";

char *evChargePayload="{"
    "\"token\": \"123XYZ\","
    "\"status\": \"finished\","
    "\"amount\": 5.37,"
    "\"currency\": \"USD\""
"}";

char *evChargeSettlePayload="{"
    "\"token\": \"123XYZ\","
    "\"status\": \"finished\","
    "\"amount\": 5.37,"
    "\"currency\": \"USD\""
"}";

void CJOSN_FreeInit(juicenet_init_fmt *init)  {
    if(init != NULL)    {
        free(init->api_base);
        free(init->jb_udp);
        free(init->jb_https);
        free(init->id);
        free(init->level);
        free(init->name);
    }
}

void CJSON_FreeRun(juicenet_run_fmt *run)    {
    if(run != NULL)  {
        free(run->rates[0].display);
        free(run->rates[1].display);
        free(run->language);
        free(run->currency);
        free(run->region);
    }
}

void CJSON_FreeEvCharge(juicenet_ev_charge_fmt *ev_charge)   {
    if(ev_charge != NULL)   {
        free(ev_charge->currency);
        free(ev_charge->status);
        free(ev_charge->token);
    }
}

void CJSON_FreeEvSettle(juicenet_ev_settle_fmt *ev_settle)  {
    if(ev_settle)   {
        free(ev_settle->currency);
        free(ev_settle->status);
        free(ev_settle->token);
    }
}

static unsigned long CJSON_GetInitResponse(CHAR  *payload, juicenet_init_fmt *init) {
    unsigned long ulRet=0;
    cJSON *cData, *cSubData, *cSubDataAll;
    
    if(payload == NULL) {
        printf("payload is null !\n");
        ulRet = 1;
        return ulRet;
    }
    
    cData = cJSON_Parse(payload);
    if(cData == NULL ) {
        printf("Json Parse Fail !\n");
        ulRet = 2;
        return ulRet;
    }
    printf("=== JuiceNet Inital Response ===\n");
    cSubData = cJSON_GetObjectItem(cData, d_KEY_INIT_JB_UDP);
//    if(cSubData == NULL) {
//       //to do
//    }
    init->jb_udp = (CHAR *)malloc(strlen(cSubData->valuestring));
    memcpy(init->jb_udp, cSubData->valuestring, strlen(cSubData->valuestring));
    printf("%s : %s\n", d_KEY_INIT_JB_UDP,  init->jb_udp);
    
    cSubData = cJSON_GetObjectItem(cData, d_KEY_INIT_JB_HTTPS);
//    if(cSubData == NULL) {
//       //to do
//    }
    init->jb_https = (CHAR *)malloc(strlen(cSubData->valuestring));
    memcpy(init->jb_https, cSubData->valuestring, strlen(cSubData->valuestring));
    printf("%s : %s\n", d_KEY_INIT_JB_HTTPS, init->jb_https);
    
    cSubData = cJSON_GetObjectItem(cData, d_KEY_INIT_API_BASE);
//    if(cSubData == NULL) {
//       //to do
//    }
    init->api_base = (CHAR *)malloc(strlen(cSubData->valuestring));
    memcpy(init->api_base, cSubData->valuestring, strlen(cSubData->valuestring));
    printf("%s : %s\n", d_KEY_INIT_API_BASE, init->api_base);
    //Get profile data
    cSubData = cJSON_GetObjectItem(cData, d_KEY_INIT_PROFILE);
    cSubDataAll = cJSON_GetObjectItem(cSubData, d_KEY_INIT_PROFILE_LEVEL);
//    if(cSubData == NULL) {
//       //to do
//    }
    init->level = (CHAR *)malloc(strlen(cSubDataAll->valuestring));
    memcpy(init->level, cSubDataAll->valuestring,  strlen(cSubDataAll->valuestring));
    printf("%s : %s\n", d_KEY_INIT_PROFILE_LEVEL, init->level);
    
    cSubDataAll = cJSON_GetObjectItem(cSubData, d_KEY_INIT_PROFILE_ID);
//    if(cSubData == NULL) {
//       //to do
//    }
    init->id = (CHAR *)malloc(strlen(cSubDataAll->valuestring));
    memcpy(init->id, cSubDataAll->valuestring,  strlen(cSubDataAll->valuestring));
    printf("%s : %s\n", d_KEY_INIT_PROFILE_ID, init->id);
    
    cSubDataAll = cJSON_GetObjectItem(cSubData, d_KEY_INIT_PROFILE_NAME);
//    if(cSubData == NULL) {
//       //to do
//    }
    init->name = (CHAR *)malloc(strlen(cSubDataAll->valuestring));
    memcpy(init->name, cSubDataAll->valuestring, strlen(cSubDataAll->valuestring));
    printf("%s : %s\n", d_KEY_INIT_PROFILE_NAME, init->name);

    cJSON_Delete(cData);
    CJOSN_FreeInit(init);
    
    return ulRet;
}

static unsigned long CJSON_GetRunResponse(CHAR  *payload, juicenet_run_fmt *run) {
    unsigned long ulRet=0;
    cJSON *cData, *cSubData;
    int i;
    
    if(payload == NULL) {
        printf("payload is null !\n");
        ulRet = 1;
        return ulRet;
    }
    cData = cJSON_Parse(payload);
    if(NULL == cData) {
        printf("Json Parse Fail !\n");
        ulRet = 2;
        return ulRet;
    }
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("=== JuiceNet Run Response ===\n");
    cSubData = cJSON_GetObjectItem(cData, d_KEY_RUN_REGION);
    run->region = (CHAR *)malloc(strlen(cSubData->valuestring));
    memcpy(run->region, cSubData->valuestring, strlen(cSubData->valuestring));
    printf("%s : %s\n", d_KEY_RUN_LANGUAGE, run->region);
    cSubData = cJSON_GetObjectItem(cData, d_KEY_RUN_REGION);
//    if(cSubData == NULL) {
//       //to do
//    }
    run->region = (CHAR *)malloc(strlen(cSubData->valuestring));
    memcpy(run->region, cSubData->valuestring, strlen(cSubData->valuestring));
    printf("%s : %s\n", d_KEY_RUN_REGION, run->region);
    cSubData = cJSON_GetObjectItem(cData, d_KEY_RUN_CURRENCY);
//    if(cSubData == NULL) {
//       //to do
//    }
    run->currency = (CHAR *)malloc(strlen(cSubData->valuestring));
    memcpy(run->currency, cSubData->valuestring, strlen(cSubData->valuestring));
    printf("%s : %s\n", d_KEY_RUN_CURRENCY, run->currency);
    //Get rates data
    cSubData = cJSON_GetObjectItem(cData, d_KEY_RUN_RATES); 
//    if(cSubData == NULL) {
//       //to do
//    }
    int arrSize = cJSON_GetArraySize(cSubData);
    for(i = 0; i < arrSize; i++) {
        cJSON *cSubDataAll = cJSON_GetArrayItem(cSubData, i);
         if(cSubDataAll == NULL)  {
             continue;
         }
         printf("Run Rates[%d]\n", i+1);
         cJSON *cRates = cJSON_GetObjectItem(cSubDataAll, d_KEY_RUN_RATES_START);
 //        if(cRates == NULL)   {
 //        //to do
 //        }
         run->rates[i].start = cRates->valueint;
         printf("%s : %d\n", d_KEY_RUN_RATES_START, run->rates[i].start);
         cRates = cJSON_GetObjectItem(cSubDataAll, d_KEY_RUN_RATES_DISPLAY);
 //        if(cRates == NULL)   {
 //        //to do
 //        }
         run->rates[i].display = (CHAR *)malloc(strlen(cSubDataAll->valuestring));
         memcpy(run->rates[i].display, cRates->valuestring, strlen(cRates->valuestring));
         printf("%s : %s\n", d_KEY_RUN_RATES_DISPLAY, run->rates[i].display);
     }
    cJSON_Delete(cData);
    CJSON_FreeRun(run);
    
    return ulRet;
}

static unsigned long CJSON_GetEvChargeResponse(CHAR  *payload) {
    unsigned long ulRet=0;
    cJSON *cData, *cSubData;
    
    if(payload == NULL) {
        printf("payload is null !\n");
        ulRet = 1;
        return ulRet;
    }
    cData = cJSON_Parse(payload);
    if(NULL == cData) {
        printf("Json Parse Fail !\n");
        ulRet = 2;
        return ulRet;
    }
    printf("=== JuiceNet EV Charge Response ===\n");
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_CHARGE_AUTH_TOKEN);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("%s : %s\n", d_KEY_EV_CHARGE_AUTH_TOKEN, cSubData->valuestring);
    
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_CHARGE_AUTH_STATUS);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("%s : %s\n", d_KEY_EV_CHARGE_AUTH_STATUS, cSubData->valuestring);
    
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_CHARGE_AUTH_AMOUNT);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("%s : %d\n", d_KEY_EV_CHARGE_AUTH_AMOUNT, cSubData->valueint);
    
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_CHARGE_AUTH_CURRENCY);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("%s : %s\n", d_KEY_EV_CHARGE_AUTH_CURRENCY, cSubData->valuestring);

    cJSON_Delete(cData);
            
    return ulRet;
}

static unsigned long CJSON_GetEvSettleResponse(CHAR  *payload) {
    unsigned long ulRet=0;
    cJSON *cData, *cSubData;

    if(payload == NULL) {
        printf("payload is null !\n");
        ulRet = 1;
        return ulRet;
    }
    cData = cJSON_Parse(payload);
    if(NULL == cData) {
        printf("Json Parse Fail !\n");
        ulRet = 2;
        return ulRet;
    }
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_SETTLEMENT_TOKEN);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("=== JuiceNet EV Settle Response ===\n");
    printf("%s : %s\n", d_KEY_EV_SETTLEMENT_TOKEN, cSubData->valuestring);
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_SETTLEMENT_STATUS);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("%s : %s\n", d_KEY_EV_SETTLEMENT_STATUS, cSubData->valuestring);
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_SETTLEMENT_AMOUNT);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("%s : %d\n", d_KEY_EV_SETTLEMENT_AMOUNT, cSubData->valueint);
    cSubData = cJSON_GetObjectItem(cData, d_KEY_EV_SETTLEMENT_CURRENCY);
//    if(cSubData == NULL) {
//       //to do
//    }
    printf("%s : %s\n", d_KEY_EV_SETTLEMENT_CURRENCY, cSubData->valuestring);

    cJSON_Delete(cData);
 
    return ulRet;
}

juicenet_init_fmt Init;
juicenet_run_fmt Run;
juicenet_ev_charge_fmt EvCharge;
juicenet_ev_settle_fmt EvSettle;
 

int main (int argc, const char * argv[]) {
    
    CJSON_GetInitResponse(initPayload, &Init);
    
    //CJSON_GetRunResponse(runPayload, &Run);
    
    CJSON_GetEvChargeResponse(evChargePayload);
    
    CJSON_GetEvSettleResponse(evChargeSettlePayload);
    
    //configTest();
 
 
	//Issue-1
	
	//Issue-2
    return 0;
}