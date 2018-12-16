#include <cstring>

#include "mqttsn_client.hpp"
#include "MQTTSNPacket.h"
#include "MQTTSNConnect.h"
#include "MQTTSNSearch.h"
#include "fsl_debug_console.h"

#define MAX_PACKET_SIZE 255
#define KEEP_ALIVE_DELAY 5
#define MQTTSN_MIN_PACKET_LENGTH 2

namespace ot {

namespace Mqttsn {
// TODO: Implement QoS and DUP behavior
// TODO: Implement OT logging

template <typename CallbackType>
MessageMetadata<CallbackType>::MessageMetadata()
{
    ;
}

template <typename CallbackType>
MessageMetadata<CallbackType>::MessageMetadata(const Ip6::Address &aDestinationAddress, uint16_t aDestinationPort, uint16_t aPacketId, uint32_t aTimestamp, uint32_t aRetransmissionTimeout, CallbackType aCallback, void* aContext)
    : mDestinationAddress(aDestinationAddress)
    , mDestinationPort(aDestinationPort)
    , mPacketId(aPacketId)
    , mTimestamp(aTimestamp)
    , mRetransmissionTimeout(aRetransmissionTimeout)
    , mRetransmissionCount(0)
    , mCallback(aCallback)
    , mContext(aContext)
{
    ;
}

template <typename CallbackType>
otError MessageMetadata<CallbackType>::AppendTo(Message &aMessage) const
{
    return aMessage.Append(this, sizeof(*this));
}

template <typename CallbackType>
uint16_t MessageMetadata<CallbackType>::ReadFrom(Message &aMessage)
{
    return aMessage.Read(aMessage.GetLength() - sizeof(*this), sizeof(*this), this);
}

template <typename CallbackType>
uint16_t MessageMetadata<CallbackType>::GetLength() const
{
    return sizeof(*this);
}

template <typename CallbackType>
WaitingMessagesQueue<CallbackType>::WaitingMessagesQueue(TimeoutCallbackFunc aTimeoutCallback, void* aTimeoutContext)
    : mQueue()
    , mTimeoutCallback(aTimeoutCallback)
    , mTimeoutContext(aTimeoutContext)
{
    ;
}

template <typename CallbackType>
WaitingMessagesQueue<CallbackType>::~WaitingMessagesQueue(void)
{
    ForceTimeout();
}

template <typename CallbackType>
otError WaitingMessagesQueue<CallbackType>::EnqueueCopy(const Message &aMessage, uint16_t aLength, const MessageMetadata<CallbackType> &aMetadata)
{
    otError error = OT_ERROR_NONE;
    Message *messageCopy = nullptr;

    VerifyOrExit((messageCopy = aMessage.Clone(aLength)) != NULL, error = OT_ERROR_NO_BUFS);
    SuccessOrExit(error = aMetadata.AppendTo(*messageCopy));
    SuccessOrExit(error = mQueue.Enqueue(*messageCopy));

exit:
    return error;
}

template <typename CallbackType>
otError WaitingMessagesQueue<CallbackType>::Dequeue(Message &aMessage)
{
    otError error = mQueue.Dequeue(aMessage);
    aMessage.Free();
    return error;
}

template <typename CallbackType>
Message* WaitingMessagesQueue<CallbackType>::Find(uint16_t aPacketId, MessageMetadata<CallbackType> &aMetadata)
{
    Message* message = mQueue.GetHead();
    while (message)
    {
        aMetadata.ReadFrom(*message);
        if (aPacketId == aMetadata.mPacketId)
        {
            return message;
        }
        message = message->GetNext();
    }
    return nullptr;
}

template <typename CallbackType>
otError WaitingMessagesQueue<CallbackType>::HandleTimer()
{
    otError error = OT_ERROR_NONE;
    Message* message = mQueue.GetHead();
    while (message)
    {
        Message* current = message;
        message = message->GetNext();
        MessageMetadata<CallbackType> metadata;
        metadata.ReadFrom(*current);
        if (metadata.mTimestamp + metadata.mRetransmissionTimeout <= TimerMilli::GetNow())
        {
            if (mTimeoutCallback)
            {
                mTimeoutCallback(metadata, mTimeoutContext);
            }
            SuccessOrExit(error = Dequeue(*current));
        }
    }
exit:
    return error;
}

template <typename CallbackType>
void WaitingMessagesQueue<CallbackType>::ForceTimeout()
{
    Message* message = mQueue.GetHead();
    while (message)
    {
        Message* current = message;
        message = message->GetNext();
        MessageMetadata<CallbackType> metadata;
        metadata.ReadFrom(*current);
        if (mTimeoutCallback)
        {
            mTimeoutCallback(metadata, mTimeoutContext);
        }
        Dequeue(*current);
    }
}



MqttsnClient::MqttsnClient(Instance& instance)
    : InstanceLocator(instance)
    , mSocket(GetInstance().GetThreadNetif().GetIp6().GetUdp())
    , mConfig()
    , mPacketId(1)
    , mPingReqTime(0)
    , mGwTimeout(0)
    , mDisconnectRequested(false)
    , mSleepRequested(false)
    , mClientState(kStateDisconnected)
    , mSubscribeQueue(HandleSubscribeTimeout, this)
    , mRegisterQueue(HandleRegisterTimeout, this)
    , mUnsubscribeQueue(HandleUnsubscribeTimeout, this)
    , mConnectCallback(nullptr)
    , mConnectContext(nullptr)
    , mPublishReceivedCallback(nullptr)
    , mPublishReceivedContext(nullptr)
    , mAdvertiseCallback(nullptr)
    , mAdvertiseContext(nullptr)
    , mSearchGwCallback(nullptr)
    , mSearchGwContext(nullptr)
    , mPublishedCallback(nullptr)
    , mPublishedContext(nullptr)
    , mDisconnectedCallback(nullptr)
    , mDisconnectedContext(nullptr)
{
    ;
}

MqttsnClient::~MqttsnClient()
{
    mSocket.Close();
    OnDisconnected();
}

static int32_t PacketDecode(unsigned char* aData, size_t aLength)
{
    int lenlen = 0;
    int datalen = 0;

    if (aLength < MQTTSN_MIN_PACKET_LENGTH)
    {
        return MQTTSNPACKET_READ_ERROR;
    }

    lenlen = MQTTSNPacket_decode(aData, aLength, &datalen);
    if (datalen != static_cast<int>(aLength))
    {
        return MQTTSNPACKET_READ_ERROR;
    }
    return aData[lenlen]; // return the packet type
}

void MqttsnClient::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    MqttsnClient* client = static_cast<MqttsnClient*>(aContext);
    Message &message = *static_cast<Message *>(aMessage);
    const Ip6::MessageInfo &messageInfo = *static_cast<const Ip6::MessageInfo *>(aMessageInfo);

