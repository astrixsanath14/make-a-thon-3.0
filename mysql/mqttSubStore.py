import paho.mqtt.client as mqtt
# -------------------------------------------------------
# mysql imports
from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String

# mysql definitions
engine = create_engine(
    'mysql://root:thisroot@localhost/jonazar', encoding='utf-8', echo=False)
Base = declarative_base()


class TRACK(Base):
    __tablename__ = 'TRACK'
    id = Column(Integer, primary_key=True)
    SNo = Column(String(255))
    UUID = Column(String(255))
    SessionNumber = Column(String(255))
    EnterTime = Column(String(255))
    ExitTime = Column(String(255))
    SessionAT = Column(String(255))
    SessionIT = Column(String(255))
    TotalAT = Column(String(255))
    TotalInTime = Column(String(255))
    LastActivityStart = Column(String(255))
    LastActivityStop = Column(String(255))
    # Merchant_Id = Column(String(255))
    # BillDesk_Dispute_Reference_Number = Column(String(255))
    # Bank_Case_Number = Column(String(255))
    # Bank_Name = Column(String(255))
    # Payment_Id = Column(String(255))
    # Transaction_Reference_Number = Column(String(255))
    # Authenticator_1 = Column(String(255))
    # Authenticator_2 = Column(String(255))
    # Authenticator_3 = Column(String(255))
    # Transaction_Amount = Column(String(255))
    # Transaction_Date = Column(String(255))
    # Dispute_Reason = Column(String(255))
    # Target_Date = Column(String(255))


# MQTT Settings
MQTT_Broker = "iot.eclipse.org"
MQTT_Port = 1883
Keep_Alive_Interval = 45
MQTT_Topic = "instaiscool12345"
# MQTT_Topic = "outsinhala123"

# Subscribe to all Sensors at Base Topic


def on_connect(self, mosq, obj, rc):
    mqttc.subscribe(MQTT_Topic, 0)

# Save Data into DB Table


def on_message(mosq, obj, msg):
    # This is the Master Call for saving MQTT Data into DB
    print("MQTT Data Received...")
    print("MQTT Topic: " + msg.topic)
    # print("Data: " + msg)
    message = (msg.payload).decode("utf-8")
    print("here 1")
    # print "MQTT Data Received..."
    # print "MQTT Topic: " + msg.topic
    # print "Data: " + msg.payload
    pushToDatabase(msg.topic, message)
    # sensor_Data_Handler(msg.topic, msg.payload)


def on_subscribe(mosq, obj, mid, granted_qos):
    print("subscribedddd.... ")
    pass


def pushToDatabase(topic, content):
    if topic != "instaiscool12345":
        print("wrong topic")
        return 0
    print("here 2")
    print(content)
    Base.metadata.create_all(engine)
    print("here 3")
    from sqlalchemy.orm import sessionmaker
    print("here 4")
    Session = sessionmaker(bind=engine)
    print("here 5")
    session = Session()
    print("pushes to database ...: ")
    # --------------------------------------------------------------
    session.bulk_insert_mappings(TRACK, list(content))
    session.commit()
    return 1


mqttc = mqtt.Client()

# Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe

# Connect
mqttc.connect(MQTT_Broker, int(MQTT_Port), int(Keep_Alive_Interval))

# Continue the network loop
mqttc.loop_forever()
