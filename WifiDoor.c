/*
* Author: Brian Pham
* Created: December 2014
* About: This file is used to read data received from the
* MySQL database in order to send signals to the 4 channel
* relay. For COMPE160 Student Project.
*/

#include <wiringPi.h>
#include <my_global.h>
#include <mysql.h>

int main(int argc, char **argv){

        //MYSQL PORTION OF CODE ESTABLISHES CONNECTION TO DATABASE
        //DO THIS TO ACCESS THE TABLE CONTAINING GPIO INFO
        MYSQL *con = mysql_init(NULL);

        if(con == NULL){
                fprintf(stderr, "%s\n", mysql_error(con));
                exit(1);
        }

        if(mysql_real_connect(con, "localhost", "root", "I1l2y3lc!",
                 "gpio", 3306, NULL, 0) == NULL){
                fprintf(stderr, "%s\n", mysql_error(con));
                mysql_close(con);
                exit(1);
        }

        //wiringPi setup
        wiringPiSetup();
        pinMode (0,OUTPUT);     //output pin to relay

        //creates variables for database values
        char on;        //current value stored in database
        char temp = '1';        //copies stored value for comparison
        int state = 1;  //value to write to pin

        while(1){
                //build query in MySQL to select values from pinStatus
                if(mysql_query(con, "SELECT * FROM pinStatus")){
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);
                }

                //store query result
                MYSQL_RES *result = mysql_store_result(con);

                //create row variable
                MYSQL_ROW row;

                //fetch row
                row = mysql_fetch_row(result);


                on = *row[2];

                //checks if pinStatus has changed by comparing it to temp
                if(on == temp){
                        state = 0;      //changes state to be written to pin
                        if(temp == '1')         //changes temp to opposite state to check in next iteration
                                temp = '0';
                        else if(temp == '0')
                                temp = '1';
                //UNCOMMENT FOR DEBUGGING PURPOSES
                printf("Temp value is %c\n", temp);
                }
                //UNCOMMENT FOR DEBUGGING PURPOSES
                printf("Value write to pin is %d\n", state);
                digitalWrite(0, state); //writes current state to pin
                delay(1000);
                state = 1;      //changes state back to 0, only write 1 if change in on value
                                //allows user to see if garage is currently open or closed on webserver
        }
}
