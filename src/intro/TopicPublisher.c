/** @example ex/Intro/TopicPublisher.c
 */
/*
 *  Copyright 2012-2019 Solace Corporation. All rights reserved.
 *
 *  http://www.solace.com
 *
 *  This source is distributed under the terms and conditions
 *  of any contract or contracts between Solace and you or
 *  your company. If there are no contracts in place use of
 *  this source is not authorized. No support is provided and
 *  no distribution, sharing with others or re-use of this
 *  source is authorized unless specifically stated in the
 *  contracts referred to above.
 *
 *  TopicPublisher
 *
 *  This sample shows the basics of creating session, connecting a session,
 *  and publishing a direct message to a topic. This is meant to be a very
 *  basic example for demonstration purposes.
 */

#include <time.h>

#include "os.h"
#include "solclient/solClient.h"
#include "solclient/solClientMsg.h"

#include "heartbeat_builder.h"
#include "arg_parser.h"

/*****************************************************************************
 * sessionMessageReceiveCallback
 *
 * The message receive callback function is mandatory for session creation.
 *****************************************************************************/
solClient_rxMsgCallback_returnCode_t
sessionMessageReceiveCallback ( solClient_opaqueSession_pt opaqueSession_p, solClient_opaqueMsg_pt msg_p, void *user_p )
{
    return SOLCLIENT_CALLBACK_OK;
}

/*****************************************************************************
 * sessionEventCallback
 *
 * The event callback function is mandatory for session creation.
 *****************************************************************************/
void
sessionEventCallback ( solClient_opaqueSession_pt opaqueSession_p,
                solClient_session_eventCallbackInfo_pt eventInfo_p, void *user_p )
{
}

/*****************************************************************************
 * main
 * 
 * The entry point to the application.
 *****************************************************************************/
int
main ( int argc, char *argv[] )
{
    /* Context */
    solClient_opaqueContext_pt context_p;
    solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;

    /* Session */
    solClient_opaqueSession_pt session_p;
    solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;

    /* Session Properties */
    const char     *sessionProps[20] = {0, };
    int             propIndex = 0;

    /* Message */
    solClient_opaqueMsg_pt msg_p = NULL;
    solClient_destination_t destination;
    const char *text_p = "Hello world!";

    /* Arguments */
    char* solace_host;
    char* solace_vpn;
    char* solace_user;
    char* solace_pwd;
    char* hb_host;
    char* hb_service_type;
    char* hb_instance;
    char* hb_version;
    char hb_status;
    char* hb_comments;
    uint64_t hb_start_time;

    get_args(argc, argv,
        &solace_host,
        &solace_vpn,
        &solace_user,
        &solace_pwd,
        &hb_host,
        &hb_service_type,
        &hb_instance,
        &hb_version,
        &hb_status,
        &hb_comments,
        &hb_start_time);

    char topic[1000] = "v2/";

    strcat(topic, hb_service_type);
    strcat(topic, "/mgmt/");
    strcat(topic, hb_instance);
    strcat(topic, "/heartbeat");

    /*************************************************************************
     * Initialize the API (and setup logging level)
     *************************************************************************/

    /* solClient needs to be initialized before any other API calls. */
    solClient_initialize ( SOLCLIENT_LOG_DEFAULT_FILTER, NULL );
    printf ( "TopicPublisher initializing...\n" );

    /*************************************************************************
     * Create a Context
     *************************************************************************/

    /* 
     * Create a Context, and specify that the Context thread be created 
     * automatically instead of having the application create its own
     * Context thread.
     */
    solClient_context_create ( SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                               &context_p, &contextFuncInfo, sizeof ( contextFuncInfo ) );

    /*************************************************************************
     * Create and connect a Session
     *************************************************************************/

    /*
     * Message receive callback function and the Session event function
     * are both mandatory. In this sample, default functions are used.
     */
    sessionFuncInfo.rxMsgInfo.callback_p = sessionMessageReceiveCallback;
    sessionFuncInfo.rxMsgInfo.user_p = NULL;
    sessionFuncInfo.eventInfo.callback_p = sessionEventCallback;
    sessionFuncInfo.eventInfo.user_p = NULL;

    /* Configure the Session properties. */
    propIndex = 0;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_HOST;
    sessionProps[propIndex++] = solace_host;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_VPN_NAME;
    sessionProps[propIndex++] = solace_vpn;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_USERNAME;
    sessionProps[propIndex++] = solace_user;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_PASSWORD;
    sessionProps[propIndex++] = solace_pwd;

    /* Create the Session. */
    solClient_session_create ( ( char ** ) sessionProps,
                               context_p,
                               &session_p, &sessionFuncInfo, sizeof ( sessionFuncInfo ) );

    /* Connect the Session. */
    solClient_session_connect ( session_p );
    printf ( "Connected.\n" );

    /*************************************************************************
     * Publish
     *************************************************************************/

    /* Allocate memory for the message that is to be sent. */
    solClient_msg_alloc ( &msg_p );

    /* Set the message delivery mode. */
    solClient_msg_setDeliveryMode ( msg_p, SOLCLIENT_DELIVERY_MODE_DIRECT );

    /* Set the destination. */
    destination.destType = SOLCLIENT_TOPIC_DESTINATION;
    destination.dest = topic;
    solClient_msg_setDestination ( msg_p, &destination, sizeof ( destination ) );

    /* Set the user data. */
    const char *protocol_version = "3.1.66";

    char* user_data_ptr = NULL;
    size_t user_data_len = 0;
    uint64_t crc = 0x20E04842;

    userdata_msg_builder(&user_data_ptr, &user_data_len, crc, protocol_version);

    solClient_msg_setUserData ( msg_p, user_data_ptr, ( solClient_uint32_t ) user_data_len );

    /* Add some content to the message. */
    uint64_t hb_update_time = time(NULL) * 1e9;

    char* hb_msg_ptr = NULL;
    size_t hb_msg_len = 0;

    heartbeat_msg_builider(
        &hb_msg_ptr,
        &hb_msg_len,
        hb_host,
        hb_service_type,
        hb_instance,
        hb_version,
        hb_status,
        hb_comments,
        hb_start_time,
        hb_update_time);

    solClient_msg_setBinaryAttachment ( msg_p, hb_msg_ptr, ( solClient_uint32_t ) hb_msg_len );

    /* Send the message. */
    // printf ( "About to send message '%s' to topic '%s'...\n", (char *)text_p, topic );
    solClient_session_sendMsg ( session_p, msg_p );

    /* Free the message. */
    printf ( "Message sent. Exiting.\n" );
    solClient_msg_free ( &msg_p );

    /*************************************************************************
     * Cleanup
     *************************************************************************/

    /* Cleanup solClient. */
    solClient_cleanup (  );

    free(user_data_ptr);
    free(hb_msg_ptr);

    return 0;
}
