#include "LoggerHelper.hpp"

#include "CTransportManager.hpp"
#include "CBluetoothAdapter.hpp"
#include "TransportManagerLoggerHelpers.hpp"

#include <algorithm>

using namespace NsAppLink::NsTransportManager;




NsAppLink::NsTransportManager::CTransportManager::CTransportManager(void):
mDeviceAdapters(),
mLogger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("TransportManager"))),
mDataListenersMutex(),
mDeviceListenersMutex(),
mDeviceHandleGenerationMutex(),
mConnectionHandleGenerationMutex(),
mDataListeners(),
mDeviceListeners(),
mLastUsedDeviceHandle(0),
mLastUsedConnectionHandle(0),
mApplicationCallbacksThread(),
mDeviceListenersConditionVar(),
mDeviceListenersCallbacks(),
mTerminateFlag(false),
mDataListenersCallbacks(),
mDataCallbacksThreads(),
mDataCallbacksConditionVars(),
mDevicesByAdapter(),
mDevicesByAdapterMutex()
{
    addDeviceAdapter(new CBluetoothAdapter(*this, *this));

    pthread_mutex_init(&mDataListenersMutex, 0);
    pthread_mutex_init(&mDeviceListenersMutex, 0);
    pthread_mutex_init(&mDeviceHandleGenerationMutex, 0);
    pthread_mutex_init(&mConnectionHandleGenerationMutex, 0);

    pthread_cond_init(&mDeviceListenersConditionVar, NULL);

    LOG4CPLUS_INFO_EXT(mLogger, "TransportManager constructed");
}

NsAppLink::NsTransportManager::CTransportManager::~CTransportManager(void)
{
    LOG4CPLUS_INFO_EXT(mLogger, "TransportManager destructor");

    mTerminateFlag = true;

    for (std::vector<IDeviceAdapter*>::iterator di = mDeviceAdapters.begin(); di != mDeviceAdapters.end(); ++di)
    {
        removeDeviceAdapter((*di));
    }

    pthread_mutex_destroy(&mDataListenersMutex);
    pthread_mutex_destroy(&mDeviceListenersMutex);
    pthread_mutex_destroy(&mDeviceHandleGenerationMutex);
    pthread_mutex_destroy(&mConnectionHandleGenerationMutex);

    pthread_cond_destroy(&mDeviceListenersConditionVar);
}

void NsAppLink::NsTransportManager::CTransportManager::run(void)
{
    LOG4CPLUS_INFO_EXT(mLogger, "Starting device adapters");
    for (std::vector<IDeviceAdapter*>::iterator di = mDeviceAdapters.begin(); di != mDeviceAdapters.end(); ++di)
    {
        (*di)->run();
        //(*di)->scanForNewDevices();
    }

    if( false == startApplicationCallbacksThread())
    {
        return;
    }


    //sleep(20);

    //connectDevice(1);

    return;


    sleep (2);

    tInternalDeviceList list;
    
    SInternalDeviceInfo device1;
    device1.mDeviceHandle = 1;
    device1.mUniqueDeviceId = "BT-UNIQUE-DEVICE-ID-111111111111";
    device1.mUserFriendlyName = "BT-FRIENDLY-NAME-111111111111";
    list.push_back(device1);

    SInternalDeviceInfo device2;
    device2.mDeviceHandle = 2;
    device2.mUniqueDeviceId = "BT-UNIQUE-DEVICE-ID-22222222222";
    device2.mUserFriendlyName = "BT-FRIENDLY-NAME-222222222222";
    list.push_back(device2);
    
    onDeviceListUpdated(mDeviceAdapters.front(), list);

    //sleep(2);

    connectDevice(3);

    //sleep(2);

    disconnectDevice(3);

    //sleep(2);

    disconnectDevice(2);

    //sleep(2);

    connectDevice(2);
    
    return;


    //sleep(3);

    tConnectionHandle h1 = 1;
    tConnectionHandle h2 = 22;
    tConnectionHandle h3 = 333;
    tConnectionHandle h4 = 4444;



    onApplicationConnected(SDeviceInfo(), h1);
    onApplicationConnected(SDeviceInfo(), h2);
    onApplicationConnected(SDeviceInfo(), h3);
    onApplicationConnected(SDeviceInfo(), h4);

    sleep(5);
    pthread_mutex_lock(&mDataListenersMutex);

    LOG4CPLUS_TRACE_EXT(mLogger, "Size of condition vars: "<<mDataCallbacksConditionVars.size());
    
    SDataListenerCallback c1(CTransportManager::DataListenerCallbackType_FrameReceived, h1, NULL, 0);
    mDataListenersCallbacks[h1]->push_back(c1);
    pthread_cond_t *v1 = mDataCallbacksConditionVars[h1];
    pthread_cond_signal(v1);
    pthread_mutex_unlock(&mDataListenersMutex);

    LOG4CPLUS_TRACE_EXT(mLogger, "CALLBACK SENT");

    sleep(5);
    
    pthread_mutex_lock(&mDataListenersMutex);

    LOG4CPLUS_TRACE_EXT(mLogger, "Size of condition vars: "<<mDataCallbacksConditionVars.size());

    SDataListenerCallback c2(CTransportManager::DataListenerCallbackType_FrameReceived, h2, NULL, 0);
    mDataListenersCallbacks[h2]->push_back(c2);
    pthread_cond_t *v2 = mDataCallbacksConditionVars[h2];
    pthread_cond_signal(v2);
    pthread_mutex_unlock(&mDataListenersMutex);

    LOG4CPLUS_TRACE_EXT(mLogger, "CALLBACK SENT");

}

