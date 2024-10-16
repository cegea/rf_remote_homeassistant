#ifndef mqtt_client_h
#define mqtt_client_h

#define TOPIC_TEST_UP "test/up"
#define TOPIC_TEST_DOWN "test/down"
#define TOPIC_COMMAND_DELETE_CREDENTIALS "remote/command/delete_credentials"
#define TOPIC_COMMAND_REBOOT "remote/command/reboot"
#define TOPIC_STATUS "remote/status"
#define TOPIC_INIT_SINCE "remote/init_since"
#define TOPIC_RF "remote/rf"
#define TOPIC_ERROR_UNKNOWN_CMD "remote/error/unknown_cmd"

void setup_mqtt();
void loop_mqtt();
void MQTT_publish_time_since_init(void);
void MQTT_command_server(char *topic);

#endif