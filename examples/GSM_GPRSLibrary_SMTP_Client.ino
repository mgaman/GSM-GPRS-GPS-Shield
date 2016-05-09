/*
  This example shows how to send an encrypted mail
  As each SMPT function basically returns true or false, depending on success,
  the flow is simply a load of nested if () statements.
  
  The SMTP class is capable of sending encrypted (SSL) or non-encrypted mail. Make sure you match the correct
  server port with your choice.
  Non-encrypted mail usually goes to port 25
  Encrypted mail usually goes to port 465 but that may vary with the mail server.
  
  The SIM900 can accept a mail body up to 4K bytes.
  SMTP class supports messages in ASCII by default. UTF-8 is theoretically supported but not yet 
  tested.
*/
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "smtp.h"
#include "inetGSM.h"

#define USESSL  // comment out if not using SSL
//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

SMTPGSM smtp;  // smtp object

int i=0;

// Edit the following for your own circumstances
char *Sender = "itsallaboutme@yahoo.com";
char *ServerURL = "smtp.mail.yahoo.com";
char *Login = "itsallaboutme@yahoo.com";
char *Password = "secret";

char *APN = ".....";
char *SenderNickName = "dave";
char *Recipient = "bill@microsoft.com";
char *RecipientNickName = "Fred";
char *Subject = "BLAH";
char *Body = "hello world";
int CID = 1;
#ifdef USESSL
int ServerPort = 465;
#else
int ServerPort = 25;
#endif

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  if (gsm.begin(9600))
  {
    Serial.println("\nstatus=READY");
    if (smtp.SmtpGprsIsOpen(CID))
      smtp.SmtpCloseGprs(CID);
    Serial.println("init passed");
  }
  else
    Serial.println("\nstatus=IDLE");
}

void loop() 
{
  if (smtp.SmtpOpenGprs(CID,APN))
  {
    smtp.SmtpTimeout(60);
    smtp.SmtpSetCS(smtp.ASCII);
    Serial.println("open gprs passed");
#ifdef USESSL
    smtp.SmtpSetSSL(true);
#else
    smtp.SmtpSetSSL(false);
#endif
    if (smtp.SmtpSetSender(Sender,SenderNickName))
    {
      Serial.println("set sender passed");
      if (smtp.SmtpSetServer(ServerURL,ServerPort))
      {
        Serial.println("set server passed");
        if (smtp.SmtpSetLogin(Login,Password))
        {
          Serial.println("set login passed");
          if (smtp.SmtpSetRecipient(smtp.TO,0,Recipient,RecipientNickName))
          {
            Serial.println("set rcp passed");
            if (smtp.SmtpSetSubject(Subject))
            {
              Serial.println("set subject passed");
              if (smtp.SmtpSendBody(Body))
                 Serial.println("send body passed");
              else
                Serial.println("send body failed");
              smtp.SmtpCloseGprs(CID);
              while (true){}
            }
            else
              Serial.println("set subject failed");
          }
          else
            Serial.println("set rcp failed");
        }
        else
          Serial.println("set sender failed");
      }
      else
        Serial.println("set login failed");
    }
    else
      Serial.println("set server failed");
  }
  else
  {
    Serial.println("open gprs failed");
    if (smtp.SmtpCloseGprs(CID))
      Serial.println("closed gprs");
  }
}

