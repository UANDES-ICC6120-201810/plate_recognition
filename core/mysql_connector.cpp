//
// Created by José Tomás González on 12-06-18.
//

#include "mysql_connector.h"

#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <mutex>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

std::mutex post_mutex;

MysqlConnector::MysqlConnector( std::string host, std::string user, std::string password, std::string database ) {
    while ( true ) {
        try {
            std::cout << "[Info] Connecting to db.." << std::endl;
            this -> driver = get_driver_instance();
            this -> connection = driver -> connect( host, user, password );

            this -> connection -> setSchema( database );
            this -> statement = connection -> createStatement();

            return;
        } catch ( sql::SQLException &e ) {
            std::cout << "[Error] Connection failed! Retrying..." << std::endl;
            usleep( 1000000 );
        }
    }
}

void MysqlConnector::async_post(std::string plate) {
    std::thread asyncDbPost(&MysqlConnector::post, this, plate);
    asyncDbPost.detach();
}

void MysqlConnector::post( std::string plate ) {
    int inserted_rows = 0;

    int post_tries = 1;
    int max_post_tries = 2;

    while ( inserted_rows == 0 && post_tries <= max_post_tries ) {
        std::cout << "[Info] Try " << post_tries << " of " << max_post_tries << ": Inserting '" << plate << "' into plate_readings" << std::endl;
        
        post_mutex.lock();
        try {
            inserted_rows = statement -> executeUpdate( "INSERT INTO plate_readings(plate) VALUES ('" + plate + "')" );
            std::cout << "[Info] Post of plate'" << plate << "' was successful!" << std::endl;
            
        } catch ( sql::SQLException &e ) {
            std::cout << "[Error] Got SQLException while inserting into DB! Retrying..." << std::endl;
        }
        post_mutex.unlock();

        post_tries++;
    }
}

MysqlConnector::~MysqlConnector() {
    delete resultset;
    delete statement;
    delete connection;
}