void NsAppLink::NsTransportManager::CTransportManager::scanForNewDevices(void)
{
    LOG4CPLUS_INFO_EXT(mLogger, "Scanning new devices on all registered device adapters");
    for (std::vector<IDeviceAdapter*>::iterator di = mDeviceAdapters.begin(); di != mDeviceAdapters.end(); ++di)
    {
        LOG4CPLUS_INFO_EXT(mLogger, "Initiating scanning of new devices on adapter: " <<(*di)->getDeviceType());
        (*di)->scanForNewDevices();
        LOG4CPLUS_INFO_EXT(mLogger, "Scanning of new devices initiated on adapter: " <<(*di)->getDeviceType());
    }
    LOG4CPLUS_INFO_EXT(mLogger, "Scanning of new devices initiated");
}

void NsAppLink::NsTransportManager::CTransportManager::connectDevice(const NsAppLink::NsTransportManager::tDeviceHandle DeviceHandle)
{
    LOG4CPLUS_INFO_EXT(mLogger, "Searching for device adapter for handling DeviceHandle: " << DeviceHandle);

    tDevicesByAdapterMap::const_iterator deviceAdaptersIterator;

    pthread_mutex_lock(&mDevicesByAdapterMutex);

    for(deviceAdaptersIterator = mDevicesByAdapter.begin(); deviceAdaptersIterator != mDevicesByAdapter.end(); ++deviceAdaptersIterator)
    {
        IDeviceAdapter* pDeviceAdapter = deviceAdaptersIterator->first;
        tInternalDeviceList *pDevices = deviceAdaptersIterator->second;

        LOG4CPLUS_INFO_EXT(mLogger, "Processing adapter with type: "<<pDeviceAdapter->getDeviceType());

        tInternalDeviceList::const_iterator devicesInAdapterIterator;
        for(devicesInAdapterIterator = pDevices->begin(); devicesInAdapterIterator != pDevices->end(); ++devicesInAdapterIterator)
        {
            LOG4CPLUS_INFO_EXT(mLogger, "Processing device with unique Id: "<<devicesInAdapterIterator->mUniqueDeviceId << ", DeviceHandle: "<<devicesInAdapterIterator->mDeviceHandle);
            if(devicesInAdapterIterator->mDeviceHandle == DeviceHandle)
            {
                LOG4CPLUS_INFO_EXT(mLogger, "DeviceHandle relates to adapter: "<<pDeviceAdapter->getDeviceType());
                pDeviceAdapter->connectDevice(DeviceHandle);
            }
        }
    }

    LOG4CPLUS_INFO_EXT(mLogger, "Connect for DeviceHandle was processed");

    pthread_mutex_unlock(&mDevicesByAdapterMutex);
}

