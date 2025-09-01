#include <SPI.h>
#include <mcp_can.h>

/* Please modify SPI_CS_PIN to adapt to different baords.

   CANBed V1        - 17
   CANBed M0        - 3
   CAN Bus Shield   - 9
   CANBed 2040      - 9
   CANBed Dual      - 9
   OBD-2G Dev Kit   - 9
   Hud Dev Kit      - 9
*/

#define SPI_CS_PIN  9

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

#define PID_ENGIN_PRM       0x0C
#define PID_VEHICLE_SPEED   0x0D
#define PID_COOLANT_TEMP    0x05
#define PID_ACCEL_PEDAL_POS 0x49

#define CAN_ID_PID          0x7DF

#define BRAKE_PIN           A1

void set_mask_filt()
{
    // set mask, set both the mask to 0x3ff

    CAN.init_Mask(0, 0, 0x7FC);
    CAN.init_Mask(1, 0, 0x7FC);

    // set filter, we can receive id from 0x04 ~ 0x09

    CAN.init_Filt(0, 0, 0x7E8);                 
    CAN.init_Filt(1, 0, 0x7E8);

    CAN.init_Filt(2, 0, 0x7E8);
    CAN.init_Filt(3, 0, 0x7E8);
    CAN.init_Filt(4, 0, 0x7E8); 
    CAN.init_Filt(5, 0, 0x7E8);
}

void sendPid(unsigned char __pid) {
    unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);
}

bool getSpeed(int *s)
{
    sendPid(PID_VEHICLE_SPEED);
    unsigned long __timeout = millis();

    while (millis() - __timeout < 50) {
        unsigned char len = 0;
        unsigned char buf[8];
        unsigned long canId;

        if (CAN_MSGAVAIL == CAN.checkReceive()) {
            CAN.readMsgBuf(&canId, &len, buf);  // FIXED: use 3 arguments

            if (buf[1] == 0x41) {
                *s = buf[3];
                return true;
            }
        }
    }

    return false;
}

bool getRPM(int *rpm)
{
    sendPid(PID_ENGIN_PRM);
    unsigned long __timeout = millis();

    while (millis() - __timeout < 50)
    {
        unsigned char len = 0;
        unsigned char buf[8];
        unsigned long canId;

        if (CAN_MSGAVAIL == CAN.checkReceive())
        {
            CAN.readMsgBuf(&canId, &len, buf);   // ★ 3-인자 버전 사용

            if (buf[1] == 0x41 && buf[2] == PID_ENGIN_PRM)
            {
                int A = buf[3];
                int B = buf[4];
                *rpm = ((A * 256) + B) / 4;
                return true;
            }
        }
    }
    return false;
}

bool getAccelPedalPos(float *pos)
{
    sendPid(PID_ACCEL_PEDAL_POS);
    unsigned long __timeout = millis();

    while (millis() - __timeout < 50)
    {
        unsigned char len = 0;
        unsigned char buf[8];
        unsigned long canId;

        if (CAN_MSGAVAIL == CAN.checkReceive())
        {
            CAN.readMsgBuf(&canId, &len, buf);   // ★ 3-인자 버전 사용

            if (buf[1] == 0x41 && buf[2] == PID_ACCEL_PEDAL_POS)
            {
                int A = buf[3];
                *pos = (A * 100.0) / 255.0;
                return true;
            }
        }
    }
    return false;
}

const int pinPwrCtrl = 12;          // for RP2040 verison
//const int pinPwrCtrl = A3;        // for Atmaega32U4 version


void setup() {
    Serial.begin(115200);
    while (!Serial);

    pinMode(pinPwrCtrl, OUTPUT);
    digitalWrite(pinPwrCtrl, HIGH);

    while (CAN_OK != CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ)) {  // FIXED
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    CAN.setMode(MCP_NORMAL);
    Serial.println("CAN init ok!");
    set_mask_filt();

    pinMode(BRAKE_PIN, INPUT);
}

void loop() {
    // Serial.println("start loop function");

    int __speed = 0;
    int __rpm = 0;
    float __pedal = 0.0;

    // if(getSpeed(&__speed))
    // {
    //     Serial.print("Vehicle Speed: ");
    //     Serial.print(__speed);
    //     Serial.println(" km/h");
    // }

    // if(getRPM(&__rpm))
    // {
    //     Serial.print("Engine RPM: ");
    //     Serial.println(__rpm);
    // }

    // if(getAccelPedalPos(&__pedal))
    // {
    //     Serial.print("Accelerator Pedal Position: ");
    //     Serial.print(__pedal, 1);
    //     Serial.println(" %");
    // }

    // int isBrake = digitalRead(BRAKE_PIN);
    // Serial.print("brake pin : ");
    // Serial.println(isBrake);
    // delay(50);
    getSpeed(&__speed);
    getRPM(&__rpm);
    getAccelPedalPos(&__pedal);
    int isBrake = digitalRead(BRAKE_PIN);

    Serial.print("/S");
    Serial.print(__speed);
    Serial.print("/R");
    Serial.print(__rpm);
    Serial.print("/P");
    Serial.print(__pedal);
    Serial.print("/B");
    Serial.println(isBrake);
    // delay(50);
}

// END FILE