#ifndef REGISTRY_H
#define REGISTRY_H

#include <QString>
#include <QVariant>
#include <QIODevice>

class Registry
{
    QMap<QString, QVariant> reg;
public:
    Registry();

    void set(QString key, QVariant value);

    QVariant get(QString key);

    static Registry* instance();
};

#endif // REGISTRY_H