    uint16_t offset = message.GetOffset();
    uint16_t length = message.GetLength() - message.GetOffset();

    unsigned char data[MAX_PACKET_SIZE];

    if (length > MAX_PACKET_SIZE)
    {
        return;
    }

    if (!data)
    {
        return;
    }
    message.Read(offset, length, data);

    // Dump packet
    PRINTF("UDP message received:\r\n");
    for (int32_t i = 0; i < length; i++)
    {
        if (i > 0)
        {
            PRINTF(" ");
        }
        PRINTF("%02X", data[i]);
    }
    PRINTF("\r\n");

    int32_t decodedPacketType = PacketDecode(data, length);
    if (decodedPacketType == MQTTSNPACKET_READ_ERROR)
    {
        return;
    }
    PRINTF("Packet type: %d\r\n", decodedPacketType);

    // TODO: Refactor switch to use separate handle functions
    switch (decodedPacketType)
    {
    case MQTTSN_CONNACK:
        {
        int connectReturnCode = 0;
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }
        if (MQTTSNDeserialize_connack(&connectReturnCode, data, length) != 1)
        {
            break;
        }
        client->mClientState = kStateActive;
        client->mGwTimeout = 0;
        if (client->mConnectCallback)
        {
            client->mConnectCallback(static_cast<ReturnCode>(connectReturnCode),
                client->mConnectContext);
        }
    }
        break;
    case MQTTSN_SUBACK:
        {
        if (client->mClientState != kStateActive)
        {
            break;
        }
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }
        int qos;
        unsigned short topicId = 0;
        unsigned short packetId = 0;
        unsigned char subscribeReturnCode = 0;
        if (MQTTSNDeserialize_suback(&qos, &topicId, &packetId, &subscribeReturnCode,
            data, length) != 1)
        {
            break;
        }

