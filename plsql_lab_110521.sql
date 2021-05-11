drop table sensors; 

create table sensors (id NUMBER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) primary key,
                      create_time timestamp default systimestamp,
                      objectname varchar2(20),
                      sensorname varchar2(20),
                      sensorvalue varchar2(20));


begin
insert into sensors (objectname,sensorname,sensorvalue) values('test','temp','22.0');
insert into sensors (objectname,sensorname,sensorvalue) values('test','temp','23.0');
insert into sensors (objectname,sensorname,sensorvalue) values('test','temp','24.0');
commit;
end;
/

DECLARE
  PRAGMA AUTONOMOUS_TRANSACTION;
BEGIN
    ORDS.ENABLE_OBJECT(p_enabled => TRUE,
                       p_schema => 'SENSORDATA',
                       p_object => 'SENSORS',
                       p_object_type => 'TABLE',
                       p_object_alias => 'sensorapi',
                       p_auto_rest_auth => FALSE);

    commit;

END;
/

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
'select objecname,create_time,sensorname, sensorvalue from sensors where objectname=:objectname'
      );  
commit;
end;
/

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
commit;
end;
/


