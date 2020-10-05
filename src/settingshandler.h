/*
 * File         settingshandler.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-09-06
 *
 */

#pragma once

#include <Arduino.h>

enum class InputStatus
{
    Active,
    Inactive,
    Write
};

enum class InputType
{
    IT_FLOAT,
    IT_INTEGER,
    IT_STRING
};

/**
 * @brief Handles the communication via the Serial interface
 * 
 * The communitaction is simple:
 * - Press 's'
 * - Press '?'
 * - Press 'l'
 * - Press '0'
 * - Insert new data
 * - Press 'w'
 * - ...
 * - Press 'Q'
 * - Press '#' finish input
 * 
 * @tparam STR_SIZE 
 */
template <int STR_SIZE>
class SettingsHandler
{
private:
    static const int MAX_INPUT_DATA = 10;

public:
    /*
     *
     */
    SettingsHandler() : m_active{false}, m_edit_active{false}, m_list_count{0}, m_index{-1} {}

    /*
     *
     */
    ~SettingsHandler() {}

    /*
     * add an new data element to the buffer, type float
    */
    void addSettingItem(const String &name, InputType type, float *data_ptr)
    {
        if (!isListReady())
            return;
        //Serial.println(name);
        String data(*data_ptr);
        m_list[m_list_count] = {name, sizeof(float), type};
        m_list[m_list_count].ptr_float = data_ptr;
        m_list_count++;
    }

    /*
     * add an new data element to the buffer, type integer
     */
    void addSettingItem(const String &name, InputType type, int *data_ptr)
    {
        if (!isListReady())
            return;
        //Serial.println(name);
        String data(*data_ptr);
        m_list[m_list_count] = {name, sizeof(int), type};
        m_list[m_list_count].ptr_int = data_ptr;
        m_list_count++;
    }

    /*
     * add an new data element to the buffer, type char string
     */
    void addSettingItem(const String &name, InputType type, char *data_ptr)
    {
        if (!isListReady())
            return;
        //Serial.println(name);
        String data = data_ptr;
        m_list[m_list_count] = {name, STR_SIZE, type};
        m_list[m_list_count].ptr_char = data_ptr;
        m_list_count++;
    }


    /**
     * @brief Main entry to check if any user action is started or ongoing.
     * 
     * @return InputStatus status information about the user activity:
     *         + Active - user is inputting data, action is not finished;
     *         + Inactive - no input is ongoing;
     *         + Write - User triggers writing the setting data
     */
    InputStatus activityStatus(void)
    {
        InputStatus rc = InputStatus::Inactive;

        if (Serial.available() > 0)
        {
            // get input if available, status is check via m_active and m_index
            char c = Serial.read();

            if (m_active && m_index == -1)
                rc = handleActive(c);
            else if (m_active)
                rc = handleEdit(c);
            else
                rc = handleInactiveState(c);
        }

        return rc;
    }

    /*
     * Clean the stored data
     */
    void clean(void)
    {
        m_active = false;
        m_edit_active = false;
        m_list_count = 0;
        m_index = -1;
    }

private:
    /*
    * Returns the stored settings value as a string
    */
    String getDataValue(int index)
    {
        if (index < 0 || index >= m_list_count)
        {
            String s("???");
            return s;
        }

        char buf[STR_SIZE];
        memset(buf, 0, STR_SIZE);
        switch (m_list[index].type)
        {
        case InputType::IT_FLOAT:
            sprintf(buf, "%f", *m_list[index].ptr_float);
            break;
        case InputType::IT_INTEGER:
            sprintf(buf, "%i", *m_list[index].ptr_int);
            break;
        case InputType::IT_STRING:
            strcpy(buf, m_list[index].ptr_char);
            break;
        default:
            buf[0] = '?';
        }

        String s(buf);
        return s;
    }

    /*
     * Test if list can get mote data
     */
    bool isListReady()
    {
        if (m_list_count == MAX_INPUT_DATA)
        {
            Serial.println(F("Error: to much data elements!"));
            return false;
        }
        return true;
    }

    /*
    * Information for user
    */
    void infoUser(void)
    {
        Serial.println("Press a command key ('?' for help)");
    }

    /*
    * Display command names with there key codes
    */
    void cmd_help(void)
    {
        Serial.println(
            "Help for settings:\n"
            "Command    Description\n"
            "?          Print this help screen\n"
            "Q          Stop setting menu\n"
            "l          List settings\n"
            "w          Write settings to EEPROM");
        for (int i = 0; i < m_list_count; i++)
        {
            Serial.printf("%i          %s\n", i, m_list[i].name.c_str());
        }
    }