        MessageMetadata<SubscribeCallbackFunc> metadata;
        Message* message = client->mSubscribeQueue.Find(packetId, metadata);
        if (!message)
        {
            break;
        }
        if (metadata.mCallback)
        {
            metadata.mCallback(static_cast<ReturnCode>(subscribeReturnCode), static_cast<TopicId>(topicId), metadata.mContext);
        }
        client->mSubscribeQueue.Dequeue(*message);
    }
        break;
    case MQTTSN_PUBLISH:
        {
        if (client->mClientState != kStateActive && client->mClientState != kStateAwake)
        {
            break;
        }
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }
        int qos;
        unsigned short packetId = 0;
        int payloadLength = 0;
        unsigned char* payload;
        unsigned char dup = 0;
        unsigned char retained = 0;
        MQTTSN_topicid topicId;
        if (MQTTSNDeserialize_publish(&dup, &qos, &retained, &packetId,
            &topicId, &payload, &payloadLength, data, length) != 1)
        {
            break;
        }
        if (client->mPublishReceivedCallback)
        {
            client->mPublishReceivedCallback(payload, payloadLength, static_cast<Qos>(qos),
                static_cast<TopicId>(topicId.data.id), client->mPublishReceivedContext);
        }
    }
        break;
    case MQTTSN_ADVERTISE:
        {
        unsigned char gatewayId = 0;
        unsigned short duration = 0;
        if (MQTTSNDeserialize_advertise(&gatewayId, &duration, data, length) != 1)
        {
            break;
        }
        if (client->mAdvertiseCallback)
        {
            client->mAdvertiseCallback(messageInfo.GetPeerAddr(), static_cast<uint8_t>(gatewayId),
                static_cast<uint32_t>(duration), client->mAdvertiseContext);
        }
    }
        break;
    case MQTTSN_GWINFO:
        {
        unsigned char gatewayId = 0;
        unsigned short addressLength = 1;
        unsigned char* addressText = nullptr;
        if (MQTTSNDeserialize_gwinfo(&gatewayId, &addressLength, &addressText, data, length) != 1)
        {
            break;
        }
        if (client->mSearchGwCallback)
        {
            Ip6::Address address;
            if (addressLength > 0)
            {
                char addressBuffer[40];
                memset(addressBuffer, 0 ,sizeof(addressBuffer));
                memcpy(addressBuffer, addressText, addressLength);
                address.FromString(addressBuffer);
            }
            else
            {
                address = messageInfo.GetPeerAddr();
            }
            client->mSearchGwCallback(address, gatewayId, client->mSearchGwContext);
        }
    }
        break;
    case MQTTSN_REGACK:
        {
        if (client->mClientState != kStateActive)
        {
            break;
        }
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }

        unsigned short topicId;
        unsigned short packetId;
        unsigned char returnCode;
        if (MQTTSNDeserialize_regack(&topicId, &packetId, &returnCode, data, length) != 1)
        {
            break;
        }
        MessageMetadata<RegisterCallbackFunc> metadata;
        Message* message = client->mRegisterQueue.Find(packetId, metadata);
        if (!message)
        {
            break;
        }
        if (metadata.mCallback)
        {
            metadata.mCallback(static_cast<ReturnCode>(returnCode), static_cast<TopicId>(topicId), metadata.mContext);
        }
        client->mRegisterQueue.Dequeue(*message);
    }
        break;
    case MQTTSN_PUBACK:
        {
        if (client->mClientState != kStateActive)
        {
            break;
        }
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }

        unsigned short topicId;
        unsigned short packetId;
        unsigned char returnCode;
        if (MQTTSNDeserialize_puback(&topicId, &packetId, &returnCode, data, length) != 1)
        {
            break;
        }
        if (client->mPublishedCallback)
        {
            client->mPublishedCallback(static_cast<ReturnCode>(returnCode), static_cast<TopicId>(topicId),
                client->mPublishedContext);
        }
    }
        break;
    case MQTTSN_UNSUBACK:
        {
        if (client->mClientState != kStateActive)
        {
            break;
        }
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }

        unsigned short packetId;
        if (MQTTSNDeserialize_unsuback(&packetId, data, length) != 1)
        {
            break;
        }
        MessageMetadata<UnsubscribeCallbackFunc> metadata;
        Message* message = client->mUnsubscribeQueue.Find(packetId, metadata);
        if (!message)
        {
            break;
        }
        if (metadata.mCallback)
        {
            metadata.mCallback(kCodeAccepted, metadata.mContext);
        }
        client->mUnsubscribeQueue.Dequeue(*message);
    }
        break;
    case MQTTSN_PINGREQ:
        {
        MQTTSNString clientId;
        Message* message = nullptr;
        if (client->mClientState != kStateActive)
        {
            break;
        }

        if (MQTTSNDeserialize_pingreq(&clientId, data, length) != 1)
        {
            break;
        }

        int32_t packetLength = -1;
        unsigned char buffer[MAX_PACKET_SIZE];
        packetLength = MQTTSNSerialize_pingresp(buffer, MAX_PACKET_SIZE);
        if (packetLength <= 0)
        {
            break;
        }
        if (client->NewMessage(&message, buffer, packetLength) != OT_ERROR_NONE ||
            client->SendMessage(*message, messageInfo.GetPeerAddr(), messageInfo.GetPeerPort()) != OT_ERROR_NONE)
        {
            break;
        }
    }
        break;
    case MQTTSN_PINGRESP:
        {
        if (MQTTSNDeserialize_pingresp(data, length) != 1)
        {
            break;
        }
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }

        client->mGwTimeout = 0;
        if (client->mClientState == kStateAwake)
        {
            client->mClientState = kStateAsleep;
            if (client->mDisconnectedCallback)
            {
                client->mDisconnectedCallback(kAsleep, client->mDisconnectedContext);
            }
        }
    }
        break;
    case MQTTSN_DISCONNECT:
        {

        int duration;
        if (MQTTSNDeserialize_disconnect(&duration, data, length) != 1)
        {
            break;
        }
        client->OnDisconnected();

        DisconnectType reason = kServer;
        switch (client->mClientState)
        {
        case kStateActive:
        case kStateAwake:
        case kStateAsleep:
            if (client->mDisconnectRequested)
            {
                client->mClientState = kStateDisconnected;
                reason = kServer;
            }
            else if (client->mSleepRequested)
            {
                client->mClientState = kStateAsleep;
                reason = kAsleep;
            }
            else
            {
                client->mClientState = kStateDisconnected;
                reason = kServer;
            }
            break;
        default:
            break;
        }
        if (!client->VerifyGatewayAddress(messageInfo))
        {
            break;
        }

        if (client->mDisconnectedCallback)
        {
            client->mDisconnectedCallback(reason, client->mDisconnectedContext);
        }
    }
        break;
    default:
        break;
    }
}

