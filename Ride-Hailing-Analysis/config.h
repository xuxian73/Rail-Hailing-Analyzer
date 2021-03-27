#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QVariant>
#include <QIODevice>

class Config
{
    QMap<QString, QVariant> config;
public:
    Config();

    void set(QString key, QVariant value);

    QVariant get(QString key);

    static Config* instance();
};

#endif // CONFIG_H