void NsAppLink::NsTransportManager::CTransportManager::disconnectDevice(const NsAppLink::NsTransportManager::tDeviceHandle DeviceHandle)
{
    LOG4CPLUS_INFO_EXT(mLogger, "Searching for device adapter for handling DeviceHandle: " << DeviceHandle);

    tDevicesByAdapterMap::const_iterator deviceAdaptersIterator;

    pthread_mutex_lock(&mDevicesByAdapterMutex);

    for(deviceAdaptersIterator = mDevicesByAdapter.begin(); deviceAdaptersIterator != mDevicesByAdapter.end(); ++deviceAdaptersIterator)
    {
        IDeviceAdapter* pDeviceAdapter = deviceAdaptersIterator->first;
        tInternalDeviceList *pDevices = deviceAdaptersIterator->second;

        LOG4CPLUS_INFO_EXT(mLogger, "Processing adapter with type: "<<pDeviceAdapter->getDeviceType());

        tInternalDeviceList::const_iterator devicesInAdapterIterator;
        for(devicesInAdapterIterator = pDevices->begin(); devicesInAdapterIterator != pDevices->end(); ++devicesInAdapterIterator)
        {
            LOG4CPLUS_INFO_EXT(mLogger, "Processing device with unique Id: "<<devicesInAdapterIterator->mUniqueDeviceId << ", DeviceHandle: "<<devicesInAdapterIterator->mDeviceHandle);
            if(devicesInAdapterIterator->mDeviceHandle == DeviceHandle)
            {
                LOG4CPLUS_INFO_EXT(mLogger, "DeviceHandle relates to adapter: "<<pDeviceAdapter->getDeviceType());
                pDeviceAdapter->disconnectDevice(DeviceHandle);
            }
        }
    }

    LOG4CPLUS_INFO_EXT(mLogger, "Disonnect for DeviceHandle was processed");

    pthread_mutex_unlock(&mDevicesByAdapterMutex);
}

void NsAppLink::NsTransportManager::CTransportManager::addDataListener(NsAppLink::NsTransportManager::ITransportManagerDataListener * Listener)
{
    pthread_mutex_lock(&mDataListenersMutex);
    mDataListeners.push_back(Listener);
    pthread_mutex_unlock(&mDataListenersMutex);
}

void NsAppLink::NsTransportManager::CTransportManager::removeDataListener(NsAppLink::NsTransportManager::ITransportManagerDataListener * Listener)
{
    pthread_mutex_lock(&mDataListenersMutex);
    mDataListeners.erase(std::remove(mDataListeners.begin(), mDataListeners.end(), Listener), mDataListeners.end());
    pthread_mutex_unlock(&mDataListenersMutex);
}

void NsAppLink::NsTransportManager::CTransportManager::addDeviceListener(NsAppLink::NsTransportManager::ITransportManagerDeviceListener * Listener)
{
    pthread_mutex_lock(&mDeviceListenersMutex);
    mDeviceListeners.push_back(Listener);
    pthread_mutex_unlock(&mDeviceListenersMutex);
}

void NsAppLink::NsTransportManager::CTransportManager::removeDeviceListener(NsAppLink::NsTransportManager::ITransportManagerDeviceListener * Listener)
{
    pthread_mutex_lock(&mDeviceListenersMutex);
    mDeviceListeners.erase(std::remove(mDeviceListeners.begin(), mDeviceListeners.end(), Listener), mDeviceListeners.end());
    pthread_mutex_unlock(&mDeviceListenersMutex);
}

int NsAppLink::NsTransportManager::CTransportManager::sendFrame(NsAppLink::NsTransportManager::tConnectionHandle ConnectionHandle, const uint8_t * Data, size_t DataSize)
{
    LOG4CPLUS_ERROR_EXT(mLogger, "Not implemented");

    return -1;
}

NsAppLink::NsTransportManager::tDeviceHandle NsAppLink::NsTransportManager::CTransportManager::generateNewDeviceHandle(void)
{
    tDeviceHandle outputDeviceHandle;

    pthread_mutex_lock(&mDeviceHandleGenerationMutex);
    ++mLastUsedDeviceHandle;
    outputDeviceHandle = mLastUsedDeviceHandle;
    pthread_mutex_unlock(&mDeviceHandleGenerationMutex);

    return outputDeviceHandle;
}