otError MqttsnClient::Start(uint16_t aPort)
{
    otError error = OT_ERROR_NONE;
    Ip6::SockAddr sockaddr;
    sockaddr.mPort = aPort;

    SuccessOrExit(error = mSocket.Open(MqttsnClient::HandleUdpReceive, this));
    SuccessOrExit(error = mSocket.Bind(sockaddr));

exit:
    return error;
}

otError MqttsnClient::Stop()
{
    return mSocket.Close();
    if (mClientState != kStateDisconnected && mClientState != kStateLost)
    {
        mClientState = kStateDisconnected;
        OnDisconnected();
        if (mDisconnectedCallback)
        {
            mDisconnectedCallback(kClient, mDisconnectedContext);
        }
    }
}

otError MqttsnClient::Process()
{
    otError error = OT_ERROR_NONE;

    uint32_t now = TimerMilli::GetNow();

    // Process pingreq
    if (mClientState != kStateActive && mPingReqTime != 0 && mPingReqTime <= now)
    {
        SuccessOrExit(error = PingGateway());
        mGwTimeout = TimerMilli::GetNow() + mConfig.GetGatewayTimeout() * 1000;
    }

    // Handle timeout
    if (mGwTimeout != 0 && mGwTimeout <= now)
    {
        OnDisconnected();
        mClientState = kStateLost;
        if (mDisconnectedCallback)
        {
            mDisconnectedCallback(kTimeout, mDisconnectedContext);
        }
    }

    // Handle pending messages timeouts
    mSubscribeQueue.HandleTimer();
    mRegisterQueue.HandleTimer();
    mUnsubscribeQueue.HandleTimer();

exit:
    return error;
}

