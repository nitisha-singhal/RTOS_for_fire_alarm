 #include <Arduino_FreeRTOS.h> 
#include <croutine.h> 
#include <event_groups.h> 
#include <FreeRTOSConfig.h> 
#include <FreeRTOSVariant.h> 
#include <list.h> 
#include <message_buffer.h> 
#include <mpu_wrappers.h> 
#include <portable.h> 
#include <portmacro.h> 
#include <projdefs.h> 
#include <queue.h> 
#include <semphr.h> 
#include <stack_macros.h> 
#include <stream_buffer.h> 
#include <task.h> 
#include <timers.h> 
 
#define alarm_pin 6 
#define led_pin 13 
#define flame_sensor_pin 8 
#define gas_sensor_pin A0 
# define sensor_thresh 1000 
 
SemaphoreHandle_t BinarySemaphore1; SemaphoreHandle_t BinarySemaphore2; 
 
 
void setup() {   Serial.begin(9600);   xTaskCreate( 
    FlameSensor 
    ,  (const portCHAR *)"Handler 1"   // A name just for humans 
    ,  200  // This stack size can be checked & adjusted by reading the Stack Highwater     ,  NULL 
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. 
    ,  NULL ); 
 
   xTaskCreate( 
    GasSensor 
    ,  (const portCHAR *)"Blink"   // A name just for humans 
    ,  200  // This stack size can be checked & adjusted by reading the Stack Highwater     ,  NULL 
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. 
    ,  NULL ); 
 
   xTaskCreate( 
    FireSuppression 
    ,  (const portCHAR *)"Blink"   // A name just for humans 
    ,  200  // This stack size can be checked & adjusted by reading the Stack Highwater     ,  NULL 
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. 
    ,  NULL ); 
 
   xTaskCreate( 
    ActivateCooler 
    ,  (const portCHAR *)"Blink"   // A name just for humans 
    ,  200  // This stack size can be checked & adjusted by reading the Stack Highwater     ,  NULL 
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.     ,  NULL );   vSemaphoreCreateBinary(BinarySemaphore1);   vSemaphoreCreateBinary(BinarySemaphore2); 
   
} 
 
void loop() { 
  Serial.println("Ideal Task");   delay(500); 
} 
 
void ActivateCooler (void *pvParameters) 
{ 
  pinMode(led_pin,OUTPUT); 
  for (;;) 
  { 
    if (xSemaphoreTake(BinarySemaphore2,999999999)) 
    {       for (int i=0;i<9;i++) 
      { 
        digitalWrite(led_pin,LOW); 
      } 
    } 
  } 
} 
 
void FireSuppression (void *pvParameters) 
{ 
  pinMode(led_pin,OUTPUT);   pinMode(alarm_pin,OUTPUT); 
    for (;;) 
    { 
      if (xSemaphoreTake(BinarySemaphore1,9999999999)){         digitalWrite(alarm_pin,HIGH);         digitalWrite(led_pin,HIGH); 
      } 
    } 
} 
 
void FlameSensor (void *pvParameters) 
{   for (;;) 
  { 
  pinMode(flame_sensor_pin,INPUT);   int p = digitalRead(flame_sensor_pin);   if (p == 0) 
  { 
    Serial.println("\nFire Detected!!! activating cooler and alarm !!");     xSemaphoreGiveFromISR(BinarySemaphore1,NULL);     vTaskDelay(200); 
  }   else  
  { 
    digitalWrite(led_pin,LOW);     digitalWrite(alarm_pin,LOW); 
     
  } 
  } 
} 
