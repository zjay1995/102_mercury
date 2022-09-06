#pragma once

#include <Adafruit_ADS1015.h>
#include "Adafruit_SHT4x.h"

class AnalogSourceInput
{
protected:

	int 			m_refreshRate = 4; //refreshes per second
	unsigned long 	m_lastReadValueTick = -5000000;
    uint16_t 		m_lastReadValue;
    uint16_t 		m_lasttemp;
    uint16_t 		m_lasthum;
    uint16_t 		m_lastReadValue_battery;

public:
    virtual uint16_t getTemp() = 0;
    virtual uint16_t getHum() = 0;
    virtual uint16_t getMiliVolts() = 0;
    virtual uint16_t getMiliVolts_battery() = 0;

};

class ADS1115AnalogSourceInput : public AnalogSourceInput
{


	Adafruit_ADS1115* m_ads1115;
    Adafruit_SHT4x* m_sht4;

	
public:

	ADS1115AnalogSourceInput(Adafruit_ADS1115* ads1115, Adafruit_SHT4x* sht4X) : m_ads1115(ads1115),m_sht4(sht4X)
	{
		
	}
	~ADS1115AnalogSourceInput()=default;
	
	
	uint16_t getMiliVolts()
	{
		unsigned long now = millis();
		
		if(now - m_lastReadValueTick > 1000 / m_refreshRate)
		{	
			m_lastReadValueTick = now;
			
			const float multiplier = 0.125F; //GAIN 1
		
			m_lastReadValue = m_ads1115->readADC_SingleEnded(0) * multiplier;// / 1000.0;
		}

		return m_lastReadValue;
	}

    uint16_t getMiliVolts_battery()
    {

        const float multiplier = 0.125F; //GAIN 1

        m_lastReadValue_battery = m_ads1115->readADC_SingleEnded(1) * multiplier;// / 1000.0;

        return m_lastReadValue_battery;
    }

    uint16_t getTemp()
    {
        sensors_event_t humidity, temp;
        m_sht4->getEvent(&humidity, &temp);// / 1000.0;
        m_lasttemp = temp.temperature;
        return m_lasttemp;
    }

    uint16_t getHum()
    {
        sensors_event_t humidity, temp;
        m_sht4->getEvent(&humidity, &temp);// / 1000.0;
        m_lasthum = humidity.relative_humidity;
        return m_lasthum;
    }

};