otError MqttsnClient::Connect(MqttsnConfig &aConfig)
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;

    if (mClientState == kStateActive)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }
    mConfig = aConfig;

    MQTTSNString clientId;
    clientId.cstring = const_cast<char *>(aConfig.GetClientId().AsCString());
    options.clientID = clientId;
    options.duration = aConfig.GetKeepAlive();
    options.cleansession = static_cast<unsigned char>(aConfig.GetCleanSession());

    unsigned char buffer[MAX_PACKET_SIZE];
    length = MQTTSNSerialize_connect(buffer, MAX_PACKET_SIZE, &options);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }
    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message));

    mDisconnectRequested = false;
    mSleepRequested = false;
    mGwTimeout = TimerMilli::GetNow() + mConfig.GetGatewayTimeout() * 1000;
    mPingReqTime = TimerMilli::GetNow() + mConfig.GetKeepAlive() * 1000;

exit:
    return error;
}

otError MqttsnClient::Subscribe(const char* aTopicName, Qos aQos, SubscribeCallbackFunc aCallback, void* aContext)
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Ip6::MessageInfo messageInfo;
    Message *message = nullptr;
    MQTTSN_topicid topicIdConfig;
    unsigned char buffer[MAX_PACKET_SIZE];

    if (mClientState != kStateActive)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    // TODO: Implement QoS
    if (aQos != Qos::kQos0)
    {
        error = OT_ERROR_NOT_IMPLEMENTED;
        goto exit;
    }

    topicIdConfig.type = MQTTSN_TOPIC_TYPE_NORMAL;
    topicIdConfig.data.long_.name = const_cast<char*>(aTopicName);
    topicIdConfig.data.long_.len = strlen(aTopicName);

    length = MQTTSNSerialize_subscribe(buffer, MAX_PACKET_SIZE, 0, static_cast<int>(aQos), mPacketId, &topicIdConfig);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }
    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message));
    SuccessOrExit(error = mSubscribeQueue.EnqueueCopy(*message, message->GetLength(),
        MessageMetadata<SubscribeCallbackFunc>(mConfig.GetAddress(), mConfig.GetPort(), mPacketId, TimerMilli::GetNow(),
            mConfig.GetGatewayTimeout() * 1000, aCallback, aContext)));
    mPacketId++;

exit:
    return error;
}

otError MqttsnClient::Register(const char* aTopicName, RegisterCallbackFunc aCallback, void* aContext)
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    MQTTSNString topicName;
    unsigned char buffer[MAX_PACKET_SIZE];

    if (mClientState != kStateActive)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    topicName.cstring = const_cast<char*>(aTopicName);
    length = MQTTSNSerialize_register(buffer, MAX_PACKET_SIZE, 0, mPacketId, &topicName);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }
    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message));
    SuccessOrExit(error = mRegisterQueue.EnqueueCopy(*message, message->GetLength(),
        MessageMetadata<RegisterCallbackFunc>(mConfig.GetAddress(), mConfig.GetPort(), mPacketId, TimerMilli::GetNow(),
            mConfig.GetGatewayTimeout() * 1000, aCallback, aContext)));
    mPacketId++;

exit:
    return error;
}

otError MqttsnClient::Publish(const uint8_t* aData, int32_t lenght, Qos aQos, TopicId aTopicId)
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    unsigned char buffer[MAX_PACKET_SIZE];

    if (mClientState != kStateActive)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    // TODO: Implement QoS
    if (aQos != Qos::kQos0)
    {
        error = OT_ERROR_NOT_IMPLEMENTED;
        goto exit;
    }

    MQTTSN_topicid topic;
    topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
    topic.data.id = static_cast<unsigned short>(aTopicId);
    length = MQTTSNSerialize_publish(buffer, MAX_PACKET_SIZE, 0, static_cast<int>(aQos), 0,
        mPacketId++, topic, const_cast<unsigned char *>(aData), lenght);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }
    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message));

exit:
    return error;
}

