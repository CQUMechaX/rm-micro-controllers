#include "app/pingpong.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX/time.h" // Which has self-implemented type
#include "FreeRTOS_POSIX/unistd.h"
#include "cmsis_os2.h"
#include "task.h"
#include "main.h"

#include <rcl/rcl/src/rcl/context_impl.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <rcl/context.h>
#include <rcl/init_options.h>
#include <rclc/types.h>
#include <rmw/init_options.h>
#include <rmw_microros/rmw_microros.h>

#include <std_msgs/msg/header.h>

#include <stdio.h>
#include <unistd.h>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#define STRING_BUFFER_LEN 50

#define RCCHECK(fn)                                                                      \
	{                                                                                    \
		rcl_ret_t temp_rc = fn;                                                          \
		if ((temp_rc != RCL_RET_OK))                                                     \
		{                                                                                \
			printf("Failed status on line %d: %d. Aborting.\n", __LINE__, (int)temp_rc); \
			vTaskDelete(NULL);                                                           \
		}                                                                                \
	}
#define RCSOFTCHECK(fn)                                                                    \
	{                                                                                      \
		rcl_ret_t temp_rc = fn;                                                            \
		if ((temp_rc != RCL_RET_OK))                                                       \
		{                                                                                  \
			printf("Failed status on line %d: %d. Continuing.\n", __LINE__, (int)temp_rc); \
		}                                                                                  \
	}

rcl_publisher_t ping_publisher;
rcl_publisher_t pong_publisher;
rcl_subscription_t ping_subscriber;
rcl_subscription_t pong_subscriber;

std_msgs__msg__Header incoming_ping;
std_msgs__msg__Header outcoming_ping;
std_msgs__msg__Header incoming_pong;

int device_id;
int seq_no;
int pong_count;

void ping_timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
	RCLC_UNUSED(last_call_time);

	if (timer != NULL)
	{

		seq_no = rand();
		sprintf(outcoming_ping.frame_id.data, "%d_%d", seq_no, device_id);
		outcoming_ping.frame_id.size = strlen(outcoming_ping.frame_id.data);

		// Fill the message timestamp
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		outcoming_ping.stamp.sec = ts.tv_sec;
		outcoming_ping.stamp.nanosec = ts.tv_nsec;

		// Reset the pong count and publish the ping message
		pong_count = 0;

		rcl_ret_t ret_val = rcl_publish(&ping_publisher, (const void *)&outcoming_ping, NULL);
		RCL_UNUSED(ret_val);

		printf("Ping send seq %s\n", outcoming_ping.frame_id.data);
	}
}

void ping_subscription_callback(const void *msgin)
{
	const std_msgs__msg__Header *msg = (const std_msgs__msg__Header *)msgin;

	// Dont pong my own pings
	if (strcmp(outcoming_ping.frame_id.data, msg->frame_id.data) != 0)
	{
		printf("Ping received with seq %s. Answering.\n", msg->frame_id.data);
		rcl_ret_t ret_val = rcl_publish(&pong_publisher, (const void *)msg, NULL);
		RCL_UNUSED(ret_val);
	}
}

void pong_subscription_callback(const void *msgin)
{
	const std_msgs__msg__Header *msg = (const std_msgs__msg__Header *)msgin;

	if (strcmp(outcoming_ping.frame_id.data, msg->frame_id.data) == 0)
	{
		pong_count++;
		printf("Pong for seq %s (%d)\n", msg->frame_id.data, pong_count);
	}
}

