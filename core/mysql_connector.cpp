//
// Created by José Tomás González on 12-06-18.
//

#include "mysql_connector.h"

#include <string>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

MysqlConnector::MysqlConnector( std::string host, std::string user, std::string password, std::string database ) {
    this -> driver = get_driver_instance();
    this -> connection = driver -> connect( host, user, password );

    this -> connection -> setSchema( database );
    this -> statement = connection -> createStatement();
}

bool MysqlConnector::post( std::string plate ) {
    try {
        statement -> executeQuery( "INSERT INTO plate_readings(plate) VALUES ('"  + plate + "')" );
    } catch (sql::SQLException &e) {
        return false;
    }

    return true;
}

MysqlConnector::~MysqlConnector() {
    delete resultset;
    delete statement;
    delete connection;
}