NsAppLink::NsTransportManager::tConnectionHandle NsAppLink::NsTransportManager::CTransportManager::generateNewConnectionHandle(void)
{
    tConnectionHandle outputConnectionHandle;

    pthread_mutex_lock(&mConnectionHandleGenerationMutex);
    ++mLastUsedConnectionHandle;
    outputConnectionHandle = mLastUsedConnectionHandle;
    pthread_mutex_unlock(&mConnectionHandleGenerationMutex);

    return outputConnectionHandle;
}

void CTransportManager::onDeviceListUpdated(IDeviceAdapter * DeviceAdapter, const tInternalDeviceList & DeviceList)
{
    if(0 == DeviceAdapter)
    {
        LOG4CPLUS_WARN_EXT(mLogger, "DeviceAdapter=0");
    }
    else
    {
        LOG4CPLUS_INFO_EXT(mLogger, "Device adapter type is: "<<DeviceAdapter->getDeviceType() << ", number of devices is: "<<DeviceList.size());
        pthread_mutex_lock(&mDevicesByAdapterMutex);

        tDevicesByAdapterMap::iterator devicesIterator = mDevicesByAdapter.find(DeviceAdapter);
        if(devicesIterator == mDevicesByAdapter.end())
        {
            LOG4CPLUS_WARN_EXT(mLogger, "Invalid adapter initialization. No devices vector available for adapter: "<<DeviceAdapter->getDeviceType());
        }
        else
        {
            // Updating devices for adapter
            tInternalDeviceList *pDevices = devicesIterator->second;
            pDevices->clear();
            std::copy(DeviceList.begin(), DeviceList.end(), std::back_inserter(*pDevices));

            LOG4CPLUS_INFO_EXT(mLogger, "Devices list for adapter is updated. Adapter type is: "<<DeviceAdapter->getDeviceType());

            pthread_mutex_unlock(&mDevicesByAdapterMutex);

            // Calling callback with new device list to subscribers
            sendDeviceListUpdatedCallback();
        }
    }
}

void CTransportManager::onApplicationConnected(const SDeviceInfo & ConnectedDevice, const tConnectionHandle ConnectionHandle)
{
    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "onApplicationConnected");

    startDataCallbacksThread(ConnectionHandle);

    // Sending callback
    pthread_mutex_lock(&mDeviceListenersMutex);

    SDeviceListenerCallback cb(CTransportManager::DeviceListenerCallbackType_ApplicationConnected, ConnectedDevice, ConnectionHandle);
    mDeviceListenersCallbacks.push_back(cb);

    pthread_cond_signal(&mDeviceListenersConditionVar);

    pthread_mutex_unlock(&mDeviceListenersMutex);

    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "END of onApplicationConnected");
}

void CTransportManager::onApplicationDisconnected(const SDeviceInfo & DisconnectedDevice, const tConnectionHandle ConnectionHandle)
{
    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "onApplicationDisconnected");

    //TODO: Checking Connection Handle validity

    stopDataCallbacksThread(ConnectionHandle);

    // Sending callback
    pthread_mutex_lock(&mDeviceListenersMutex);

    SDeviceListenerCallback cb(CTransportManager::DeviceListenerCallbackType_ApplicationDisconnected, DisconnectedDevice, ConnectionHandle);
    mDeviceListenersCallbacks.push_back(cb);

    pthread_cond_signal(&mDeviceListenersConditionVar);

    pthread_mutex_unlock(&mDeviceListenersMutex);

    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "END of onApplicationDisconnected");
}

void CTransportManager::onFrameReceived(IDeviceAdapter * DeviceAdapter, tConnectionHandle ConnectionHandle, const uint8_t * Data, size_t DataSize)
{
    TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "onFrameReceived called. DA: "<<DeviceAdapter<<", DataSize: "<<DataSize);
}

void CTransportManager::onFrameSendCompleted(IDeviceAdapter * DeviceAdapter, tConnectionHandle ConnectionHandle, int FrameSequenceNumber, ESendStatus SendStatus)
{
    LOG4CPLUS_INFO_EXT(mLogger, "onFrameSendCompleted called");
}

