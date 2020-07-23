#ifndef APPRUNTHREAD_H
#define APPRUNTHREAD_H

#include <QThread>

class AppRunThread : public QThread
{
    Q_OBJECT

public:
    explicit AppRunThread(const QString &exec, QObject *parent = nullptr);

    void run();

private:
    QString m_exec;

};

#endif // APPRUNTHREAD_H
