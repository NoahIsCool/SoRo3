#include "configreader.h"

using namespace Logger;

ConfigReader::ConfigReader(QObject *parent) : QObject(parent)
{

}

ConfigReader::ConfigReader(QString configFile){
    QFile file(configFile);
    QTextStream in(&file);
    if(!file.exists()){
        doesExist = false;
        LOG_W(LOG_TAG,"config file " + configFile + " does not exist.");
        LOG_W(LOG_TAG,"WHATEVER! I DO WHAT I WANT!");
    }else{
        if(!file.open(QIODevice::ReadOnly)) {
            LOG_E(LOG_TAG,file.errorString());
        }
        LOG_E(LOG_TAG,"File exists! Oh Boy!");
        doesExist = true;
        QString line = "";
        LOG_I(LOG_TAG,line);
        while(!in.atEnd()) {
            line = in.readLine();

            QStringList  fields = line.split("=");

            StringPair pair;
            pair.tag = fields[0];
            pair.value = fields[1];
            list.push_back(pair);
        }
    }
}

bool ConfigReader::exists(){
    return doesExist;
}

QString ConfigReader::find(QString tag){
    for(StringPair pair : list){
        if(pair.tag == tag){
            return pair.value;
        }
    }
    return "NOT_FOUND";
}
