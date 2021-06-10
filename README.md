# Edge to ML meetups
Collection of material from techAngelist, by Oracle meetups

10/6-2021 material for the From Edge to ML, Demo and Lab,  Auto ML

- iot_oml_apex_lab3.pdf
- lab3_scripts.zip
- 
You will need the following files for this day 3 lab:


27/5-2021 material for the From Edge to ML, Demo and Lab,  Auto ML

The following files is needed for the day 2 lab:

- WS_Postman.zip, postman collections and environment for calling the ML REST API
- OML_AutoML_2705.pdf, presentation of 27/6

11/5-2021 From Edge to ML Arduino hands on lab
- Lab for setting up and preparing the databvase for REST ingestion of sensor data
-      EE2ML110421.pdf
- Lab for preparing your computer for Arduino IDE
-      Arduino20210511.pdf
- Source code for PL/SQL 
-      plsql_lab_110521.sql
- Lab for building the code scanning the sensor and ingesting into Oracle ATP over REST
- Arduino Source code
-      meetup20210511prod.ino

The lab require some parts form either digikey or mouser.
You may order the following parts from Mouser:
https://no.mouser.com/ProductDetail/Adafruit/3619?qs=sGAEpiMZZMu3sxpa5v1qrlKzXlDUmWhOUpteeGygzU0%3D

https://no.mouser.com/ProductDetail/Seeed-Studio/103100062?qs=uwxL4vQweFOCqRcwBc9i%2FQ%3D%3D

https://no.mouser.com/ProductDetail/Adafruit/4528?qs=OlC7AqGiEDmHEtHHQiIw7w%3D%3D

https://no.mouser.com/ProductDetail/Adafruit/3660?qs=W0yvOO0ixfFypXCClAyRMg%3D%3D


For the code to run in your envrionment, you need to change
- const char* ssid="yourssid";
- const char* password="your wifi password"

If you will use SODA interface

//#define SODA needs to be changed to #define SODA

String sodaUrl="Your SODA API URL";
String uidSODA="database username";
String pwdSODA="Database Password";


If you use traditional REST ORDS interface
Make sure //#define SODA is unset
change
String url="Your ORDS URL";



Recent changes:

meetup20210511prod.ino:
Updated the arduino soruce code, removed usage of explisitt HTTPS Certificate. Latest
ArduinoHTTPClient is compiled with sufficient certificates.

plsql_lab_110521.sql
Changed objectname column length to 40 in senors table to have space for the full objectname 
generated by the arduino soruce


The REST API URL is made up of the follwoing elements:
URL to your OCI Console + /ords/+scemaname+basepath derived from ORDS.DEFINE_MODULE + value of p_pattern 

As anexample assume your UTL to OCI is https://tn1tv18ynzxubz6-iosp.adb.eu-frankfurt-1.oraclecloudapps.com/
with the follwoing template:

ORDS.DEFINE_MODULE(
      p_module_name    => 'sensors',
      p_base_path      => 'sensors',
      p_items_per_page =>  0,
      p_status         => 'PUBLISHED',
      p_comments       => NULL);     
ORDS.DEFINE_TEMPLATE(
      p_module_name    => 'sensors',
      p_pattern        => 'iotapi/',
      p_priority       => 0,
      p_etag_type      => 'HASH',
      p_etag_query     => NULL,
      p_comments       => NULL);
      
 URL:to OCI Console: https://tn1tv18ynzxubz6-iosp.adb.eu-frankfurt-1.oraclecloudapps.com/
 DB Schema:  sensordata
 Template module name:  sensors
 pattern:  iotapi/
 
 The full URL for user sensordata will then be:
 "https://tn1tv18ynzxubz6-iosp.adb.eu-frankfurt-1.oraclecloudapps.com/ords/sensordata/sensors/iotapi/
