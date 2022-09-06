#pragma once

#include <vector>

#include <Arduino.h>

#include "ConfigurationManager.h"
#include "Globals.h"
#include <EEPROM.h>
#include "BluetoothSerial.h"


class Gas
{
    String m_name;
    double m_slope;
    double m_intercept;
    double m_secondp;
    double m_thermalConductivity;

public:

    Gas(String name, double thermalConductivity = 1.0) : m_name(name),
                                                         m_thermalConductivity(thermalConductivity)
    {

    }

    String getName() { return m_name; }

    double getThermalConductivity() const 					{ return m_thermalConductivity; }
    void setThermalConductivity(double thermalConductivity) { m_thermalConductivity = thermalConductivity; }

};



class GasManager : public ParamChangeListener
{
    std::vector<Gas> m_gases;

    int m_selectedGas = 0;

    double m_slope;
    double m_intercept;
    double m_secondp;
    double m_zero;
    double m_calgas;

    ConfigurationManager* m_configurationManager;

public:
    GasManager(double slope, double intercept, double secondp, double zero, double calgas) : m_slope(slope),
                                                                 m_intercept(intercept),
                                                                 m_secondp(secondp),
                                                                 m_zero(zero),
                                                                 m_calgas(calgas)
    {}

    ~GasManager() {}

    double getSlope() const { return m_slope; }
    double getIntercept() const { return m_intercept; }
    double getSecondp() const { return m_secondp; }
    double getZero() const {return m_zero; }
    double getCalgas() const {return m_calgas; }
    void setSlope(double s) { m_slope = s; }
    void setIntercept(double i) { m_intercept = i; }
    void setSecondp(double p) {m_secondp = p; }
    void setZero(double z) {m_zero = z; }
    void setCalgas(double c) {m_calgas = c;}

    double calculateSLM(double voltage) {
        //double val = (voltage*voltage*m_slope + voltage*m_secondp + m_intercept ) * getSelectedGas().getThermalConductivity();
        double val = (voltage*m_secondp + m_intercept ) / getSelectedGas().getThermalConductivity();
        //SerialBT.print((String(val, 0) + ",ppb\n").c_str());
        //Serial.println(String(m_secondp));
        //Serial.println("calculateSLM " + String(val) + " " + String(voltage) + " " + String(getSelectedGas().getThermalConductivity()) + " " + String(m_slope) + " " + String(m_intercept));
        return val < 0 ? 0 : val;
    }

    void setConfigurationManager(ConfigurationManager* c) { m_configurationManager = c; }

    void addGas(Gas g) { m_gases.push_back(g); }

    void selectNextGas()
    {
        m_selectedGas = (m_selectedGas + 1) % m_gases.size();
        Serial.println("selectNextGas");

    }

    void selectGasByIndex(int index)
    {
        Serial.println("selectGasByIndex");
        if(index >= 0 && index < m_gases.size())
        {
            m_selectedGas = index;

        }
    }

    void calibrate(float zero){
        m_zero = zero;
        m_intercept = -zero;
        EEPROM.writeDouble(12, m_intercept);
        Serial.println(m_zero);
        Serial.println(m_calgas);
        Serial.println(m_secondp);
    }

    void calibrate2(float cal){
        m_calgas = cal;
        m_secondp = 1/(m_calgas - m_zero);
        EEPROM.writeDouble(20, m_secondp);

        Serial.println(m_zero);
        Serial.println(m_calgas);
        Serial.println(m_secondp);
    }

    void calibrate3(){
        m_secondp = (m_calgas - m_zero);

        Serial.println("New Cal Factors: ");
        Serial.println(m_secondp);
        Serial.println("Calibrated !!!");
    }

    void selectPreviousGas()
    {
        Serial.println("selectPreviousGas");

        if(m_selectedGas == 0)
            m_selectedGas = m_gases.size() - 1;
        else
            m_selectedGas = m_selectedGas - 1;
    }

    Gas& getSelectedGas() { return m_gases[m_selectedGas]; }


    std::vector<Gas>& getAllGases() { return m_gases; }

    void onParamChange(String param, String value)
    {

        Serial.println("onGasParamChange: " + param + "=" + value);

        if(param.equals(c_SLOPE_PARAM_NAME))
        {
            m_slope = value.toDouble();
            Serial.println("Setting SLOPE: " + value);
        }
        else if(param.equals(c_INTERCEPT_PARAM_NAME))
        {
            m_intercept = value.toDouble();
            Serial.println("Setting INTERCEPT: " + value);
        }
        else if(param.equals(c_SECONDP_PARAM_NAME))
        {
            m_secondp = value.toDouble();
            Serial.println("Setting SECONDP: " + value);
        }
        else
        {
            for (auto& gas : m_gases)
            {
                if (gas.getName().equals(param))
                {
                    double tc = value.toDouble();
                    gas.setThermalConductivity(tc);
                    Serial.println("Setting TC for gas: " + gas.getName() + " " + String(tc));
                    break;
                }
            }
        }
    }


};

