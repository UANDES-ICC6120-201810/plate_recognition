//
// Created by José Tomás González on 12-06-18.
//

#ifndef MYSQL_CONNECTOR_H
#define MYSQL_CONNECTOR_H

#include <string>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class MysqlConnector {
public:
    MysqlConnector( std::string host, std::string user, std::string password, std::string database );
    void async_post( std::string plate );
    void post( std::string plate );
    ~MysqlConnector();

private:
    sql::Driver *driver;
    sql::Connection *connection;
    sql::Statement *statement;
    sql::ResultSet *resultset;
};

#endif
