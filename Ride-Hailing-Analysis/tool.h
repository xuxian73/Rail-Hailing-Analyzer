#ifndef TOOL_H
#define TOOL_H

#include <QSqlQuery>

int sqlSize(QSqlQuery query);

void pre_connect_database();

void pro_connect_database();

QSqlDatabase get_connection();

#endif // TOOL_H
