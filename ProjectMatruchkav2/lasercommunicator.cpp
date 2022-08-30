#include "lasercommunicator.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>


LaserCommunicator::LaserCommunicator(QObject *parent) : QObject(parent)
{
    pLLT = c_CreateLLTDevice(INTF_TYPE_ETHERNET);

    if(pLLT == 0)
    {
        qDebug() << "Device creation failed";
        return;
    }
    qDebug() << "Device created: " << pLLT;
    qRegisterMetaType<QVector<unsigned int>>("QVector<unsigned int>");
    Interfaces = {1};

    serialCom = new SerialCommunicator(this);
}

void LaserCommunicator::dataToCSV(std::vector<double> &xVector, std::vector<double> &zVector, std::vector<unsigned short> intensityVector, int resolution)
{
    int numberOfProfiles = xVector.size() / resolution;

    bool fileFound = true;
    int fileIterator = 0;
    QString fileName = "";
    while(fileFound)
    {
        fileName = "xyz" + QString::number(fileIterator) + ".csv";

        if(!QFile::exists(fileName))
        {
            fileFound = false;
        }
        else fileIterator++;
        qDebug()<<fileIterator;
    }

    std::fstream fout;
    qDebug() << fileName;
    fout.open(fileName.toStdString(), std::fstream::out | std::fstream::trunc);

    //printing xVector
    for (int i = 0; i<numberOfProfiles; i++)
    {
        for (int j = 0; j<resolution-1; j++)
        {
            fout << xVector[j] << ';';
        }
        fout << xVector[resolution-1] << '\n';
    }
    fout << '\n';

    //printing zVector
    for (int i = 0; i<numberOfProfiles; i++)
    {
        for (int j = 0; j<resolution-1; j++)
        {
            fout << zVector[j] << ';';
        }
        fout << zVector[resolution-1] << '\n';
    }
    fout << '\n';

    //printing intensityVector
    for (int i = 0; i<numberOfProfiles; i++)
    {
        for (int j = 0; j<resolution-1; j++)
        {
            fout << intensityVector[j] << ';';
        }
        fout << intensityVector[resolution-1] << '\n';
    }
    fout << '\n';
}

void LaserCommunicator::scanInterfaces()
{
    c_GetDeviceInterfacesFast(pLLT, &Interfaces[0], Interfaces.size());
    if(Interfaces[0] == 0)
    {
        qDebug() << "No devices found";
        return;
    }
    emit devicesFound(Interfaces);
}

