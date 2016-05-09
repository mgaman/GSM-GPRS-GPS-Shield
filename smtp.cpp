#include "smtp.h"

/**********************************************************
Method sends SMTP


return: 
        All functions except SmtpBody return bool
		true - success
		false - failure
		
		Note that none of the mail parameters (server, authentification, sender
		recipients, subject) are persistent. Once a mail has been sent they must 
		all be reset for the next message.
		Character set, timeout and SSL are persistent.
		Maximum mail body size 4K. Up to you to keep track of what you are sending

an example of usage for encrypted mail: 
        SMTPGSM smtp;
		int CID = 1;  // can be 1,2 or 3 you must keep track of what is used
		smtp.SmtpOpenGprs(CID,"myapn");
		smtp.SmtpTimeout(60);
        smtp.SmtpSetServer("mail.smtp.yahoo.com",465);
		smtp.SmtpSetLogin("yourlogin","yourpassword");
		smtp.SmtpSetSSL(true);
		smtp.SmtpSetCS(smtp.ASCII);
		smtp.SmtpSetSender("myname@yahoo.com","nickname");
		smtp.SmtpSetRecipient(smtp.TO,0,"billgates@microsoft.com");
		smtp.SmtpSetRecipient(smtp.TO,1,"zuck@facebook.com");
		smtp.SmtpSetRecipient(smtp.CC,0,"pope@vatican.org");
		smtp.SmtpSetRecipient(smtp.BCC,0,"queen@palace.co.uk");
		smtp.SmtpSetSubject("party tonight");
		smtp.SmtpSendBody("dont be late");	
		smtp.SmtpCloseGprs(CID);
**********************************************************/
const char *OK = "OK";

bool writewithtimeout(char *buf,const char *response,unsigned long start,unsigned long interchar)
{
	bool success = false;
	byte status;
#ifdef SMTP_DEBUG_ON
	Serial.println(buf);
#endif
	gsm.SimpleWrite(buf);
	// 1 sec. for initial comm tmout
    // and max. 150 msec. for inter character timeout
	gsm.RxInit(start, interchar); 
	// wait response is finished
	do
	{
		if (gsm.IsStringReceived(response))
		{ 
			// perfect - we have some response, but what:
			status = RX_FINISHED;
			success = true;
			break; // so finish receiving immediately and let's go to 
             // to check response 
		}
		status = gsm.IsRxFinished();
	} while (status == RX_NOT_FINISHED);
	return success;
}

bool SMTPGSM::SmtpSetServer(char *server, int port)
{
	bool success = false;
	//pServer = server;
	//Port = port;
	sprintf(printbuf,"AT+SMTPSRV=\"%s\",%d\r\n",server,port);
#ifdef DEBUG_ON
	Serial.println("DEBUG:SMTP SETSERV");
	Serial.println(printbuf);
#endif
	return writewithtimeout(printbuf,OK,4000,150);
}
bool SMTPGSM::SmtpSetLogin(char *login_str,char *password)
{
	bool success = false;
	//pLogin = login_str;
	//pPassword = password;
	sprintf(printbuf,"AT+SMTPAUTH=1,\"%s\",\"%s\"\r",login_str,password);
#ifdef DEBUG_ON
	Serial.println("DEBUG:SMTP SETLOGIN");
	Serial.println(printbuf);
#endif
	return writewithtimeout(printbuf,OK,1000,150);
}
bool SMTPGSM::SmtpSetSender(char *address,char *name)
{
//	pSenderAddress = address;
	//pSenderName = name;
	bool success = false;
	sprintf(printbuf,"AT+SMTPFROM=\"%s\",\"%s\"\r",address,name);
#ifdef DEBUG_ON
	Serial.println("DEBUG:SMTP SETSENDER");
	Serial.println(printbuf);
#endif
	return writewithtimeout(printbuf,OK,1000,150);
}
bool SMTPGSM::SmtpSetSSL(bool ssl)
{
	if (ssl)
		return writewithtimeout("AT+EMAILSSL=1\r",OK,1000,150);
	else
		return writewithtimeout("AT+EMAILSSL=0\r",OK,1000,150);
}
bool SMTPGSM::SmtpSetCS(eSMTPCS cs)
{
#if 0
	bool success=false;
	cset = cs;
	if (cs == ASCII)
		return writewithtimeout("AT+SMTPCS=\"ASCII\"\r",OK,1000,150);
	else
		return writewithtimeout("AT+SMTPCS=\"UTF-8\",\r",OK,1000,150);
#else
	return writewithtimeout("AT+SMTPCS=\"ASCII\"\r",OK,1000,150);
#endif
}
bool SMTPGSM::SmtpSetRecipient(enum eSMTPRCPT type, int index, char *address,char *name)
{
	bool success = false;
	sprintf(printbuf,"AT+SMTPRCPT=%d,%d,\"%s\",\"%s\"\r",type,index,address,name);
#ifdef DEBUG_ON
	Serial.println("DEBUG:SMTP SETRCPT");
	Serial.println(printbuf);
#endif
	return writewithtimeout(printbuf,OK,1000,150);
}

