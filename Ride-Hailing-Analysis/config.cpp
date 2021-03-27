#include "config.h"

static Config* configure = NULL;

Config::Config()
{
}

void Config::set(QString key, QVariant value){
    config[key] = value;

}

QVariant Config::get(QString key) {
    return config[key];
}

Config* Config::instance() {
    if (!configure) {
        configure = new Config;
    }
    return configure;
}

