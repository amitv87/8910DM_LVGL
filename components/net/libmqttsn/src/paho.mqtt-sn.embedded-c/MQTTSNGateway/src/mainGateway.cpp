/**************************************************************************************
 * Copyright (c) 2016, Tomoaki Yamaguchi
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Tomoaki Yamaguchi - initial API and implementation and/or initial documentation
 **************************************************************************************/
#include "MQTTSNGateway.h"
#include "MQTTSNGWBrokerRecvTask.h"
#include "MQTTSNGWBrokerSendTask.h"
#include "MQTTSNGWClientRecvTask.h"
#include "MQTTSNGWClientSendTask.h"
#include "MQTTSNGWPacketHandleTask.h"

using namespace MQTTSNGW;

/*
 *  Gateway Process
 */
Gateway* gw = new Gateway();
PacketHandleTask  task1(gw);
ClientRecvTask    task2(gw);
ClientSendTask    task3(gw);
BrokerRecvTask    task4(gw);
BrokerSendTask    task5(gw);

int main(int argc, char** argv)
{
    gw->initialize(argc, argv);
    gw->run();
    delete gw;
    return 0;
}
