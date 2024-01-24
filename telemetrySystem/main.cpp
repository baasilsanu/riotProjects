#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include "msg.h"
#include "ztimer.h"
#include "can/can.h" 

#define RECEIVER_THREAD_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)
#define SENDER_THREAD_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)
#define CAN_MSG_QUEUE_LENGTH 8
#define CAN_FRAME_ID 0x123
#define TIMER_INTERVAL_SEC 1

static msg_t receiver_msg_queue[CAN_MSG_QUEUE_LENGTH];
static kernel_pid_t receiver_thread_pid;

static void *receiver_thread(void *arg) {
    (void)arg;
    msg_t msg;
    can_frame frame;

    msg_init_queue(receiver_msg_queue, CAN_MSG_QUEUE_LENGTH);

    while (1) {
        msg_receive(&msg);
        memcpy(&frame, msg.content.ptr, sizeof(can_frame));
        printf("Received CAN frame with digits: ");
        for (int i = 0; i < frame.can_dlc; ++i) {
            printf("%d", frame.data[i]);
        }
        printf("\n");
        free(msg.content.ptr);
    }

    return NULL;
}
bool toggle = false;
static void *sender_thread(void *arg) {
    (void)arg;
    

    while (1) {
        can_frame frame;
        frame.can_id = CAN_FRAME_ID | CAN_EFF_FLAG;
        frame.can_dlc = 8;

        if (toggle) {
            // Send the digits 35897932
            frame.data[0] = 3;
            frame.data[1] = 5;
            frame.data[2] = 8;
            frame.data[3] = 9;
            frame.data[4] = 7;
            frame.data[5] = 9;
            frame.data[6] = 3;
            frame.data[7] = 2;
        } else {
            // Send the digits 31415926
            frame.data[0] = 3;
            frame.data[1] = 1;
            frame.data[2] = 4;
            frame.data[3] = 1;
            frame.data[4] = 5;
            frame.data[5] = 9;
            frame.data[6] = 2;
            frame.data[7] = 6;
        }

        toggle = !toggle; // Toggle the frame to be sent next time

        can_frame *frame_ptr = (can_frame *)malloc(sizeof(can_frame));
        if (!frame_ptr) {
            printf("Error: unable to allocate memory for CAN frame\n");
            continue;
        }
        *frame_ptr = frame;

        msg_t msg;
        msg.content.ptr = (void *)frame_ptr;
        msg_send(&msg, receiver_thread_pid);

        ztimer_sleep(ZTIMER_SEC, TIMER_INTERVAL_SEC);
    }

    return NULL;
}

char receiver_stack[RECEIVER_THREAD_STACK_SIZE];
char sender_stack[SENDER_THREAD_STACK_SIZE];

int main(void) {
    receiver_thread_pid = thread_create(receiver_stack, sizeof(receiver_stack),
                                        THREAD_PRIORITY_MAIN - 1,
                                        THREAD_CREATE_STACKTEST,
                                        receiver_thread,
                                        NULL, "Receiver Thread");

    thread_create(sender_stack, sizeof(sender_stack),
                  THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST,
                  sender_thread,
                  NULL, "Sender Thread");

    thread_create(sender_stack, sizeof(sender_stack),
                  THREAD_PRIORITY_MAIN - 3,
                  THREAD_CREATE_STACKTEST,
                  sender_thread,
                  NULL, "Sender Thread");

    while(1) {
        ztimer_sleep(ZTIMER_SEC, 1);
    }

    return 0;
}
