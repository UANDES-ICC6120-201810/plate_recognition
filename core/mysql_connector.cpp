//
// Created by José Tomás González on 12-06-18.
//

#include "mysql_connector.h"

#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

MysqlConnector::MysqlConnector( std::string host, std::string user, std::string password, std::string database ) {
    while ( true ) {
        try {
            std::cout << "Connecting to db.." << std::endl;
            this -> driver = get_driver_instance();
            this -> connection = driver -> connect( host, user, password );

            this -> connection -> setSchema( database );
            this -> statement = connection -> createStatement();

            return;
        } catch ( sql::SQLException &e ) {
            std::cout << "Connection failed! Retrying..." << std::endl;
            usleep( 1000000 );
        }
    }
}

void MysqlConnector::async_post(std::string plate) {
    std::thread asyncDbPost(&MysqlConnector::post, this, plate);
    asyncDbPost.detach();
}

void MysqlConnector::post( std::string plate ) {
    while ( true ) {
        try {
            statement -> executeQuery( "INSERT INTO plate_readings(plate) VALUES ('" + plate + "')" );
            return;
        } catch ( sql::SQLException &e ) {}
    }
}

MysqlConnector::~MysqlConnector() {
    delete resultset;
    delete statement;
    delete connection;
}