CTransportManager::SDeviceListenerCallback::SDeviceListenerCallback(CTransportManager::EDeviceListenerCallbackType CallbackType, const tDeviceList& DeviceList)
: mCallbackType(CallbackType)
, mDeviceList(DeviceList)
, mDeviceInfo()
, mConnectionHandle()
{
}

CTransportManager::SDeviceListenerCallback::SDeviceListenerCallback(CTransportManager::EDeviceListenerCallbackType CallbackType, const SDeviceInfo& DeviceInfo, const tConnectionHandle& ConnectionHandle)
: mCallbackType(CallbackType)
, mDeviceList()
, mDeviceInfo(DeviceInfo)
, mConnectionHandle(ConnectionHandle)
{
}

CTransportManager::SDeviceListenerCallback::~SDeviceListenerCallback(void )
{
}

CTransportManager::SDataListenerCallback::SDataListenerCallback(CTransportManager::EDataListenerCallbackType CallbackType, tConnectionHandle ConnectionHandle, uint8_t* Data, size_t DataSize)
: mCallbackType(CallbackType)
, mConnectionHandle(ConnectionHandle)
, mData(Data)
, mDataSize(DataSize)
, mFrameSequenceNumber(-1)
, mSendStatus(NsAppLink::NsTransportManager::SendStatusUnknownError)
{

}

CTransportManager::SDataListenerCallback::SDataListenerCallback(CTransportManager::EDataListenerCallbackType CallbackType, tConnectionHandle ConnectionHandle, int FrameSequenceNumber, ESendStatus SendStatus)
: mCallbackType(CallbackType)
, mConnectionHandle(ConnectionHandle)
, mData(0)
, mDataSize(0)
, mFrameSequenceNumber(FrameSequenceNumber)
, mSendStatus(SendStatus)
{

}

CTransportManager::SDataListenerCallback::~SDataListenerCallback(void )
{

}

CTransportManager::SDataThreadStartupParams::SDataThreadStartupParams(CTransportManager* TransportManager, tConnectionHandle ConnectionHandle)
: mTransportManager(TransportManager)
, mConnectionHandle(ConnectionHandle)
{
}


void CTransportManager::applicationCallbacksThread()
{
    LOG4CPLUS_INFO_EXT(mLogger, "Waiting for callbacks to send");

    while(false == mTerminateFlag)
    {
        pthread_mutex_lock(&mDeviceListenersMutex);
        pthread_cond_wait(&mDeviceListenersConditionVar, &mDeviceListenersMutex);

        if(mTerminateFlag)
        {
            LOG4CPLUS_INFO_EXT(mLogger, "Thread must be terminated");
            continue;
        }

        LOG4CPLUS_INFO_EXT(mLogger, "Callback to send added. Number of callbacks: " << mDeviceListenersCallbacks.size());

        std::vector<SDeviceListenerCallback>::const_iterator callbackIterator;
        for(callbackIterator = mDeviceListenersCallbacks.begin(); callbackIterator != mDeviceListenersCallbacks.end(); ++callbackIterator)
        {
            LOG4CPLUS_INFO_EXT(mLogger, "Processing callback of type: " << (*callbackIterator).mCallbackType);

            std::vector<ITransportManagerDeviceListener*>::const_iterator deviceListenersIterator;
            int deviceListenerIndex = 0;

            for (deviceListenersIterator = mDeviceListeners.begin(), deviceListenerIndex=0; deviceListenersIterator != mDeviceListeners.end(); ++deviceListenersIterator, ++deviceListenerIndex)
            {
                LOG4CPLUS_INFO_EXT(mLogger, "Calling callback on listener #" << deviceListenerIndex);

                switch((*callbackIterator).mCallbackType)
                {
                    case CTransportManager::DeviceListenerCallbackType_DeviceListUpdated:
                        (*deviceListenersIterator)->onDeviceListUpdated((*callbackIterator).mDeviceList);
                        break;
                    case CTransportManager::DeviceListenerCallbackType_ApplicationConnected:
                        (*deviceListenersIterator)->onApplicationConnected((*callbackIterator).mDeviceInfo, (*callbackIterator).mConnectionHandle);
                        break;
                    case CTransportManager::DeviceListenerCallbackType_ApplicationDisconnected:
                        (*deviceListenersIterator)->onApplicationDisconnected((*callbackIterator).mDeviceInfo, (*callbackIterator).mConnectionHandle);
                        break;
                    default:
                        LOG4CPLUS_ERROR_EXT(mLogger, "Unknown callback type: " << (*callbackIterator).mCallbackType);
                        break;
                }

                LOG4CPLUS_INFO_EXT(mLogger, "Callback on listener #" << deviceListenerIndex <<" called");
            }
        }

        LOG4CPLUS_INFO_EXT(mLogger, "All callbacks processed");
        mDeviceListenersCallbacks.clear();

        pthread_mutex_unlock(&mDeviceListenersMutex);
    }

    LOG4CPLUS_INFO_EXT(mLogger, "ApplicationsCallback thread terminated");
}

