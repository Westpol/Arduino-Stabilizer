    //Use the pot as the gain for all channels for now
    float GainPot = (float)(TxVal[2]) * 0.001f;

    //Get the target values from the TX
    int PitchTarg = (TxVal[3] / 10);
    int RollTarg = (TxVal[4] / 10);
    int YawTarg = (TxVal[6] / 10);

    //Prime the Target WOZ values
    if(PitchTargWOZ == 9999)
    PitchTargWOZ = PitchTarg;

    if(RollTargWOZ == 9999)
    RollTargWOZ = RollTarg;

    if(YawTargWOZ == 9999)
    YawTargWOZ = YawTarg;

    //Get the Centered target values
    float PitchTargCentred = (float)(PitchTarg – PitchTargWOZ);
    float RollTargCentred = (float)(RollTarg – RollTargWOZ);
    float YawTargCentred = (float)(YawTarg – YawTargWOZ);

    //Calculate gains
    float PitchGain = GainPot * 1.0f;
    float RollGain = GainPot * 1.0f;
    float YawGain = GainPot * 1.0f;

    //Get Gyro values
    float PitchGyro = (float)(AnIn[2] – AnInWOZ[2]);
    float RollGyro = (float)(AnIn[1] – AnInWOZ[1]);
    float YawGyro = (float)(AnIn[0] – AnInWOZ[0]);

    //Calc P error
    float PitchError = (float)PitchTargCentred + PitchGyro;
    float RollError = (float)RollTargCentred + RollGyro;
    float YawError = (float)YawTargCentred + YawGyro;

    //Apply gains
    int PitchTrim = (int)(PitchError * PitchGain);
    int RollTrim = (int)(RollError * RollGain);
    int YawTrim = (int)(YawError * YawGain);

    //Constaring trim authority
    PitchTrim = constrain(PitchTrim, -30, 30);
    RollTrim = constrain(RollTrim, -30, 30);
    YawTrim = constrain(YawTrim, -30, 30);

    //Dump the trim value
    if((TxVal[9] & 0x4) == 0)
    {
    PitchTrim = 0;
    RollTrim = 0;
    YawTrim = 0;
    }

    Here is all the RX the code

    #define MAX_CHAN 12
    #define MAX_IN_STR 200
    #define MAX_SETTINGS 20
    #define MAX_NAV_VALS 50
    #define MAX_SAMPLE 10

    #include

    char buf[255] = {0, };
    String str = “”;
    char *p;

    Servo servo[7]; // create servo object to control a servo
    int val = 0; // variable to read the value from the analog pin
    int AnInWOZ[MAX_CHAN] = {0, };
    int AnIn[MAX_CHAN] = {0, };
    int AnInBuf[MAX_CHAN] = {0, };

    //Get the target values from the TX at rest
    int PitchTargWOZ = 9999;
    int RollTargWOZ = 9999;
    int YawTargWOZ = 9999;

    String inputString = “”; // a string to hold incoming data

    int Sample = 0;
    int TxTemp[MAX_CHAN + 1] = {0, };
    int TxVal[MAX_CHAN + 1] = {0, };

    int NavVal[MAX_NAV_VALS] = {0, };
    int SettingVal[MAX_SETTINGS] = {0, };

    int rssiDur = 0;
    int DigBits = 0;
    int ComState = 0;
    long PacketCount = 0;
    long NoPacketCount = 0;

    //Digital inputs TX code helper
    //TxVal[8] |= (digitalRead(5) << 0);//joy 2 push
    //TxVal[8] |= (digitalRead(6) << 1);//pb
    //TxVal[8] |= (digitalRead(7) << 2);//slide
    //TxVal[8] |= (digitalRead(8) << 3);//toggle

    void setup() {

    // initialize serial:
    Serial.begin(38400);

    // reserve 200 bytes for the inputString:
    inputString.reserve(MAX_IN_STR);

    pinMode(2,INPUT);//rssi
    digitalWrite(2, HIGH);

    servo[0].attach(3); // attaches the servo on pin 3 to the servo object
    servo[1].attach(5); // attaches the servo on pin 5 to the servo object
    servo[2].attach(6); // attaches the servo on pin 6 to the servo object
    servo[3].attach(9); // attaches the servo on pin 9 to the servo object
    servo[4].attach(10); // attaches the servo on pin 10 to the servo object
    servo[5].attach(11); // attaches the servo on pin 11 to the servo object

    NullServos();

    //Get all the analogue signals
    //Do a wind off zero

    for(int i = 0;i < 8;i++)
    AnInWOZ[i] = 0;

    for(int i = 0;i < MAX_SAMPLE;i++){

    AnIn[0] += analogRead(A0);
    AnIn[1] += analogRead(A1);
    AnIn[2] += analogRead(A2);
    AnIn[3] += analogRead(A3);
    AnIn[4] += analogRead(A4);
    AnIn[5] += analogRead(A5);
    AnIn[6] += analogRead(A6);
    AnIn[7] += analogRead(A7);

    delayMicroseconds(10);
    }

    for(int i = 0;i < 8;i++)
    AnInWOZ[i] = (AnIn[i] / MAX_SAMPLE);

    //Prime the WOZ values
    PitchTargWOZ = 9999;
    RollTargWOZ = 9999;
    YawTargWOZ = 9999;

    }

    void loop(){

    //*Get all the analogue signals
    for(int i = 0;i < 8;i++)
    AnInBuf[i] = 0;

    for(int i = 0;i < MAX_SAMPLE;i++){

    AnInBuf[0] += analogRead(A0);
    AnInBuf[1] += analogRead(A1);
    AnInBuf[2] += analogRead(A2);
    AnInBuf[3] += analogRead(A3);
    AnInBuf[4] += analogRead(A4);
    AnInBuf[5] += analogRead(A5);
    AnInBuf[6] += analogRead(A6);
    AnInBuf[7] += analogRead(A7);

    delayMicroseconds(10);
    }

    for(int i = 0;i < 8;i++)
    AnIn[i]= (AnInBuf[i] / MAX_SAMPLE);

    //Capture the Xbee comms
    int CharCount = 0;

    while ((Serial.available()) && ((++CharCount) 0)
    inputString += inChar;

    if(inputString.length() >= MAX_IN_STR)
    break;

    //Detect end of packet
    if ( (inChar == ‘\n’) && (ComState > 0) )
    {
    //Serial.println(inputString);

    //Count packets
    PacketCount++;

    int NumChan = ExtractPacket();

    //Tramsmitter
    if( ComState == 1)
    {
    for(int i = 0 ;i < NumChan;i++)
    TxVal[i] = TxTemp[i];

    DoTelemetery();

    UpdateServos();

    }//Navigator
    else if( ComState == 2)
    {
    for(int i = 0 ;i < NumChan;i++)
    NavVal[i] = TxTemp[i];

    }//Settings
    else if( ComState == 3)
    {
    for(int i = 0 ;i 50)
    {
    NullServos();
    SoftReset();
    }

    //delayMicroseconds(1000);
    }

    int ExtractPacket()
    {

    int Lchk = 0;
    int channel = 0; //initialise the channel count

    p = &inputString[0];

    while ((str = strtok_r(p, “,”, &p)) != NULL) // delimiter is the comma
    {

    TxTemp[channel] = str.toInt(); //use the channel as an index to add each value to the array

    Lchk += TxTemp[channel];

    channel++; //increment the channel

    if(channel > MAX_CHAN)
    break;
    }

    p = NULL;
    inputString = “”;

    //Process in comming data
    if(channel > 2)
    {
    //Remove the remote chk from the total
    Lchk -= TxTemp[channel-2];

    //Checksum
    if((Lchk – TxTemp[channel-2]) == 0)
    return channel;

    }

    return -1;
    }

    void DoTelemetery()
    {

    //Send back a telemetery packet
    if((PacketCount % 5) == 0)
    {
    rssiDur = pulseIn(5, LOW, 200);

    int PacketType = 12;

    //sprintf(buf, “T%02X,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n”, PacketType, rssiDur, PacketCount, NoPacketCount, TxVal[1], TxVal[2], TxVal[3], TxVal[4], TxVal[5], TxVal[6], AnIn[0], AnIn[1], AnIn[2], AnIn[3], AnIn[4], AnIn[5], AnIn[6], AnIn[7], DigBits);
    //sprintf(buf, “T %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n”, PacketType, rssiDur, PacketCount, NoPacketCount, AnIn[0], AnIn[1], AnIn[2], AnIn[3], AnIn[4], AnIn[5], AnIn[6], AnIn[7], DigBits, TxVal[9]);
    sprintf(buf, “T%02X%02X%04X%02X%03X%03X%03X%03X%03X%03X%03X%03X%03X%02X%02X\n”, PacketType, rssiDur, PacketCount, NoPacketCount, AnIn[0], AnIn[1], AnIn[2], AnIn[3], AnIn[4], AnIn[5], AnIn[6], AnIn[7], DigBits, TxVal[9]);

    Serial.write(buf);

    if(digitalRead(13) == false)
    digitalWrite(13, HIGH); // set the LED on
    else
    digitalWrite(13, LOW); // set the LED off

    }
    }

    void UpdateServos()
    {

    //Digital inputs TX code helper
    //TxVal[8] |= (digitalRead(5) << 0);//joy 2 push
    //TxVal[8] |= (digitalRead(6) << 1);//pb
    //TxVal[8] |= (digitalRead(7) << 2);//slide
    //TxVal[8] |= (digitalRead(8) << 3);//toggle

    //Throttle TxVal[1]
    //Rotary pot TxVal[2]
    //Joy 1 X TxVal[3]
    //Joy 1 Y TxVal[4]
    //Joy 2 X TxVal[5]
    //Joy 2 Y TxVal[6]
    //rssi TxVal[7]
    //digital TxVal[8]
    //micros() TxVal[9]

    //Use the pot as the gain for all channels for now
    float GainPot = (float)(TxVal[2]) * 0.001f;

    //Get the target values from the TX
    int PitchTarg = (TxVal[3] / 10);
    int RollTarg = (TxVal[4] / 10);
    int YawTarg = (TxVal[6] / 10);

    //Prime the Target WOZ values
    if(PitchTargWOZ == 9999)
    PitchTargWOZ = PitchTarg;

    if(RollTargWOZ == 9999)
    RollTargWOZ = RollTarg;

    if(YawTargWOZ == 9999)
    YawTargWOZ = YawTarg;

    //Get the Centered target values
    float PitchTargCentred = (float)(PitchTarg – PitchTargWOZ);
    float RollTargCentred = (float)(RollTarg – RollTargWOZ);
    float YawTargCentred = (float)(YawTarg – YawTargWOZ);

    //Calculate gains
    float PitchGain = GainPot * 1.0f;
    float RollGain = GainPot * 1.0f;
    float YawGain = GainPot * 1.0f;

    //Get Gyro values
    float PitchGyro = (float)(AnIn[2] – AnInWOZ[2]);
    float RollGyro = (float)(AnIn[1] – AnInWOZ[1]);
    float YawGyro = (float)(AnIn[0] – AnInWOZ[0]);

    //Calc P error
    float PitchError = (float)PitchTargCentred + PitchGyro;
    float RollError = (float)RollTargCentred + RollGyro;
    float YawError = (float)YawTargCentred + YawGyro;

    //Apply gains
    int PitchTrim = (int)(PitchError * PitchGain);
    int RollTrim = (int)(RollError * RollGain);
    int YawTrim = (int)(YawError * YawGain);

    //Constaring trim authority
    PitchTrim = constrain(PitchTrim, -30, 30);
    RollTrim = constrain(RollTrim, -30, 30);
    YawTrim = constrain(YawTrim, -30, 30);

    //Dump the trim value
    if((TxVal[9] & 0x4) == 0)
    {
    PitchTrim = 0;
    RollTrim = 0;
    YawTrim = 0;
    }

    //Calc flap anglke
    int Flaps = 0;

    //Apply flaps
    if((TxVal[9] & 0x8) != 0)
    Flaps = 25;

    //Throttle
    val = TxVal[1] / 10;
    val = map(val, 1, 179, 30, 179);
    val = constrain(val, 1, 165); // scale it to use it with the servo (value between 0 and 180)
    servo[0].write(val); // sets the servo position according to the scaled value

    //Elevator Joy 1 Y TxVal[4]
    val = PitchTarg + PitchTrim;
    val = constrain(val, 15, 165);
    val = map(val, 0, 179, 135, 45); // scale it to use it with the servo (value between 0 and 180)
    servo[1].write(val); // sets the servo position according to the scaled value

    //Left Flaperon
    val = RollTarg + Flaps + RollTrim;
    val = constrain(val, 15, 165);
    val = map(val, 0, 179, 165, 15); // scale it to use it with the servo (value between 0 and 180)
    servo[2].write(val); // sets the servo position according to the scaled value

    //Right Flaperon
    val = RollTarg – Flaps + RollTrim;
    val = constrain(val, 15, 165);
    val = map(val, 0, 179, 165, 15); // scale it to use it with the servo (value between 0 and 180)
    servo[3].write(val); // sets the servo position according to the scaled value

    //Joy 2 x nose Wheel / rudder
    val = (TxVal[6] / 10);
    val = map(val, 0, 179, 55, 125);
    servo[4].write(val); // sets the servo position according to the scaled value

    //Joy 2 Y
    val = TxVal[5] / 10;
    val = constrain(val, 15, 165); // scale it to use it with the servo (value between 0 and 180)
    servo[5].write(val); // sets the servo position according to the scaled value

    }

    void NullServos()
    {

    //Throttle TxVal[1]
    //Rotary pot TxVal[2]
    //Joy 1 X TxVal[3]
    //Joy 1 Y TxVal[4]
    //Joy 2 X TxVal[5]
    //Joy 2 Y TxVal[6]
    //rssi TxVal[7]
    //digital TxVal[8]
    //micros() TxVal[9]

    //Throttle
    val = 0;
    val = map(val, 1, 179, 30, 179);
    val = constrain(val, 1, 179); // scale it to use it with the servo (value between 0 and 180)
    servo[0].write(val); // sets the servo position according to the scaled value

    //Elevator Joy 1 Y TxVal[4]
    val = 90;
    val = constrain(val, 1, 179); // scale it to use it with the servo (value between 0 and 180)
    servo[1].write(val); // sets the servo position according to the scaled value

    //Left Flaperon
    val = 90;
    val = map(val, 0, 179, 1, 179); // scale it to use it with the servo (value between 0 and 180)
    servo[2].write(val); // sets the servo position according to the scaled value

    //Right Flaperon
    val = 90;
    val = constrain(val, 1, 179); // scale it to use it with the servo (value between 0 and 180)
    servo[3].write(val); // sets the servo position according to the scaled value

    //Joy 2 X
    val = 90;
    val = constrain(val, 1, 179); // scale it to use it with the servo (value between 0 and 180)
    servo[4].write(val); // sets the servo position according to the scaled value

    //Joy 2 Y
    val = 90;
    val = constrain(val, 1, 179); // scale it to use it with the servo (value between 0 and 180)
    servo[5].write(val); // sets the servo position according to the scaled value

    }

    void SoftReset() // Restarts program from beginning but does not reset the peripherals and registers
    {

    //Prime the WOZ values
    PitchTargWOZ = 9999;
    RollTargWOZ = 9999;
    YawTargWOZ = 9999;

    NoPacketCount = 0;

    asm volatile (” jmp 0″);
    }
