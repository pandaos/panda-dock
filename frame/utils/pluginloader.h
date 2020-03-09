#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QThread>

class PluginLoader : public QThread
{
    Q_OBJECT

public:
    explicit PluginLoader(const QString &pluginDirPath, QObject *parent);

signals:
    void finished() const;
    void pluginFounded(const QString &pluginFile) const;

protected:
    void run();

private:
    QString m_pluginDirPath;
};

#endif // PLUGINLOADER_H
