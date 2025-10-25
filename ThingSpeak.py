import pmt
import requests
from gnuradio import gr

class blk(gr.basic_block):  
    def __init__(self):
        gr.basic_block.__init__(
            self,
            name="LoRa Packet Receiver",
            in_sig=[],
            out_sig=[]
        )

        self.message_port_register_in(pmt.intern("in"))
        self.set_msg_handler(pmt.intern("in"), self.handle_msg)

        self.node_a_temp = None
        self.node_a_hum = None
        self.node_a_light = None
        self.node_b_temp = None
        self.node_b_hum = None
        self.node_b_light = None

        self.api_key = "9IX44TQJ9WLVQ37V"

    def send_node_a_to_thingspeak(self):
        payload = {
            "api_key": self.api_key,
            "field1": self.node_a_temp,
            "field2": self.node_a_hum,
            "field3": self.node_a_light
        }
        try:
            response = requests.post("https://api.thingspeak.com/update", data=payload, timeout=5)
            if response.status_code == 200:
                print("[ThingSpeak] Node A data sent.")
            else:
                print(f"[ThingSpeak] Node A failed. Status: {response.status_code}")
        except Exception as e:
            print(f"[ThingSpeak] Node A error: {e}")

    def send_node_b_to_thingspeak(self):
        payload = {
            "api_key": self.api_key,
            "field4": self.node_b_temp,
            "field5": self.node_b_hum,
            "field6": self.node_b_light
        }
        try:
            response = requests.post("https://api.thingspeak.com/update", data=payload, timeout=5)
            if response.status_code == 200:
                print("[ThingSpeak] Node B data sent.")
            else:
                print(f"[ThingSpeak] Node B failed. Status: {response.status_code}")
        except Exception as e:
            print(f"[ThingSpeak] Node B error: {e}")

    def handle_msg(self, msg):
        print(">>> Python block received something!")

        try:
            decoded = pmt.symbol_to_string(msg).strip()
            print(f"[LoRa Rx] Raw: {decoded}")

            parts = decoded.split()
            i = 0
            while i < len(parts) - 1:
                if parts[i].endswith(":"):
                    key = parts[i][:-1]
                    val_str = parts[i+1]

                    try:
                        val = float(val_str)

                        if key == "NAT":
                            self.node_a_temp = val
                            print(f"[Node A] Temp: {val}")
                        elif key == "NAH":
                            self.node_a_hum = val
                            print(f"[Node A] Hum: {val}")
                        elif key == "NAL":
                            self.node_a_light = val
                            print(f"[Node A] Light: {val}")
                        elif key == "NBT":
                            self.node_b_temp = val
                            print(f"[Node B] Temp: {val}")
                        elif key == "NBH":
                            self.node_b_hum = val
                            print(f"[Node B] Hum: {val}")
                        elif key == "NBL":
                            self.node_b_light = val
                            print(f"[Node B] Light: {val}") 
                        else:
                            print(f"[LoRa Rx] Unknown key: {key}")

                    except ValueError:
                        print(f"[LoRa Rx] Invalid value for {key}: {val_str}")
                    i += 2
                else:
                    print(f"[LoRa Rx] Unexpected token: {parts[i]}")
                    i += 1

            # Send Node A data if both values are available
            if self.node_a_temp is not None and self.node_a_hum is not None and self.node_a_light is not None:
                self.send_node_a_to_thingspeak()
                self.node_a_temp = None
                self.node_a_hum = None
                self.node_a_light = None


            # Send Node B data if both values are available
            if self.node_b_temp is not None and self.node_b_hum is not None and self.node_b_light is not None:
                self.send_node_b_to_thingspeak()
                self.node_b_temp = None
                self.node_b_hum = None
                self.node_b_light= None

        except Exception as e:
            print(f"[LoRa Rx] Error: {e}")