void* CTransportManager::applicationCallbacksThreadStartRoutine(void* Data)
{
    if (0 != Data)
    {
        static_cast<CTransportManager*>(Data)->applicationCallbacksThread();
    }

    return 0;
}

void CTransportManager::dataCallbacksThread(const tConnectionHandle ConnectionHandle)
{
    pthread_mutex_lock(&mDataListenersMutex);
    tDataCallbacksConditionVariables::iterator conditionVarIterator = mDataCallbacksConditionVars.find(ConnectionHandle);
    pthread_mutex_unlock(&mDataListenersMutex);

    if(conditionVarIterator == mDataCallbacksConditionVars.end())
    {
        TM_CH_LOG4CPLUS_ERROR_EXT(mLogger, ConnectionHandle, "Condition variable was not found");
        return;
    }

    pthread_cond_t *conditionVar = conditionVarIterator->second;

    TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Waiting for callbacks");

    while(false == mTerminateFlag)
    {
        pthread_mutex_lock(&mDataListenersMutex);
        pthread_cond_wait(conditionVar, &mDataListenersMutex);

        if(mTerminateFlag)
        {
            TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Thread must be terminated");
            continue;
        }

        TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Triggered callback calling");

        tDataCallbacks::iterator callbacksMapIterator = mDataListenersCallbacks.find(ConnectionHandle);
        if(callbacksMapIterator == mDataListenersCallbacks.end())
        {
            TM_CH_LOG4CPLUS_ERROR_EXT(mLogger, ConnectionHandle, "Callbacks vector was not found");
            pthread_mutex_unlock(&mDataListenersMutex);
            continue;
        }

        tDataCallbacksVector *pCallbacksVector = callbacksMapIterator->second;

        TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Callback to send added. Number of callbacks: " << pCallbacksVector->size());

        tDataCallbacksVector::const_iterator callbackIterator;
        for(callbackIterator = pCallbacksVector->begin(); callbackIterator != pCallbacksVector->end(); ++callbackIterator)
        {
            TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Processing callback of type: " << (*callbackIterator).mCallbackType);

            if(ConnectionHandle != callbackIterator->mConnectionHandle)
            {
                TM_CH_LOG4CPLUS_ERROR_EXT(mLogger, ConnectionHandle, "Possible error. Thread connection handle ("<<ConnectionHandle<<") differs from callback connection handle ("<<callbackIterator->mConnectionHandle<<")");
            }

            std::vector<ITransportManagerDataListener*>::const_iterator dataListenersIterator;
            int deviceListenerIndex = 0;

            for (dataListenersIterator = mDataListeners.begin(), deviceListenerIndex=0; dataListenersIterator != mDataListeners.end(); ++dataListenersIterator, ++deviceListenerIndex)
            {
                TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Calling callback on listener #" << deviceListenerIndex);

                switch((*callbackIterator).mCallbackType)
                {
                    case CTransportManager::DataListenerCallbackType_FrameReceived:
                        (*dataListenersIterator)->onFrameReceived(callbackIterator->mConnectionHandle, callbackIterator->mData, callbackIterator->mDataSize);
                        break;
                    case CTransportManager::DataListenerCallbackType_FrameSendCompleted:
                        (*dataListenersIterator)->onFrameSendCompleted(callbackIterator->mConnectionHandle, callbackIterator->mFrameSequenceNumber, callbackIterator->mSendStatus);
                        break;
                    default:
                        TM_CH_LOG4CPLUS_ERROR_EXT(mLogger, ConnectionHandle, "Unknown callback type: " << (*callbackIterator).mCallbackType);
                        break;
                }

                LOG4CPLUS_INFO_EXT(mLogger, "Callback on listener #" << deviceListenerIndex <<" called"<<", ConnectionHandle: "<<ConnectionHandle);
            }
        }

        TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "All callbacks processed, ConnectionHandle: ");
        pCallbacksVector->clear();


        pthread_mutex_unlock(&mDataListenersMutex);

    }

    TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Connection thread terminated");
}

