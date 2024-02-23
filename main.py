import paho.mqtt.client as mqtt
import serial
import os
import time

broker_endpoint = "<insert you broker endpoint here>"
broker_port = 8883 # use mqtts port

# create a folder named "certificates" inside the cloned directory use these file paths
# you can also customized the name of the folder of the certificates and the certificates name here 
certificate_directory = "certificates"
ca_certs_path = os.path.join(certificate_directory, "AmazonRootCA1.pem")
certfile_path = os.path.join(certificate_directory, "certificate.pem.crt")
keyfile_path = os.path.join(certificate_directory, "private.pem.key")

# define the topic used in the project
raspi_pub_topic = "esp32/button"
raspi_sub_topic = "esp32/led"

# define callback functions for mqtt client
def on_connect(client, userdata, flags, reason_code, properties):
  print(f"Connected to {client} with reason code {reason_code}")
  client.subscribe("esp32/pushButton")
  client.subscribe("esp32/led")
  
# You can enable this if you want to see the log
# def on_log(client, userdata, level, buf):
#   print("log: ",buf)
  
def on_message(client, userdata, msg, ser):
  topic = msg.topic
  payload = msg.payload.decode('utf-8')
  print("Topic", topic)
  print("payload", payload)
  
  if topic == raspi_sub_topic:
    print("Pesan dari topic esp32/led")
    print("\nLed State : ", payload)
    if payload == "0":
      ser.write("0".encode())
    elif payload == "1":
      ser.write("1".encode())

def main():  
  # catch the error on serial connection
  try:
    ser = serial.Serial('/dev/ttyUSB0', 115200)
  except serial.SerialException as error:
    print(f"Error: {error}")
    return
  
  client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
  client.on_connect = on_connect
  # client.on_log=on_log
  client.on_message = lambda client, userdata, msg: on_message(client, userdata, msg, ser)

  print("Connecting to broker : ");
  client.tls_set(ca_certs=ca_certs_path, certfile=certfile_path, keyfile=keyfile_path)
  client.tls_insecure_set(True)

  # catch the error on mqtt connection
  try:
    client.connect(broker_endpoint, broker_port, 300)
  except mqtt.MQTTException as error:
    print(f"Error: {error}")
    ser.close()
    return
  
  client.loop_start()

  while True:
    pushButton = ser.readline().decode()
    print("State Push Button", pushButton)      
    client.publish(raspi_pub_topic, pushButton)
  
  ser.close()
  
if __name__ == "__main__":
  main()