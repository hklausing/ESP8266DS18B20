/*
 * File         userinput.cpp
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-14
 * Note
 */

#include "userinput.h"


UserInput::UserInput(bool keymode)
    : m_index{0}
    , m_string_available{false}
    , m_keymode{keymode}
    , m_last_read_char{CHAR_NUL}
{
    static_assert(MAX_INPUT_SIZE > 1, "Value must be greater than 1!");
}

UserInput::~UserInput()
{
}


UserInput::InputState UserInput::readInput(void)
{
    // read character if available
    if(Serial.available()) {
        // continue if all outgoing data transmitted.
        Serial.flush();

        char m_last_read_char = Serial.read();

        if(!m_keymode)
        {

            // handle strings
            m_string_available = false;
            switch(m_last_read_char)
            {
                case CHAR_ESC:  // Escape clears all
                    clear();
                    break;
                case CHAR_RET:
                    m_input_buffer[m_index] = 0;
                    m_index = 0;
                    m_string_available = true;
                    break;
                default:
                    if(m_index < MAX_INPUT_SIZE - 1)
                    {
                        m_input_buffer[m_index++] = m_last_read_char;
                    }
                    else
                    {
                        Serial.println("\nERROR: input buffer overflow, inputs discarded!");
                        while(Serial.available())  // remove all characters from input buffer
                        {
                            Serial.read();
                        }
                        clear();
                    }
            }
        }
    }
    return status();
}


UserInput::InputState UserInput::status(void)
{
    if(m_keymode)
    {

    }
    else
    {

        if(m_index == 0) {
            if(m_string_available) {
                return InputState::INPUT_READY;
            } else {
                return InputState::NO_INPUT;
            }
        }
    }
    return InputState::INPUT_WAIT;

}


char* UserInput::getInput(void)
{
    if(m_index == 0 && m_string_available) {
        return m_input_buffer;
    }
    return NULL;
}


char UserInput::getChar(void)
{
    clear();
    return m_last_read_char;
}

void UserInput::clear(void)
{
    m_index = 0;
    m_string_available = false;
}


bool UserInput::getKeyMode()
{
    return m_keymode;
}


void UserInput::setKeyMode(bool state)
{
    m_keymode = state;
}