    /*
    * Display setting names and values with index numbers
    */
    void cmd_list(void)
    {
        Serial.println(F("List settings:"));

        for (int i = 0; i < m_list_count; i++)
        {
            Serial.printf("%i) %-20s - %s\n", i, m_list[i].name.c_str(), getDataValue(i).c_str());
        }
    }

    /*
    * Show the current data value, and prepare the input information for user
    */
    void cmdEditSetting()
    {
        Serial.printf("%-20s - %s  (ESC to abort input)\n", m_list[m_index].name.c_str(), getDataValue(m_index).c_str());
        Serial.print("new value: ");
    }

    /*
    * Insert next input charcater to the temporary buffer
    */
    void insertInputCharcater(const int index, const char c)
    {
        bool input_filter_passed = false;
        switch (m_list[index].type)
        {
        case InputType::IT_FLOAT:
            if ((c == '-' && m_buf.isEmpty()) || c == '.' || (c >= '0' && c <= '9'))
                input_filter_passed = true;
            break;
        case InputType::IT_INTEGER:
            if (c == '-' || (c >= '0' && c <= '9'))
                input_filter_passed = true;
            break;
        case InputType::IT_STRING:
            if (c >= ' ' && c <= 255)
                input_filter_passed = true;
            break;
        default:
            Serial.println("ups whats that!");
        }

        if (input_filter_passed)
        {
            m_edit_active = true;
            m_buf += c;
            Serial.print(c);
        }
    }

    /*
    * Finalize the input setting
    */
    void finializeInput(const int index)
    {
        // add data
        switch (m_list[m_index].type)
        {
        case InputType::IT_FLOAT:
            *m_list[m_index].ptr_float = m_buf.toFloat();
            break;
        case InputType::IT_INTEGER:
            *m_list[m_index].ptr_int = m_buf.toInt();
            break;
        case InputType::IT_STRING:
            memset(m_list[m_index].ptr_int, 0, STR_SIZE);
            memcpy(m_list[m_index].ptr_int, m_buf.c_str(), m_buf.length());
            break;
        }
        m_edit_active = false;
        m_index = -1;
    }

    /*
    * Main input loop; controls edit start, prepare switch to data edit section
    */
    InputStatus handleActive(char c)
    {
        InputStatus rc = m_active ? InputStatus::Active : InputStatus::Inactive;

        if (!m_edit_active)
            switch (c)
            {
            case 'Q':
                // Quit edit mode
                m_active = false;
                m_edit_active = false;
                Serial.println("Setting inactive");
                break;
            case '?':
                // Show help list
                cmd_help();
                infoUser();
                break;
            case 'l':
                // Show data list
                cmd_list();
                infoUser();
                break;
            case 'w':
                // Write setting to EEPROM
                rc = InputStatus::Write;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                // select one of the 10 input values
                m_index = c - '0';
                if (m_index < 0 || m_index >= (int)m_list_count)
                {
                    Serial.println("\nNo supported data line.");
                    m_index = -1;
                    break;
                }
                m_edit_active = true;
                m_buf = "";
                cmdEditSetting();
                break;
            }

        return rc;
    }

    /*
    * Controls data input
    */
    InputStatus handleEdit(char c)
    {
        InputStatus rc = m_active ? InputStatus::Active : InputStatus::Inactive;

        /* Edit is active */
        switch (c)
        {
        case '\n':
            // Enter - finialze input
            finializeInput(m_index);
            Serial.println("\nInput was stored.");
            break;
        case 0x1b:
            // ESC - stop editing
            m_edit_active = false;
            m_index = -1;
            Serial.println("\nInput was interrupted.");
            break;
        default:
            // put the input to buffer
            insertInputCharcater(m_index, c);
        }

        return rc;
    }

    /**
    * @brief 
    * 
    * @param c 
    * @return InputStatus 
    */
    InputStatus handleInactiveState(char c)
    {
        if (c == 's')
        {
            m_active = true;
            Serial.println(F("Setting active"));
            infoUser();
        }
        return m_active ? InputStatus::Active : InputStatus::Inactive;
    }

private:
    /* data */
    bool m_active;
    bool m_edit_active;

    /**
     * @brief Parameter element structure to support some basic data types
     * 
     * The supported basic types are:
     * - float
     * - integer
     * - char array with user defind size that include termination character
     */
    typedef struct list_e
    {
        String name;
        size_t size;
        InputType type;
        union
        {
            float *ptr_float;
            int *ptr_int;
            char *ptr_char;
        };
    } list_t;
    list_t m_list[MAX_INPUT_DATA];
    uint8_t m_list_count;

    // if -1 no data is selected
    int m_index;
    String m_buf;
};



extern SettingsHandler<32> g_ih;
