/*
 * File         userinput.h
 * Author       Heiko Klausing (h dot klausing at gmx dot de)
 * Created      2020-08-14
 * Note
 */
#pragma once

#include <Arduino.h>



enum ASCII {
    CHAR_NUL = 0x00,
    CHAR_TAB = 0x09,
    CHAR_ESC = 0x1b,
    CHAR_RET = 0x0d,
    CHAR_SP  = 0x20,
    CHAR_DEL = 0x7f,
};


class UserInput
{
public:
    // Request values
    enum class InputState {
        NO_INPUT,               // no input loop running
        INPUT_WAIT,             // User input is ongoing
        INPUT_READY,            // User string is available
        INPUT_CHAR,             // An input character is available
        REQUEST_UNKNOWN         // request for unknown page
    };
private:
    /* data */
    static const uint8_t MAX_INPUT_SIZE = 32;
    char m_input_buffer[MAX_INPUT_SIZE];
    uint8_t m_index;
    bool m_string_available;
    bool m_keymode;
    char m_last_read_char;

public:
    UserInput(bool keymode);
    UserInput(const UserInput&) = delete;
    UserInput& operator=(const UserInput&) = delete;
    ~UserInput();


    InputState readInput(void);

    InputState status(void);

    char* getInput(void);

    char getChar(void);

    void clear(void);

    bool getKeyMode(void);

    void setKeyMode(bool state);

};

