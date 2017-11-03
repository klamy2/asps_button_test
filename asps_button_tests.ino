#include <SPI.h>
#include <Ethernet.h>
#define REQ_BUF_SZ   60

const int led1 = PN_1;
const int led2 = PN_0;
// MAC address from Ethernet shield sticker under board
byte mac[]={ 0x00, 0x1A, 0xB6, 0x03, 0x36, 0x68 };
IPAddress ip(169, 254, 60, 0); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

//String HTTP_req;          // stores the HTTP request
//boolean LED_status = 0;   // state of LED, off by default
char HTTP_req[REQ_BUF_SZ] = {0};
char req_index = 0; 
boolean LED_state[4] = {0}; // stores the states of the LEDs

void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(led1, OUTPUT);       // LED on pin 2
    pinMode(led2, OUTPUT);
}

void loop()
{
  
      EthernetClient client = server.available();  // try to get client
      char c=client.read();

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
               /* HTTP_req += c;*/  // save the HTTP request 1 char at a time
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                /*if (HTTP_req.indexOf("favicon.ico") > -1) {
                  HTTP_req="";
                  break;
                }*/
                if (c == '\n' && currentLineIsBlank) { //After this ut html code
                // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    //client.println("Content-Type: text/html");
                    //client.println("Connection: close");
                    //client.println();
                    if (StrContains(HTTP_req, "ajax_inputs")) {
                        // send rest of HTTP header
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        SetLEDs();
                        // send XML file containing input states
                        XML_response(client);
                    }
                    else {  // web page request
                        // send rest of HTTP header
                        client.println("Content-Type: text/html");
                        client.println("Connection: keep-alive");
                        client.println();
                    // send web page
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<title>ASPS-DAQ BSL Helper Page</title>");
                    
                    client.println("<script>");
                    client.println("strLED1 = \"\";");
		    client.println("strLED2 = \"\";");
		    client.println("strLED3 = \"\";");
		    client.println("strLED4 = \"\";");
		    client.println("var LED3_state = 0;");
		    client.println("var LED4_state = 0;");
                    client.println("function GetArduinoIO() {");
                    client.println("nocache = \"&nocache=\" + Math.random() * 1000000");
                    client.println("var request = new XMLHttpRequest();");
                    client.println("request.onreadystatechange = function() {");
                    client.println("if (this.readyState == 4) {");
                    client.println("if (this.status == 200) {");
                    client.println("if (this.responseXML != null) {");
                    client.println("var count;");
                    ////SYNTAX ERROR HERE?
                    //client.println("var num_an = this.responseXML.getElementsByTagName('analog').length;");
                    /////////
                    client.println("for (count = 0; count < num_an; count++) {");
                    client.println("document.getElementsByClassName(\"analog\")[count].innerHTML = this.responseXML.getElementsByTagName('analog')[count].childNodes[0].nodeValue; }");
                    client.println("if (this.responseXML.getElementsByTagName('LED')[0].childNodes[0].nodeValue === \"checked\") {");
                    client.println("document.LED_form.LED1.checked = true; }");
                    client.println("else {");
                    client.println("document.LED_form.LED1.checked = false; }");
                    client.println("if (this.responseXML.getElementsByTagName('LED')[1].childNodes[0].nodeValue === \"checked\") {");
                    client.println("document.LED_form.LED2.checked = true; }";
                    client.println("else {");
                    client.println("document.LED_form.LED2.checked = false; } } } } }");
                    client.println("request.open(\"GET\", \"ajax_inputs\" + strLED1 + strLED2 + strLED3 + strLED4 + nocache, true);");
                    client.println("request.send(null);");
                    client.println("setTimeout('GetArduinoIO()', 1000);");
                    client.println("strLED1 = \"\";");
                    client.println("strLED2 = \"\";");
                    client.println("strLED3 = \"\";");
                    client.println("strLED4 = \"\"; }");
                    client.println("function GetCheck() {");
                    client.println("if (LED_form.LED1.checked) {");
                    client.println("strLED1 = \"&LED1=1\"; }");
                    client.println("else {");
                    client.println("strLED1 = \"&LED1=0\"; }");
                    client.println("if (LED_form.LED2.checked) {");
                    client.println("strLED2 = \"&LED2=1\"; }");
                    client.println("else {");
                    client.println("strLED2 = \"&LED2=0\"; } }");
                    client.println("</script>");
                    
                    client.println("<style>");
                    client.println(".IO_box {");
                    client.println("float: left;");
                    client.println("margin: 0 20px 20px 0;");
                    client.println("border: 1px solid red;");
                    /////Syntax error
                    client.println("padding: 0 5px 0 5px;");
                    client.println("width: 120px; }");
                    client.println("h1 {");
                    client.println("font-size: 120%;");
                    client.println("color: black;");
                    client.println("margin: 0 0 10px 0; }");
                                        
                    client.println("<body onload=\"GetArduinoIO()\">");
                    client.println("<h1> Button Press test</h1>")
                    client.println("<div class=\"IO_box\">");
                    client.println("<h2>LEDs Using Checkboxes</h2>");
                    client.println("<form id=\"check_LEDs\" name=\"LED_form\">");
                    client.println("<input type=\"checkbox\" name=\"LED1\" value=\"0\" onclick=\"GetCheck()\" />LED 1 (D6)<br /><br />");
                    client.println("<input type=\"checkbox\" name=\"LED2\" value=\"0\" onclick=\"GetCheck()\" />LED 2 (D7)<br /><br />");
                    client.println("</form>");
                    client.println("</div>");
                    
                    client.println("</head>");
                    /*
                    client.println("<body>");
                    client.println("<h1>ASPS-DAQ BSL Helper Page</h1>");
                    client.println("<form action=\"index.html\" method=\"link\">");
                    client.println("<input type=\"submit\" value=\"Main Page\">");
                    client.println("</form>");
                    client.println("<form action=\"serial.html\" method=\"link\">");   //Adding buttons to navigate!
                    client.println("<input type=\"submit\" value=\"Serial Page\">");
                    client.println("</form>");
                    client.println("<form action=\"bsl.html\" method=\"link\">");
                    client.println("<input type=\"submit\" value=\"BSL Page\">");
                    client.println("</p>");
                    client.println("<br>");
                    client.println("<form action=\"bsl.html\" method=\"get\">");
                    //client.println("<input type=\"radio\" name=\"0\" value=\"0\" checked>ASPS-POWER");
                    client.println("<form method=\"get\">");
                    ProcessCheckbox(client);    //I Think this reads the box but how name it?
                    client.println("<input type=\"radio\" name=\"0\" value=\"1\">Heater");
                    client.println("<br>");
                    client.println("<input type=\"radio\" name=\"1\" value=\"0\" checked>Normal");
                    client.println("<input type=\"radio\" name=\"1\" value=\"1\">BSL");
                    client.println("<br>");
                    client.println("<input type=\"submit\" value=\"Reboot Microcontroller\">");
                    client.println("</form>");
                    client.println("</body>");
                    */
                    client.println("</html>");
                    }
                    Serial.print(HTTP_req);
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
                }
                 if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                 }
                 else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                 }
            }
        }
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    }
}
void SetLEDs(void)
{
    // LED 1 (pin 6)
    if (StrContains(HTTP_req, "LED1=1")) {
        LED_state[0] = 1;  // save LED state
        digitalWrite(led1, HIGH);
    }
    else if (StrContains(HTTP_req, "LED1=0")) {
        LED_state[0] = 0;  // save LED state
        digitalWrite(led1, LOW);
    }
    // LED 2 (pin 7)
    if (StrContains(HTTP_req, "LED2=1")) {
        LED_state[1] = 1;  // save LED state
        digitalWrite(led2, HIGH);
    }
    else if (StrContains(HTTP_req, "LED2=0")) {
        LED_state[1] = 0;  // save LED state
        digitalWrite(led2, LOW);
    }
}
void XML_response(EthernetClient cl)
{
  cl.print("<?xml version = \"1.0\" ?>");
  cl.print("<inputs>");
  
   cl.print("<LED>");
    if (LED_state[0]) {
        cl.print("checked");
    }
    else {
        cl.print("unchecked");
    }
    cl.println("</LED>");
    // LED2
    cl.print("<LED>");
    if (LED_state[1]) {
        cl.print("checked");
    }
    else {
        cl.print("unchecked");
    }
     cl.println("</LED>");
     cl.print("</inputs>");
}
 
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
