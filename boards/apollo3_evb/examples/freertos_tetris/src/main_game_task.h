//*****************************************************************************
//
//! @file led_task.h
//!
//! @brief Functions and variables related to the LED task.
//!
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2019, Cullen Logan
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision v2.2.0-7-g63f7c2ba1 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef MAIN_GAME_TASK_H
#define MAIN_GAME_TASK_H

//*****************************************************************************
//
// LED task handle.
//
//*****************************************************************************
extern TaskHandle_t game_task_handle;

typedef enum _eUIMsgType_ {
    UI_MSG_TYPE_HEARTBEAT,
    UI_MSG_TYPE_BUTTON_PRESS,
    UI_MSG_TYPE_VOICE_COMMAND
} eUIMsgType_t;

typedef struct _UIMessage_t_ {
    eUIMsgType_t msgType;

    union _uinputType_{
        uint32_t butttonPressed;
        uint32_t voiceCommand;
    } uInpuType;

    am_hal_rtc_time_t timeStamp;
} UIMessage_t;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern void GameTaskSetup(void);
extern void GameTask(void *pvParameters);

//*****************************************************************************
//
// External Variables.
//
//*****************************************************************************
extern QueueHandle_t gameUIQueue;


#endif // RADIO_TASK_H
