//*****************************************************************************
//
//! @file main_game_task.c
//!
//! @brief Task to handle LED operation.
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

//*****************************************************************************
//
// Global includes for this project.
//
//*****************************************************************************
#include "types.h"
#include "error.h"
#include "freertos_tetris.h"
#include "task.h"
#include "tetris.h"
//*****************************************************************************
//
// Game task handle.
//
//*****************************************************************************
TaskHandle_t game_task_handle;
//*****************************************************************************
//
// Handle for LED-related events.
//
//*****************************************************************************
EventGroupHandle_t xLedEventHandle;

/* The number of items the queue can hold.  This is 5 as the receive task
will remove items as they are added but may need to process multiple button
presses that occur rapidly*/
#define gameQUEUE_LENGTH                    ( 5 )

QueueHandle_t gameUIQueue = NULL;
TimerHandle_t heartBeatTimerHandle = NULL;

void HeartBeatTimerCallback(TimerHandle_t xTimer);

extern gfx_Bitmap_t buddha;
extern gfx_Bitmap_t buddha_sideways;
extern gfx_Bitmap_t longI;
//*****************************************************************************
//
// Perform initial setup for the Game task.
//
//*****************************************************************************
void GameTaskSetup(void)
{
    int32_t retVal = 0;
    am_util_debug_printf("GameTask: setup\r\n");

    /*
     * Configure IOM1 GPIO[9:8] for I2C Mode with 12mA drive strength and 1.5k pull-ups
     */
    retVal = ssd1306_platform_init(1);

    if ( AM_HAL_STATUS_SUCCESS != retVal )
    {
        am_util_debug_printf("ERROR!!! GameTask: setup failed - ssd1306_platform_init()\r\n");
        return;
    }

    ssd1306_show_frame(&Ambiq_Micro_Logo);

    /* Create the queue. */
    gameUIQueue = xQueueCreate(gameQUEUE_LENGTH, sizeof(UIMessage_t));

    if( NULL != gameUIQueue )
    {
        /* Start the two tasks as described in the comments at the top of this
        file. */
        xTaskCreate( GameTask,                  /* The function that implements the task. */
                    "GameTask",                 /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                    configMINIMAL_STACK_SIZE,   /* The size of the stack to allocate to the task. */
                    ( void * ) gameUIQueue,     /* The parameter passed to the task - just to check the functionality. */
                    4,                             /* The priority assigned to the task. */
                    &game_task_handle );        /* The task handle is not required, so NULL is passed. */
        //
        heartBeatTimerHandle = xTimerCreate("HeartBeat",
                                            pdMS_TO_TICKS(1000),
                                            pdTRUE,
                                            0,
                                            HeartBeatTimerCallback);

    }
    else
    {
        am_util_debug_printf("GameTask: Error creating gameUIQueue\r\n");
    }
}