otError MqttsnClient::Unsubscribe(TopicId aTopicId, UnsubscribeCallbackFunc aCallback, void* aContext)
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    unsigned char buffer[MAX_PACKET_SIZE];

    if (mClientState != kStateActive)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    MQTTSN_topicid topic;
    topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
    topic.data.id = static_cast<unsigned short>(aTopicId);
    length = MQTTSNSerialize_unsubscribe(buffer, MAX_PACKET_SIZE, mPacketId, &topic);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }
    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = mUnsubscribeQueue.EnqueueCopy(*message, message->GetLength(),
        MessageMetadata<UnsubscribeCallbackFunc>(mConfig.GetAddress(), mConfig.GetPort(), mPacketId, TimerMilli::GetNow(),
            mConfig.GetGatewayTimeout() * 1000, aCallback, aContext)));
    SuccessOrExit(error = SendMessage(*message));
    mPacketId++;

exit:
    return error;
}

otError MqttsnClient::Disconnect()
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    unsigned char buffer[MAX_PACKET_SIZE];

    if (mClientState != kStateActive && mClientState != kStateAwake
        && mClientState != kStateAsleep)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    length = MQTTSNSerialize_disconnect(buffer, MAX_PACKET_SIZE, 0);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }
    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message));

    mDisconnectRequested = true;
    mGwTimeout = TimerMilli::GetNow() + mConfig.GetGatewayTimeout() * 1000;

exit:
    return error;
}

otError MqttsnClient::Sleep(uint16_t aDuration)
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    unsigned char buffer[MAX_PACKET_SIZE];

    if (mClientState != kStateActive && mClientState != kStateAwake && mClientState != kStateAsleep)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    length = MQTTSNSerialize_disconnect(buffer, MAX_PACKET_SIZE, aDuration);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }

    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message));

    mSleepRequested = true;
    mGwTimeout = TimerMilli::GetNow() + mConfig.GetGatewayTimeout() * 1000;

exit:
    return error;
}

otError MqttsnClient::Awake(uint32_t aTimeout)
{
    otError error = OT_ERROR_NONE;
    if (mClientState != kStateAwake && mClientState != kStateAsleep)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    SuccessOrExit(error = PingGateway());

    mClientState = kStateAwake;
    mGwTimeout = TimerMilli::GetNow() + aTimeout * 1000;
exit:
    return error;
}

otError MqttsnClient::SearchGateway(const Ip6::Address &aMulticastAddress, uint16_t aPort, uint8_t aRadius)
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    unsigned char buffer[MAX_PACKET_SIZE];

    length = MQTTSNSerialize_searchgw(buffer, MAX_PACKET_SIZE, aRadius);
    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }

    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message, aMulticastAddress, aPort, aRadius));

exit:
    return error;
}

ClientState MqttsnClient::GetState(ClientState aState)
{
    return mClientState;
}

otError MqttsnClient::SetConnectedCallback(ConnectCallbackFunc aCallback, void* aContext)
{
    mConnectCallback = aCallback;
    mConnectContext = aContext;
    return OT_ERROR_NONE;
}

otError MqttsnClient::SetPublishReceivedCallback(PublishReceivedCallbackFunc aCallback, void* aContext)
{
    mPublishReceivedCallback = aCallback;
    mPublishReceivedContext = aContext;
    return OT_ERROR_NONE;
}

otError MqttsnClient::SetAdvertiseCallback(AdvertiseCallbackFunc aCallback, void* aContext)
{
    mAdvertiseCallback = aCallback;
    mAdvertiseContext = aContext;
    return OT_ERROR_NONE;
}

otError MqttsnClient::SetSearchGwCallback(SearchGwCallbackFunc aCallback, void* aContext)
{
    mSearchGwCallback = aCallback;
    mSearchGwContext = aContext;
    return OT_ERROR_NONE;
}

otError MqttsnClient::SetPublishedCallback(PublishedCallbackFunc aCallback, void* aContext)
{
    mPublishedCallback = aCallback;
    mPublishedContext = aContext;
    return OT_ERROR_NONE;
}

otError MqttsnClient::SetDisconnectedCallback(DisconnectedCallbackFunc aCallback, void* aContext)
{
    mDisconnectedCallback = aCallback;
    mDisconnectedContext = aContext;
    return OT_ERROR_NONE;
}

