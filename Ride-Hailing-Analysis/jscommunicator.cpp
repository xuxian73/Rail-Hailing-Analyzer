#include "jscommunicator.h"

jsCommunicator::jsCommunicator()
{
}

void jsCommunicator::jschangemarker(){
    qDebug() << "type:" << type << " lat:" << lat << " lng:" << lng;
    emit jsChangeMarker(type.toInt(), lat.toDouble(), lng.toDouble());
}