void* CTransportManager::dataCallbacksThreadStartRoutine(void* Data)
{
    SDataThreadStartupParams * parameters = static_cast<SDataThreadStartupParams*>(Data);

    if (0 != parameters)
    {
        CTransportManager *pTransportManager = parameters->mTransportManager;
        tConnectionHandle connectionHandle(parameters->mConnectionHandle);

        delete parameters;
        parameters = 0;

        pTransportManager->dataCallbacksThread(connectionHandle);
    }

    return 0;
}

bool CTransportManager::startApplicationCallbacksThread()
{
    LOG4CPLUS_INFO_EXT(mLogger, "Starting device listeners thread");

    int errorCode = pthread_create(&mApplicationCallbacksThread, 0, &applicationCallbacksThreadStartRoutine, this);

    if (0 == errorCode)
    {
        LOG4CPLUS_INFO_EXT(mLogger, "Device listeners thread started");
        return true;
    }
    else
    {
        LOG4CPLUS_ERROR_EXT(mLogger, "Device listeners thread cannot be started, error code " << errorCode);
        return false;
    }
}

void CTransportManager::stopApplicationCallbacksThread()
{
    LOG4CPLUS_TRACE_EXT(mLogger, "Stopping application-callbacks thread");

    pthread_mutex_lock(&mDeviceListenersMutex);
    pthread_cond_signal(&mDeviceListenersConditionVar);
    pthread_mutex_unlock(&mDeviceListenersMutex);
}

bool CTransportManager::startDataCallbacksThread(const tConnectionHandle ConnectionHandle)
{
    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "Starting data-callbacks thread");

    pthread_mutex_lock(&mDataListenersMutex);

    if(isThreadForConnectionHandleExist(ConnectionHandle))
    {
        TM_CH_LOG4CPLUS_ERROR_EXT(mLogger, ConnectionHandle, "Thread already exist. Possible error.");
    }
    else
    {
        pthread_t connectionThread;
        SDataThreadStartupParams *connectionThreadParams = new SDataThreadStartupParams(this, ConnectionHandle);

        int errorCode = pthread_create(&connectionThread, 0, &dataCallbacksThreadStartRoutine, connectionThreadParams);

        if (0 == errorCode)
        {
            TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Thread started. Preparing callbacks vector and condition variable");

            mDataCallbacksThreads.insert(std::make_pair(ConnectionHandle, connectionThread));

            pthread_cond_t *pConnectionConditionVariable = new pthread_cond_t();
            pthread_cond_init(pConnectionConditionVariable, NULL);
            mDataCallbacksConditionVars.insert(std::make_pair(ConnectionHandle, pConnectionConditionVariable));

            tDataCallbacksVector *pConnectionCallbacks = new tDataCallbacksVector();
            mDataListenersCallbacks.insert(std::make_pair(ConnectionHandle, pConnectionCallbacks));

            TM_CH_LOG4CPLUS_INFO_EXT(mLogger, ConnectionHandle, "Callbacks vector and condition variable prepared");
        }
        else
        {
            LOG4CPLUS_ERROR_EXT(mLogger, "Thread start for connection handle (" << ConnectionHandle << ") failed, error code " << errorCode);
            delete connectionThreadParams;
        }
    }

    pthread_mutex_unlock(&mDataListenersMutex);
}