otError MqttsnClient::NewMessage(Message **aMessage, unsigned char* aBuffer, int32_t aLength)
{
    otError error = OT_ERROR_NONE;
    Message *message = nullptr;

    VerifyOrExit((message = mSocket.NewMessage(0)) != nullptr,
        error = OT_ERROR_NO_BUFS);
    SuccessOrExit(error = message->Append(aBuffer, aLength));
    *aMessage = message;

exit:
    if (error != OT_ERROR_NONE && message != nullptr)
    {
        message->Free();
    }
    return error;
}

otError MqttsnClient::SendMessage(Message &aMessage)
{
    return SendMessage(aMessage, mConfig.GetAddress(), mConfig.GetPort());
}

otError MqttsnClient::SendMessage(Message &aMessage, const Ip6::Address &aAddress, uint16_t aPort)
{
    return SendMessage(aMessage, aAddress, aPort, 0);
}

otError MqttsnClient::SendMessage(Message &aMessage, const Ip6::Address &aAddress, uint16_t aPort, uint8_t aHopLimit)
{
    otError error = OT_ERROR_NONE;
    Ip6::MessageInfo messageInfo;

    messageInfo.SetHopLimit(aHopLimit);
    messageInfo.SetPeerAddr(aAddress);
    messageInfo.SetPeerPort(aPort);
    messageInfo.SetInterfaceId(OT_NETIF_INTERFACE_ID_THREAD);

    PRINTF("Sending message to %s[:%u]\r\n", messageInfo.GetPeerAddr().ToString().AsCString(), messageInfo.GetPeerPort());
    SuccessOrExit(error = mSocket.SendTo(aMessage, messageInfo));

    if (mClientState == kStateActive)
    {
        mPingReqTime = TimerMilli::GetNow() + (mConfig.GetKeepAlive() - KEEP_ALIVE_DELAY) * 1000;
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        aMessage.Free();
    }
    return error;
}

otError MqttsnClient::PingGateway()
{
    otError error = OT_ERROR_NONE;
    int32_t length = -1;
    Message* message = nullptr;
    unsigned char buffer[MAX_PACKET_SIZE];

    if (mClientState != kStateActive && mClientState != kStateAwake)
    {
        error = OT_ERROR_INVALID_STATE;
        goto exit;
    }

    MQTTSNString clientId;
    clientId.cstring = const_cast<char *>(mConfig.GetClientId().AsCString());
    length = MQTTSNSerialize_pingreq(buffer, MAX_PACKET_SIZE, clientId);

    if (length <= 0)
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }
    SuccessOrExit(error = NewMessage(&message, buffer, length));
    SuccessOrExit(error = SendMessage(*message));

exit:
    return error;
}

void MqttsnClient::OnDisconnected()
{
    mDisconnectRequested = false;
    mSleepRequested = false;
    mGwTimeout = 0;
    mPingReqTime = 0;

    mSubscribeQueue.ForceTimeout();
    mRegisterQueue.ForceTimeout();
    mUnsubscribeQueue.ForceTimeout();
}

bool MqttsnClient::VerifyGatewayAddress(const Ip6::MessageInfo &aMessageInfo)
{
    return aMessageInfo.GetPeerAddr() == mConfig.GetAddress()
        && aMessageInfo.GetPeerPort() == mConfig.GetPort();
}

void MqttsnClient::HandleSubscribeTimeout(const MessageMetadata<SubscribeCallbackFunc> &aMetadata, void* aContext)
{
    OT_UNUSED_VARIABLE(aContext);

    aMetadata.mCallback(kCodeTimeout, 0, aMetadata.mContext);
}

void MqttsnClient::HandleRegisterTimeout(const MessageMetadata<RegisterCallbackFunc> &aMetadata, void* aContext)
{
    OT_UNUSED_VARIABLE(aContext);

    aMetadata.mCallback(kCodeTimeout, 0, aMetadata.mContext);
}

void MqttsnClient::HandleUnsubscribeTimeout(const MessageMetadata<UnsubscribeCallbackFunc> &aMetadata, void* aContext)
{
    OT_UNUSED_VARIABLE(aContext);

    aMetadata.mCallback(kCodeTimeout, aMetadata.mContext);
}

}
}