//*****************************************************************************
//
// Task thread implementing the game's UI, graphics sub-system init, and timer
// based forward progression in game.
//
//*****************************************************************************
void GameTask(void *pvParameters)
{
    BaseType_t bDone = pdFALSE;
    UIMessage_t uiEvent;
    BaseType_t ret;
    uint32_t u32Button;
    static bool inverted = false;
    uint32_t retVal = ERROR_GENERIC;
    int xPos = 8;
    int yPos = 18;

    gfx_DeviceContext_t* pDC = NULL;
    //setup the graphics engine
    gfx_Bitmap_t *ptr = ssd1306_get_frame_buffer();
    retVal = gfx_2d_DeviceContextInit(ptr, NULL, 20, 20);

    if(SUCCESS == retVal)
    {
        pDC = gfx_2d_DeviceContextGet();
        ret = xTimerStart(heartBeatTimerHandle, pdMS_TO_TICKS(100));
    }

    /*
    am_hal_rtc_time_t rcvTime, deltaTime;
    */
    while( (retVal == SUCCESS) && (pdFALSE == bDone) )
    {
        ret = xQueueReceive(gameUIQueue, &uiEvent, pdMS_TO_TICKS(5000));    //40 ms should allow for 25 fps
    /*    //first thing to do is get he current RTC time so hat we can compare it to the time stamp
        ret = am_hal_rtc_time_get(&rcvTime);
        if ( AM_HAL_STATUS_SUCCESS == ret ){
            deltaTime.ui32Year = rcvTime.ui32Year - uiEvent.timeStamp.ui32Year;
            deltaTime.ui32Hour = rcvTime.ui32Hour - uiEvent.timeStamp.ui32Hour;
            deltaTime.ui32Minute = rcvTime.ui32Minute - uiEvent.timeStamp.ui32Minute;
            deltaTime.ui32Second = rcvTime.ui32Second - uiEvent.timeStamp.ui32Second;
            deltaTime.ui32Hundredths = rcvTime.ui32Hundredths - uiEvent.timeStamp.ui32Hundredths;
            am_util_debug_printf("GameTask: Message Receive Delta [%u:%u:%u:%u:%u]\r\n",
                                    deltaTime.ui32Year,
                                    deltaTime.ui32Hour,
                                    deltaTime.ui32Minute,
                                    deltaTime.ui32Second,
                                    deltaTime.ui32Hundredths);
        }
     */
        if( pdPASS == ret )
        {
            static bool led0 = false;
            static bool led1 = false;
            static bool led2 = false;
            static bool clearOnce = false;
            //in this case we must process the UI input
            //or in the case of a time expiration we can move a piece down
            switch(uiEvent.msgType) {
            case UI_MSG_TYPE_HEARTBEAT:
                //am_util_debug_printf("GameTask: Heartbeat Received %u\r\n");
                inverted = !inverted;

                if(!clearOnce){

                    ssd1306_clear_screen(true);
                    clearOnce = !clearOnce;

                    gfx_RectNode_t *activeClipRegionNode = gfx_clip_Get();
                    activeClipRegionNode->pNext = NULL;
                    activeClipRegionNode->Rect = gfx_rect_Make(0,0,ptr->uWidth - 1, ptr->uHeight - 1);
                    pDC->pActiveClipRegion = activeClipRegionNode;

                    draw_tetris_well_10_x_20(pDC);
                    gfx_bmp_DrawBitmapByAddr(pDC, &longI, xPos,yPos);
                    pDC->bDirty = TRUE;
                    ssd1306_show_frame(pDC->pBitmap);
                    //if(pDC->pActiveClipRegion){
                    //    gfx_clip_ReleaseClipChain(pDC->pActiveClipRegion);
                    //}

                }else{

                    gfx_Rect_t rect1 = {xPos,yPos,longI.uWidth,longI.uHeight};//gfx_rect_Make(0,0,25, 64);
                    gfx_2d_FillRect(pDC,&rect1, pDC->pPalette[PALETTE_BG_COLOR_IDX]);

                    if((xPos + longI.uWidth + 6) < ptr->uWidth){
                        xPos += 6;
                    }

                    gfx_bmp_DrawBitmapByAddr(pDC, &longI, xPos,yPos);
                    pDC->bDirty = TRUE;
                    ssd1306_show_frame(pDC->pBitmap);
                }

                //ssd1306_invert_display(inverted);
                //we should move a piece down in response to this event
                break;
            case UI_MSG_TYPE_BUTTON_PRESS:
                u32Button = uiEvent.uInpuType.butttonPressed;
                am_util_debug_printf("GameTask: Button Press Received %u\r\n", u32Button);
                //Clear last Drawing of tetrimino

                gfx_Rect_t rect1 = {xPos,yPos,longI.uWidth,longI.uHeight};//gfx_rect_Make(0,0,25, 64);
                gfx_2d_FillRect(pDC,&rect1, pDC->pPalette[PALETTE_BG_COLOR_IDX]);

                switch(u32Button){
                case 0:
                    //Left Button
                    led0 = !led0;

                    if(xPos > 5) {
                        xPos -= 6;
                    }

                    /*
                    gfx_RectNode_t *activeClipRegionNode = gfx_clip_Get();
                    activeClipRegionNode->pNext = NULL;
                    activeClipRegionNode->Rect = gfx_rect_Make(0,0,ptr->uWidth - 1, ptr->uHeight - 1);
                    pDC->pActiveClipRegion = activeClipRegionNode;

                    //gfx_Rect_t myRect = gfx_rect_Make(rand()%88,rand()%24,40, 40);
                    static gfx_Rect_t rect1 = {0,0,25,64};//gfx_rect_Make(0,0,25, 64);
                    static gfx_Rect_t rect2 = {102,0,26,64};//gfx_rect_Make(102,0,26,64);
                    //gfx_2d_FillRect(pDC,&myRect, RGB(0xFF,0xFF,0xFF));
                    //gfx_bmp_DrawBitmapByAddr(pDC, (gfx_Bitmap_t *) jMasks, (x+10)%128, (y+1)%64);
                    gfx_2d_FillRect(pDC,&rect1, RGB(0,0,0));
                    gfx_2d_FillRect(pDC,&rect2, RGB(0,0,0));
                    gfx_bmp_DrawBitmapByAddr(pDC, &buddha_sideways, 25,0);

                    pDC->bDirty = TRUE;
                    ssd1306_show_frame(pDC->pBitmap);
                    if(pDC->pActiveClipRegion){
                        gfx_clip_ReleaseClipChain(pDC->pActiveClipRegion);
                    }
                    */
                    break;
                case 1:
                    //Middle Button
                    led1 = !led1;

                    if((yPos + longI.uHeight + 6) < ptr->uHeight){
                        yPos += 6;
                    }

                    break;
                case 2:
                    //Right Button
                    led2 = !led2;

                    if((xPos + longI.uWidth + 6) < ptr->uWidth){
                        xPos += 6;
                    }

                    break;
                default:
                    am_util_debug_printf("GameTask: Unknown Button Press Detected\r\n");
                    break;
                }

                gfx_bmp_DrawBitmapByAddr(pDC, &longI, xPos,yPos);
                pDC->bDirty = TRUE;
                ssd1306_show_frame(pDC->pBitmap);
                //rotate or move a piece
                break;
            case UI_MSG_TYPE_VOICE_COMMAND:
                am_util_debug_printf("GameTask: Voice Command Received\r\n");
                break;
            default:
                am_util_debug_printf("GameTask: Error unknown msgType: %d\r\n", uiEvent.msgType);
                break;
            }
        }
    }

    //in FreeRTOS, a task should never return unless it is first removed from the scheduler
    vTaskDelete(NULL);

}

void HeartBeatTimerCallback(TimerHandle_t xTimer)
{
    UIMessage_t uiEvent;
    uiEvent.msgType = UI_MSG_TYPE_HEARTBEAT;
    xQueueSend(gameUIQueue, &uiEvent, 0);
}
