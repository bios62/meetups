
rem
rem Run this as admin, if WEB or ORDS is note enabled with database actions usermanagement
rem
DECLARE
  PRAGMA AUTONOMOUS_TRANSACTION;
BEGIN
    ORDS.ENABLE_SCHEMA(p_enabled => TRUE,
                       p_schema => 'SENSORDATA',
                       p_url_mapping_type => 'BASE_PATH',
                       p_url_mapping_pattern => 'sensorapi',
                       p_auto_rest_auth => FALSE);
    commit;
END;
/
rem   
rem to be run as schema owner
rem
reem if needed get rid of old stuff
remdrop table sensors; 
rem create table to hold non json sensor database
rem
create table sensors (id NUMBER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) primary key,
                      create_time timestamp default systimestamp,
                      objectname varchar2(40),
                      sensorname varchar2(20),
                      sensorvalue varchar2(20));
rem
rem add some dummy data to verify the GET request
rem
begin
insert into sensors (objectname,sensorname,sensorvalue) values('test','temp','22.0');
insert into sensors (objectname,sensorname,sensorvalue) values('test','temp','23.0');
insert into sensors (objectname,sensorname,sensorvalue) values('test','temp','24.0');
commit;
end;
/
rem
rem define the template
rem     
begin
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
ORDS.DEFINE_TEMPLATE(
      p_module_name    => 'sensors',
      p_pattern        => 'iotapi/:objectname',
      p_priority       => 0,
      p_etag_type      => 'HASH',
      p_etag_query     => NULL,
      p_comments       => NULL);
commit;
end;
/
rem
rem define handlers
rem
begin
ORDS.DEFINE_HANDLER(
      p_module_name    => 'sensors',
      p_pattern        => 'iotapi/',
      p_method         => 'GET',
      p_source_type    => 'json/collection',
      p_items_per_page =>  0,
      p_mimes_allowed  => '',
      p_comments       => NULL,
      p_source         => 
'select create_time,sensorname, sensorvalue from sensors'
      );
commit;
end;
/
rem
rem define handlers
rem
begin
ORDS.DEFINE_HANDLER(
      p_module_name    => 'sensors',
      p_pattern        => 'iotapi/:objectname',
      p_method         => 'GET',
      p_source_type    => 'json/collection',
      p_items_per_page =>  0,
      p_mimes_allowed  => '',
      p_comments       => NULL,
      p_source         => 
'select objectname,create_time,sensorname, sensorvalue from sensors where objectname=:objectname'
      );  
commit;
end;
/
rem
rem define handlers
rem
begin
  ORDS.DEFINE_HANDLER(
      p_module_name    => 'sensors',
      p_pattern        => 'iotapi/',
      p_method         => 'POST',
      p_source_type    => 'plsql/block',
      p_items_per_page =>  0,
      p_mimes_allowed  => 'application/json',
      p_comments       => NULL,
      p_source         => 
'declare
  id sensors.id%type;
BEGIN
  INSERT INTO sensors(objectname, sensorname, sensorvalue)
     VALUES (:objectname,:sensorname,:sensorvalue)
  RETURNING ID INTO id;
  :status := 201;
END;'
    );
  COMMIT; 
END;
/
rem
rem  test with postman GET on https://<cloud tenant url>/ords/<schema name>/sensors/iotapi/
rem

