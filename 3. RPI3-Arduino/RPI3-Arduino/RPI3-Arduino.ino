int incomingByte = 0;
const int bSize = 128; 

char Buffer[bSize];
char Data[bSize];

int ByteCount;
bool LEDmode = false;


void SerialParser(void) {
  ByteCount = -1;
  ByteCount =  Serial.readBytesUntil('\0',Buffer,bSize);  

  if(ByteCount > 0) {                    
    if(strcmp((char *)Buffer,"POWER:ON") == 0){
      strcpy(Data,Buffer);  
      LEDmode = true;
      Serial.println("Received");
    }
    else if(strcmp((char *)Buffer,"POWER:OFF") == 0){
      strcpy(Data,Buffer);       
      LEDmode = false;
      Serial.println("Received");
    }
    else{
      memset(Data, 0, bSize);      
    }
  }
  memset(Buffer, 0, bSize);     
  Serial.flush();
}

void setup() {
  Serial.begin(115200);    
  pinMode(13,OUTPUT);
}

void loop() {
  SerialParser();
  if(LEDmode){
    digitalWrite(13,HIGH);
  }
  else{
    digitalWrite(13,LOW);
  }
}
