#include "logger.h"

//FIXME: change the color of the text
void Logger::LOG_I(QString logTag,QString message,bool save){
    std::cout << logTag.toStdString() << ": " << message.toStdString() << std::endl;
    if(save){
        //write it to some file
    }
}

void Logger::LOG_W(QString logTag,QString message,bool save){
    std::cout << logTag.toStdString() << ": " << message.toStdString() << std::endl;
    if(save){
        //write it to some file
    }
}

void Logger::LOG_E(QString logTag,QString message,bool save){
    std::cout << logTag.toStdString() << ": " << message.toStdString() << std::endl;
    if(save){
        //write it to some file
    }
}
