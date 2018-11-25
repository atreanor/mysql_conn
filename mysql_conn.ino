#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#include <MySQL_Connection.h>    // MySQL connection modules
#include <MySQL_Cursor.h>

SoftwareSerial NodeMCU(D2,D3);
// const char* ssid = "*WIFI-NAME*";  // enter your *wifi name*
// const char* wifi_pass = "*PASSWORD*";  // enter your wifi *password*


byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // mac address to initialise device

IPAddress server_addr(0,0,0,0);   // IP address from your RDS, get from AWS console or ping endpoint.
char user[] = "*USERNAME*";                 // MySQL *username* & *password*
char mysql_pass[] = "*PASSWORD*";

int weight = 0;
row_values* row;

// --- Sample Insert MySQL query to write to database ---------------------------------
char INSERT_SQL[] = "INSERT INTO dbName.tableName (cust_id, bowl_weight) VALUES ('123', '144')";

// --- Sample Update MySQL query to update database -----------------------------------
char UPDATE_SQL[] = "UPDATE dbName.tableName SET bowl_weight = 52 WHERE cust_id = 1001";

// --- Sample select MySQL query to retrieve data from database -----------------------
char SELECT_SQL[] = "SELECT feed_weight FROM dbName.tableName WHERE cust_id = 1001";

WiFiClient client;
MySQL_Connection conn((Client *)&client);

// ***************************************************************
// **************************** SETUP ****************************
// ***************************************************************

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // SETS TO STATION MODE!

  WiFi.begin(ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1500);
  }
  Serial.println("WiFi Connected");
	NodeMCU.begin(4800);
	pinMode(D2,INPUT);
	pinMode(D3,OUTPUT);
  
  Serial.println("Connecting to DB...");
  if (conn.connect(server_addr, 3306, user, mysql_pass)) {
    Serial.println("DB connection successful!!"); 
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);  // initiate the query class instance
    // --------------------------- INSERT SQL --------------------------------------------
    // cur_mem->execute(INSERT_SQL);  //execute insert query

    // --------------------------- UPDATE SQL --------------------------------------------
    // cur_mem->execute(UPDATE_SQL); // execute update query

    // -------------------------- READ SQL -----------------------------------------------
    int db_weight;
    cur_mem->execute(SELECT_SQL);
    column_names *columns = cur_mem->get_columns();
    do {
      row = cur_mem->get_next_row();
      if (row != NULL) {
        db_weight = atol(row->values[0]);
      }
    }
    while (row != NULL);
      delete cur_mem;
    Serial.print(" feed_weight from database = ");
    Serial.print(db_weight);
    

    delete cur_mem; // no results to be read so deleting the cursor frees up memory
    delay(1000); 
  }
  else
    Serial.println("DB connection failed.");
  conn.close();  
}


// ***************************************************************
// **************************** LOOP *****************************
// ***************************************************************

void loop(){    
  Serial.print("IP is ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC Address is 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED");
  float val = NodeMCU.parseFloat();
    if(NodeMCU.read()== '\n'){
      Serial.println(val);
  }
delay(30000);
}