void CTransportManager::stopDataCallbacksThread(const tConnectionHandle ConnectionHandle)
{
    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "Stopping data-callbacks thread");

    pthread_mutex_lock(&mDataListenersMutex);

    tDataCallbacksConditionVariables::iterator conditionVarIterator = mDataCallbacksConditionVars.find(ConnectionHandle);

    if(conditionVarIterator == mDataCallbacksConditionVars.end())
    {
        TM_CH_LOG4CPLUS_ERROR_EXT(mLogger, ConnectionHandle, "ConnectionHandle is invalid");
    }
    else
    {
        pthread_cond_t *conditionVar = conditionVarIterator->second;
        pthread_cond_signal(conditionVar);
    }

    pthread_mutex_unlock(&mDataListenersMutex);
}


bool CTransportManager::isThreadForConnectionHandleExist(const tConnectionHandle ConnectionHandle)
{
    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "Checking whether thread for connection handle exist");

    bool bThreadExist = (mDataCallbacksThreads.find(ConnectionHandle) != mDataCallbacksThreads.end());

    TM_CH_LOG4CPLUS_TRACE_EXT(mLogger, ConnectionHandle, "Result of checking is: " << bThreadExist);

    return bThreadExist;
}

void CTransportManager::addDeviceAdapter(IDeviceAdapter* DeviceAdapter)
{
    mDeviceAdapters.push_back(DeviceAdapter);
    mDevicesByAdapter.insert(std::make_pair(DeviceAdapter, new tInternalDeviceList()));
}

void CTransportManager::removeDeviceAdapter(IDeviceAdapter* DeviceAdapter)
{
    tDevicesByAdapterMap::iterator devicesIterator = mDevicesByAdapter.find(DeviceAdapter);
    if(devicesIterator != mDevicesByAdapter.end())
    {
        delete devicesIterator->second;
        mDevicesByAdapter.erase(DeviceAdapter);
    }

    delete DeviceAdapter;
}

void CTransportManager::sendDeviceListUpdatedCallback()
{
    LOG4CPLUS_INFO_EXT(mLogger, "Preparing complete device list from all adapters");

    // Preparing complete device list
    tDeviceList devices;

    tDevicesByAdapterMap::const_iterator deviceAdaptersIterator;

    pthread_mutex_lock(&mDevicesByAdapterMutex);

    for(deviceAdaptersIterator = mDevicesByAdapter.begin(); deviceAdaptersIterator != mDevicesByAdapter.end(); ++deviceAdaptersIterator)
    {
        IDeviceAdapter* pDeviceAdapter = deviceAdaptersIterator->first;
        tInternalDeviceList *pDevices = deviceAdaptersIterator->second;

        LOG4CPLUS_INFO_EXT(mLogger, "Processing adapter with type: "<<pDeviceAdapter->getDeviceType());

        tInternalDeviceList::const_iterator devicesInAdapterIterator;
        for(devicesInAdapterIterator = pDevices->begin(); devicesInAdapterIterator != pDevices->end(); ++devicesInAdapterIterator)
        {
            SDeviceInfo deviceInfo;

            deviceInfo.mDeviceHandle = devicesInAdapterIterator->mDeviceHandle;
            deviceInfo.mDeviceType = pDeviceAdapter->getDeviceType();
            deviceInfo.mUniqueDeviceId = devicesInAdapterIterator->mUniqueDeviceId;
            deviceInfo.mUserFriendlyName = devicesInAdapterIterator->mUserFriendlyName;

            devices.push_back(deviceInfo);

            LOG4CPLUS_INFO_EXT(mLogger, "Processed device with unique Id: "<<devicesInAdapterIterator->mUniqueDeviceId << ", friendlyName: "<<devicesInAdapterIterator->mUserFriendlyName);
        }
    }

    LOG4CPLUS_INFO_EXT(mLogger, "Complete device list from all adapters was prepared. Preparing callback OnDeviceListUpdated for sending");

    pthread_mutex_unlock(&mDevicesByAdapterMutex);

    // Sending DeviceListUpdatedCallback
    pthread_mutex_lock(&mDeviceListenersMutex);

    SDeviceListenerCallback cb(CTransportManager::DeviceListenerCallbackType_DeviceListUpdated, devices);
    mDeviceListenersCallbacks.push_back(cb);

    pthread_cond_signal(&mDeviceListenersConditionVar);

    pthread_mutex_unlock(&mDeviceListenersMutex);

    LOG4CPLUS_INFO_EXT(mLogger, "Callback OnDeviceListUpdated was prepared for sending");
}