void LaserCommunicator::connectToSensor(unsigned int sensorID, int numberOfProfiles)
{
    //int yRightcounter = 0;


    //Go Start coordinates
    //serial->write("o");
    //Sleep(400);
    //serial->write("1");
    //while(datas != "q"){
    //    if(!serial->waitForReadyRead(-1)){ //block until new data arrives
    //        qDebug() << "error: " << serial->errorString();
    //        }
    //    else{
    //        qDebug() << "New data available: " << serial->bytesAvailable();
    //        datas = serial->readAll();
    //        qDebug() << datas;
    //        }
    //}
    //datas = "!";
    //serial->write("f");


    //serial->write("o");
    //Sleep(5);
    //////y yolladı q bekledi
    //while(yRightcounter<400){
    //    serial->write("y");
    //    while(datas != "q")
    //    {

    //        if(!serial->waitForReadyRead(-1)){ //block until new data arrives
    //            qDebug() << "error: " << serial->errorString();
    //            }
    //        else{
    //            qDebug() << "New data available: " << serial->bytesAvailable();
    //            datas = serial->readAll();

    //            qDebug() << datas;
    //            }

    //    }
    //    Sleep(10);

    //    datas = "!";
    //    qDebug() << yRightcounter++;
    //}
    //datas = "!";
//serial->write("f");


///////////// connection
///
    int err = c_SetDeviceInterface(pLLT, sensorID, 0);
//    -252 scanControl is connected call disconnect first
    if(err == -252)
    {
        emit sendFeedback(err);
        return;
    }
    err = c_Connect(pLLT);
    if (err == 1)
        c_GetLLTType(pLLT, &scanCONTROLType);
    emit sendFeedback(err);
//    ERROR_CONNECT_LLT_COUNT -300
//    There is no scanCONTROL connected
//    to the computer or the driver is not
//    installed correctly.
//    ERROR_CONNECT_SELECTED_LLT -301
//    The selected interface is not available
//    -> choose a new interface with
//    SetDeviceInterface()
//    ERROR_CONNECT_ALREADY_CONNECTED -302
//    There is already a scanCONTROL
//    connected with this ID
//    ERROR_CONNECT_LLT_NUMBER_ALREADY_USED -303
//    The requested scanCONTROL is
//    already used by another instance ->
//    choose another scanCONTROL with
//    SetDeviceInterface()
//    ERROR_CONNECT_SERIAL_CONNECTION -304
//    The scanCONTROL by serial interface
//    could not be connected -> choose
//    another scanCONTROL with
//    SetDeviceInterface()
//    ERROR_CONNECT_INVALID_IP -305
//    Invalid device interface is
//    assigned(via SetDeviceInterface())

    //GET-SET RESOLUTION
    QVector<DWORD> Resolutions(6);
    if(c_GetResolutions(pLLT, &Resolutions[0], Resolutions.size()) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during GetResolution";
        return;
    }
    qDebug() << Resolutions;

    if(c_SetResolution(pLLT, Resolutions[0]) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetResolution";
        return;
    }
    resolution = Resolutions[0];

    //TYPE ALREADY RECEIVED IN CONNECTION

    if(c_SetFeature(pLLT, FEATURE_FUNCTION_TRIGGER, TRIG_INTERNAL) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetFeature(FEATURE_FUNCTION_TRIGGER)";
        return;
    }

    if(c_SetProfileConfig(pLLT, CONTAINER) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetProfileConfig";
        return;
    }

    if(c_SetFeature(pLLT, FEATURE_FUNCTION_EXPOSURE_TIME, exposureTime) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetFeature(FEATURE_FUNCTION_EXPOSURE_TIME)";
        return;
    }

    if(c_SetFeature(pLLT, FEATURE_FUNCTION_IDLE_TIME, idleTime) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetFeature(FEATURE_FUNCTION_IDLE_TIME)";
    }

    DWORD dwInquiry;
    DWORD dwRearrangement;

    double dTempLog = 1.0 / log(2.0);
    DWORD dwResolutionBitField = (DWORD)floor((log((double)Resolutions[0]) * dTempLog) + 0.5);

    if(c_GetFeature(pLLT, INQUIRY_FUNCTION_PROFILE_REARRANGEMENT, &dwInquiry) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during GetFeature";
        return;
    }

    //test

    //qDebug() << "dwinq" << dwInquiry << "\n";
    //end_test



    if((dwInquiry & 0x80000000) == NULL)
    {
        qDebug() << "Sensor does not support container mode";
        return;
    }

    //if(c_SetFeature(pLLT, FEATURE_FUNCTION_PROFILE_REARRANGEMENT, CONTAINER_DATA_Z | CONTAINER_DATA_X | CONTAINER_DATA_TS |
    //                CONTAINER_DATA_EMPTYFIELD4TS | CONTAINER_STRIPE_1 |
    //                dwResolutionBitField << 12) < GENERAL_FUNCTION_OK)
    //{
    //    qDebug() << "Error during SetFeature(FEATURE_FUNCTION_PROFILE_REARRANGEMENT)";
    //    return;
    //}

    //test

    if(c_SetFeature(pLLT, FEATURE_FUNCTION_PROFILE_REARRANGEMENT,CONTAINER_DATA_INTENS | CONTAINER_DATA_Z | CONTAINER_DATA_X | CONTAINER_DATA_TS |
                    CONTAINER_DATA_EMPTYFIELD4TS | CONTAINER_STRIPE_1 |
                    dwResolutionBitField << 12) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetFeature(FEATURE_FUNCTION_PROFILE_REARRANGEMENT)";
        return;
    }

    //end_test

    if(c_GetFeature(pLLT, FEATURE_FUNCTION_PROFILE_REARRANGEMENT, &dwRearrangement) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during GetFeature(FEATURE_FUNCTION_PROFILE_REARRANGEMENT)";
        return;
    }

        qDebug() << "dwrearr" << dwRearrangement << "\n";
        this->dwRearrangement = dwRearrangement;



    this->profileCount = numberOfProfiles;

    if(c_SetProfileContainerSize(pLLT, 0, profileCount) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetProfileContainerSize";
        return;
    }



}

