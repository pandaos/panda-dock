 #include "apprunthread.h"
#include <stdlib.h>

AppRunThread::AppRunThread(const QString &exec, QObject *parent)
    : QThread(parent),
      m_exec(exec)
{

}

void AppRunThread::run()
{
    system(m_exec.toStdString().c_str());
}
