#include "registry.h"

static Registry* registry = NULL;
Registry::Registry()
{

}

void Registry::set(QString key, QVariant value){
    reg[key] = value;
}

QVariant Registry::get(QString key) {
    return reg[key];
}

Registry* Registry::instance(){
    if (!registry) {
        registry = new Registry;
    }
    return registry;
}
