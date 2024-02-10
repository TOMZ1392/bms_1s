#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 6, 5, 4, 3);
#define BATT_V_ADC_PIN A0
#define AVG_SAMPLES 10
#define SAMPLE_POLL_DLY 50
#define LIPO_1S 1
#define LIPO_2S 2
#define LIPO_3S 3
#define LIPO_4S 4
#define VOLTMETER_MODE 5
#define BATT_ID_ERR 255

float adcAvg = 0.0;
float battInitVolt = 0.0;
uint8_t inputBatt = 0;
#define SET_INPUT_BATT(x) (inputBatt = x)
#define GET_INPUT_BATT() (inputBatt)
#define OFFSET_VOLT  1.1f
const char* battTypeNames[6] = {"LIPO_1S", "LIPO_2S", "LIPO_3S", "LIPO_4S", "VOLT_SRC", "UNKNOWN"};
/*
 * Funcnction identifies input battery. 
 * Blocks for 500ms
 * Sets global batt initial voltage: float  battInitVolt
 * returs LIPO_1S,LIPO_2S,LIPO_3S,LIPO_4S => no of cells in series and BATT_ID_ERR if otherwise
 */
uint8_t bms_IdInputBatt()
{
    uint32_t battInAdc = 0, ctr = 0;
    uint8_t battIn;
    while (ctr < AVG_SAMPLES)
    {
        battInAdc += analogRead(BATT_V_ADC_PIN);
        ctr++;
        delay(SAMPLE_POLL_DLY);
    }
    adcAvg = (float)battInAdc / AVG_SAMPLES; //avg
    battInitVolt = (adcAvg * 0.0293);         //0.293=30/1024
    if (battInitVolt > 3.3 && battInitVolt < 4.3)
    {
        return LIPO_1S;
    }
    else if (battInitVolt > 6.7 && battInitVolt < 8.5)
    {
        return LIPO_2S;
    }
    else if (battInitVolt > 9.9 && battInitVolt < 12.7)
    {
        return LIPO_3S;
    }
    else if (battInitVolt > 13.6 && battInitVolt < 16.9)
    {
        return LIPO_4S;
    }
    else if (battInitVolt >= 0.0 && battInitVolt < 31.0)
    {
        return VOLTMETER_MODE;
    }
    else
    {
        return BATT_ID_ERR;
    }
}

void setup()
{

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Batt id:");
    lcd.setCursor(9, 0);
    SET_INPUT_BATT(bms_IdInputBatt());
   // lcd.print(battTypeNames[(GET_INPUT_BATT()) < 255 ? GET_INPUT_BATT() - 1 : 5]);
    lcd.setCursor(3, 1);
    lcd.print("%");
    lcd.setCursor(9, 1);
    lcd.print("V");
}

void initHeaders()
{
    lcd.print("Batt id:");
    lcd.setCursor(9, 0);
    uint8_t nameIdx=GET_INPUT_BATT() < 255 ? GET_INPUT_BATT() - 1 : 5;
    lcd.write(battTypeNames[nameIdx]);
    lcd.setCursor(3, 1);
    lcd.print("%");
    lcd.setCursor(11, 1);
    lcd.print("V");
    delay(500);
}
/*
 * Function monitors battery voltage and updates voltage and percentage parameters for detected batt
 */
boolean bms_monitorBatt(float *volt, uint8_t *perc_charge)
{
    uint16_t adcVal = analogRead(BATT_V_ADC_PIN);
    *volt = (adcVal * 0.0293)+ OFFSET_VOLT; //0.293=30/1024

    switch (GET_INPUT_BATT())
    {
    case LIPO_1S:

        if (*volt < 3.4)
        {
            *perc_charge = 0;
        }
        else
        {
            *perc_charge = (uint8_t)((( 1- (4.3 - (*volt))) * 100));
        }
        break;
    case LIPO_2S:
        if (*volt < 6.6)
        {
            *perc_charge = 0;
        }
        else
        {
            *perc_charge = (uint8_t)((1-(8.5-(*volt)))*50);
        }
        break;
    case LIPO_3S:
        if (*volt < 10.3)
        {
            *perc_charge = 0;
        }
        else
        {
            *perc_charge = (uint8_t)(((1-(12.7 - (*volt))) * 40));
        }
        break;
    case LIPO_4S:
        if (*volt < 13.8)
        {
            *perc_charge = 0;
        }
        else
        {
            *perc_charge = (uint8_t)(((1-(16.9 - (*volt))) * 33));
        }
        break;
    case VOLTMETER_MODE:

        *perc_charge = (uint8_t)(((1-(30 - (*volt))) * 3.33));

        break;
    case BATT_ID_ERR:
        *perc_charge = BATT_ID_ERR;
        break;
    default:
        break;
    }
    if(*perc_charge<10 || *perc_charge == BATT_ID_ERR){
     return false; 
     }
     else{
      return true;
      }
}
float battvolt = 0.0;
uint8_t percentageCharg = 0;
/*
 * Function displays status: input batt type,batt volt and charge%
 * blocks for 1second
 */
const uint16_t dispRefreshDly=1000;//delay in ms
void bms_dispStatus()
{
    bms_monitorBatt(&battvolt, &percentageCharg);
    lcd.clear();
    initHeaders();
    lcd.setCursor(0, 1);
    lcd.print(percentageCharg);
    lcd.setCursor(5, 1);
    lcd.print(battvolt);
    delay(dispRefreshDly);
}

void loop()
{
 void bms_dispStatus() 
}