void LaserCommunicator::disconnectFromSensor()
{
    int err = c_Disconnect(pLLT);
    emit sendFeedback(err);
}

void LaserCommunicator::startProcess()
{

    int xCounter = 0, yCounter = 0;
    bool right = true;
    bool left = false;


    serialCom->writeSerial("o");
    Sleep(200);
    serialCom->writeSerial("x");
    serialCom->readSerial();

    for(;yCounter < 50; yCounter++)
    {
        tempGetData(xCounter,yCounter);
        serialCom->stepY(right);
    }

    tempGetData(xCounter,yCounter);
    serialCom->stepX(right);
    xCounter++;

    for(;yCounter > 0; yCounter--)
    {
        tempGetData(xCounter,yCounter);
        serialCom->stepY(left);
    }

    tempGetData(xCounter,yCounter);
    serialCom->stepX(right);
    xCounter++;

    pollData();
    qDebug() << "finished getType()";
}

void LaserCommunicator::pollData()
{

    qDebug()<<"transfering now";
    if(c_TransferProfiles(pLLT, NORMAL_CONTAINER_MODE, true) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during TransferProfiles";
        return;
    }


    qDebug() << "res is " << resolution;
    TConvertContainerParameter param;
    bool noContainerReceived = true;
    std::vector<double> vdValueX(resolution * profileCount);
    std::vector<double> vdValueZ(resolution * profileCount);
    std::vector<unsigned short> vdMaxIntensity(resolution * profileCount);





    qDebug()<<"im here";
    Sleep(120);


    //TRANSFERING NOW


    vucContainerBuffer.resize(2 * resolution * fieldCount * profileCount);

    int counter = 0;
    while(noContainerReceived)
    {
     //   qDebug()<<"in while" << counter;
        if(c_GetActualProfile(pLLT, &vucContainerBuffer[0], vucContainerBuffer.size(), CONTAINER, NULL) !=
                (int)(vucContainerBuffer.size()))
        {
            noContainerReceived = true;
            Sleep((idleTime+exposureTime)/100);
        }
        else
        {
            qDebug() << "Container Received";
            noContainerReceived = false;
        }
        counter++;
    }

    param.pContainer = &vucContainerBuffer[0];
    param.profileRearrangement = dwRearrangement;
    param.numberOfProfilesToExtract = profileCount;
    param.scannerType = scanCONTROLType;
    param.reflectionNumber = 0;
    param.convertToMM = true;
    param.pReflectionWidth = nullptr;
    param.pMaxIntensity = &vdMaxIntensity[0];
    param.pThreshold = nullptr;
    param.pMoment0 = nullptr;
    param.pMoment1 = nullptr;
    param.pX = &vdValueX[0];
    param.pZ = &vdValueZ[0];

    if(c_ConvertContainer2Values(pLLT, param) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during ConvertContainer2Values";
        return;
    }

    //qDebug() << vdValueX.size();
    //qDebug() << vdValueZ.size();
    //qDebug() << "Profile Buffer: " << vucContainerBuffer;
    qDebug() << "X values: " << vdValueX.size();
    qDebug() << "Z values: " << vdValueZ.size();
    qDebug() << "MaxIntensities: " << vdMaxIntensity.size();

    dataToCSV(vdValueX, vdValueZ, vdMaxIntensity, resolution);

    qDebug()<<"transfering now";
    if(c_TransferProfiles(pLLT, NORMAL_CONTAINER_MODE, false) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during TransferProfiles";
        return;
    }

//    std::vector<unsigned char>ProfileBuffer(Resolutions[0] * 64);
//    std::vector<double>ValueX(Resolutions[0]);
//    std::vector<double>ValueZ(Resolutions[0]);
//    unsigned int LostProfiles = 0;
//
//    c_TransferProfiles(pLLT, NORMAL_TRANSFER, true);
//    Sleep(120);
//    c_GetActualProfile(pLLT, &ProfileBuffer.at(0), (unsigned int)ProfileBuffer.size(),PROFILE , &LostProfiles);
//    c_ConvertProfile2Values(pLLT, &ProfileBuffer.at(0), Resolutions[0], PROFILE, scanCONTROLType, 0 , 1, NULL, NULL, NULL, &ValueX.at(0), &ValueZ.at(0), NULL, NULL);
//    c_TransferProfiles(pLLT, NORMAL_TRANSFER, false);
//    qDebug() << "ProfileBuffer: " << ProfileBuffer;
//    qDebug() << "Value X: " << ValueX;
//    qDebug() << "Value Z: " << ValueZ;
}