bool SMTPGSM::SmtpSendBody(char *body)
{
	bool success = false;
	if (writewithtimeout("AT+SMTPBODY\r",">",1000,150))
	{
#ifdef DEBUG_ON
		Serial.println("DEBUG:SMTP SETBODY");
		Serial.println(body);
#endif
		gsm.SimpleWrite(body);
		sprintf(printbuf,"%c",26);
		if (writewithtimeout(printbuf,OK,1000,150)) // add CTRL-Z end marker 
		{
			delay(2000);
			success = writewithtimeout("AT+SMTPSEND\r","+SMTPSEND: 1",1000,timeout*1000);	
		}
#ifdef SMTP_DEBUG_ON
  		Serial.println((char *)gsm.comm_buf);	
#endif
	}
	return success;	
}
bool SMTPGSM::SmtpTimeout(int to)
{
	bool success = false;
	timeout = (unsigned long)to;
	sprintf(printbuf,"AT+EMAILTO=%d\r",to);
#ifdef DEBUG_ON
	Serial.println("DEBUG:SMTP SETINIT");
	Serial.println(printbuf);
#endif
	return writewithtimeout(printbuf,OK,1000,150);
}

bool SMTPGSM::SmtpOpenGprs(int CID,char *apn)
{
	bool success = false;
	cid = CID;
	sprintf(printbuf,"AT+SAPBR=3,%d,\"CONTYPE\",\"GPRS\"\r",cid);
	if (writewithtimeout(printbuf,OK,5000,1000))
	{
		delay(2000);
		sprintf(printbuf,"AT+SAPBR=3,%d,\"APN\",\"%s\"\r",cid,apn);
#ifdef DEBUG_ON
		Serial.println("DEBUG:SMTP SETOPENGPRS");
		Serial.println(printbuf);
#endif
		if (writewithtimeout(printbuf,OK,1000,150))
		{
			delay(2000);
			sprintf(printbuf,"AT+SAPBR=1,%d\r",cid);
			if (writewithtimeout(printbuf,OK,1000,1000))
			{
				sprintf(printbuf,"AT+EMAILCID=%d\r",CID);
				return writewithtimeout(printbuf,OK,1000,150);
			}
		}
	}
	return success;
}

bool SMTPGSM::SmtpQueryGprs(int CID)
{
	sprintf(printbuf,"AT+SAPBR=2,%d\r",CID);
	return writewithtimeout(printbuf,OK,1000,150);
}

bool SMTPGSM::SmtpCloseGprs(int CID)
{
	sprintf(printbuf,"AT+SAPBR=0,%d\r",CID);
	return writewithtimeout(printbuf,OK,1000,150);
}

bool SMTPGSM::SmtpSetSubject(char *sj)
{
	bool success = false;
	int i;
	sprintf(printbuf,"AT+SMTPSUB=\"%s\"\r",sj);
#ifdef DEBUG_ON
	Serial.println("DEBUG:SMTP SETSUB");
	Serial.println(printbuf);
#endif
	success= writewithtimeout(printbuf,OK,1000,150);
	return success;
}

/*
    Must call SmtpInit before calling this function;
*/
int SMTPGSM::SmtpGetBodySize()
{
	char *ch;
	Serial.println("bodysize");
	if (writewithtimeout("AT+SMTPBODY=?\r",OK,5000,150))
	{
		ch = strstr((char *)gsm.comm_buf, "+SMTPBODY:");
		if (ch == 0)
			return -1;
		else
		{
			ch += 10; // skip over compare string
			return atoi(ch);
		}
		return 1;
	}
	else
		return -1;
}

bool SMTPGSM::SmtpGprsIsOpen(int CID)
{
	char *ch;
	sprintf(printbuf,"AT+SAPBR=2,%d\r",CID);
	if (writewithtimeout(printbuf,OK,5000,150))
	{
		ch = strstr((char *)gsm.comm_buf, "+SAPBR:1,");
		if (ch == 0)
			return false;
		else
		{
			ch += 9; // skip over compare string
			return *ch == '1';
		}
	}
	else
		return false;
}