/*inline*/ bool microRosAppInit(rcl_allocator_t * allocator, rcl_node_t * node, rclc_support_t * support, rclc_executor_t * executor)
{
	#define INIT_FAIL_RETURN_FALSE(fn) \
	{ \
		rcl_ret_t temp_rc = fn; \
		if ((temp_rc != RCL_RET_OK)) \
		{ \
			printf("Failed status on line %d: %d. Aborting.\n", __LINE__, (int)temp_rc); \
			return false; \
		} \
	}
	static rcl_timer_t timer;
	// support.context.impl -> init_options.impl -> rmw_init_options -> impl = (rmw_uxrce_transport_params_t){};
	// create init_options
	INIT_FAIL_RETURN_FALSE(rclc_support_init(support, 0, NULL, allocator));

	// create node
	INIT_FAIL_RETURN_FALSE(rclc_node_init_default(node, "pingpong_node", "", support));

	// Create a reliable ping publisher
	INIT_FAIL_RETURN_FALSE(rclc_publisher_init_default(&ping_publisher, node,
										ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/ping"));

	// Create a best effort pong publisher
	INIT_FAIL_RETURN_FALSE(rclc_publisher_init_best_effort(&pong_publisher, node,
											ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/pong"));

	// Create a best effort ping subscriber
	INIT_FAIL_RETURN_FALSE(rclc_subscription_init_best_effort(&ping_subscriber, node,
											   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/ping"));

	// Create a best effort  pong subscriber
	INIT_FAIL_RETURN_FALSE(rclc_subscription_init_best_effort(&pong_subscriber, node,
											   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header), "/microROS/pong"));

	// Create a 3 seconds ping timer timer,
	INIT_FAIL_RETURN_FALSE(rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(2000), ping_timer_callback));

	// Create executor
	INIT_FAIL_RETURN_FALSE(rclc_executor_init(executor, &support->context, 3, allocator));
	INIT_FAIL_RETURN_FALSE(rclc_executor_add_timer(executor, &timer));
	INIT_FAIL_RETURN_FALSE(rclc_executor_add_subscription(executor, &ping_subscriber, &incoming_ping,
										   &ping_subscription_callback, ON_NEW_DATA));
	INIT_FAIL_RETURN_FALSE(rclc_executor_add_subscription(executor, &pong_subscriber, &incoming_pong,
										   &pong_subscription_callback, ON_NEW_DATA));

	// Create and allocate the pingpong messages
	char outcoming_ping_buffer[STRING_BUFFER_LEN];
	outcoming_ping.frame_id.data = outcoming_ping_buffer;
	outcoming_ping.frame_id.capacity = STRING_BUFFER_LEN;

	char incoming_ping_buffer[STRING_BUFFER_LEN];
	incoming_ping.frame_id.data = incoming_ping_buffer;
	incoming_ping.frame_id.capacity = STRING_BUFFER_LEN;

	char incoming_pong_buffer[STRING_BUFFER_LEN];
	incoming_pong.frame_id.data = incoming_pong_buffer;
	incoming_pong.frame_id.capacity = STRING_BUFFER_LEN;

	device_id = rand();

	return true;
}

/*inline*/ bool microRosAppFini(rcl_allocator_t * allocator, rcl_node_t * node, rclc_support_t * support, rclc_executor_t * executor)
{
	// Free resources
	RCSOFTCHECK(rcl_publisher_fini(&ping_publisher, node));
	RCSOFTCHECK(rcl_publisher_fini(&pong_publisher, node));
	RCSOFTCHECK(rcl_subscription_fini(&ping_subscriber, node));
	RCSOFTCHECK(rcl_subscription_fini(&pong_subscriber, node));
	RCSOFTCHECK(rcl_node_fini(node));
	RCSOFTCHECK(rclc_support_fini(support));
	return true;
}

void microRosAppMain(void *arguments)
{
	rcl_allocator_t allocator = rcl_get_default_allocator();
	rcl_node_t node = rcl_get_zero_initialized_node(); // avoid NO NULL address reference error.
	rclc_support_t support;
	rclc_executor_t executor = rclc_executor_get_zero_initialized_executor(); // just for fun.

	osDelay(1000); // wait to beep end... wait for usb avalible (must be avalible?)

	while(true)
	{
		if(microRosAppInit(&allocator, &node, &support, &executor))
		{
			while (true)
			{
				rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
				usleep(100);
			}
		}
		microRosAppFini(&allocator, &node, &support, &executor);
        uxTaskGetStackHighWaterMark(NULL);
		osDelay(5000);
	}

}