void LaserCommunicator::pollImageData()
{

    if(c_SetFeature(pLLT, FEATURE_FUNCTION_TRIGGER, TRIG_INTERNAL) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetFeature(FEATURE_FUNCTION_TRIGGER)";
        return;
    }

    if(c_SetProfileConfig(pLLT, VIDEO_IMAGE) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetProfileConfig";
        return;
    }

    if(c_SetFeature(pLLT, FEATURE_FUNCTION_EXPOSURE_TIME, exposureTime) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetFeature(FEATURE_FUNCTION_EXPOSURE_TIME)";
        return;
    }

    if(c_SetFeature(pLLT, FEATURE_FUNCTION_IDLE_TIME, idleTime) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetFeature(FEATURE_FUNCTION_IDLE_TIME)";
        return;
    }

    //TYPE ALREADY RECEIVED IN CONNECTION

    if ((iRetValue = c_SetPacketSize(pLLT, m_uiPacketSize)) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during SetPacketSize " << iRetValue;
        return;
    }

    Sleep(200);


    unsigned int uiWidth, uiHeight;
    bool noVideoImageReceived = true;


    if((iRetValue = c_TransferVideoStream(pLLT, VIDEO_MODE_1,true, &uiWidth, &uiHeight)) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during TransferProfiles " << iRetValue;
        return;
    }

    Sleep(1000);
    qDebug() << "Save video image data\n";
    qDebug() << c_SaveProfiles(pLLT, "D:/qt/EX/VideoImage.bmp", BMP);

    vucVideoBuffer.resize(uiWidth * uiHeight);

    while (noVideoImageReceived) {
            if ((iRetValue = c_GetActualProfile(pLLT, &vucVideoBuffer[0], (unsigned int)vucVideoBuffer.size(), VIDEO_IMAGE, NULL)) !=
                (int)vucVideoBuffer.size())
            {
                if (iRetValue == ERROR_PROFTRANS_NO_NEW_PROFILE) {
                    qDebug() << "in here";
                    Sleep((idleTime + exposureTime) / 100);
                    continue;
                }
                else {
                    qDebug() << "Error during GetActualProfile " << iRetValue;
                    return;
                }
            }
            else {
                qDebug() << "One video picture received \n";
                noVideoImageReceived = false;
            }
        }
    if((iRetValue = c_TransferVideoStream(pLLT, VIDEO_MODE_1, false, NULL, NULL)) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during TransferProfiles " << iRetValue;
        return;
    }

    if ((iRetValue = c_Disconnect(pLLT)) < GENERAL_FUNCTION_OK)
    {
        qDebug() << "Error during Disconnect " << iRetValue;
    }
}

void LaserCommunicator::tempGetData(int x, int y)
{
    qDebug() << "x: " << x << "y: " << y;
    Sleep(50);
}


