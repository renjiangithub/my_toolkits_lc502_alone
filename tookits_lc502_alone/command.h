#ifndef COMMAND_H
#define COMMAND_H


#include <vector>
#include <memory>
#include <map>
namespace sansi{
    namespace protocol{

        enum Command {
            ASK_AUTHORIZATION = 1,
            GET_SERIAL_PORT_PARAMS = 2,
            SET_SERIAL_PORT_PARAMS = 3,
            GET_AR502_SIDE_PARAMS  = 4,
            SET_AR502_SIDE_PARAMS  = 5,
            GET_X9_SIDE_PARAMS  = 6,
            SET_X9_SIDE_PARAMS  = 7,
            GET_COMMON_REGISTER_DATA  = 8,
            ASYNC_LC502_TIME  = 9,
            RESET_LC502_TIME  = 10,
            RESTORE_LC502_FACTORY_SETTING  = 11,
            CLEAR_LC502_DATA = 12,
            GET_CHANNEL_DATA = 13,
            SET_CHANNEL_1_LIGHTING = 14,
            GET_CHANNEL_1_FEEDBACK = 15,
            SET_CHANNEL_2_LIGHTING = 16,
            GET_CHANNEL_2_FEEDBACK = 17,
            GET_LC502_REGISTER = 18,
            SET_DO_1_SWITCH = 19,
            GET_DO_1_FEEDBACK = 20,
            SET_DO_2_SWITCH = 21,
            GET_DO_2_FEEDBACK = 22
        };


    }
}
#endif // COMMAND_H
