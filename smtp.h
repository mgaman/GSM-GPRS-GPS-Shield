#ifndef _SMTP_
#define _SMTP_

#include "SIM900.h"
//#define SMTP_DEBUG_ON  // comment out if not needed
#define MAX_TEXT_LENGTH 100   // maximum length of a body text
#define MAX_COMMAND_LENGTH 20  // maximum length of a raw AT command format

#define SMTP_LIB_VERSION 100 // initial version, UTF-8 unsupported, SSL and non-SSL supported
             // NOTE I have noticed that login names with non alphanumeric characters can cause the
			 // SIM900 to reset e.g. the AT command
			 // AT_SMTPAUTH=1,"fred_1_2@gmail.com","password" causes SIM900 to reset while
			 // AT_SMTPAUTH=1,"fred12@gmail.com","password" is accepted
			 // persistent data between successful sends are SSL,character set, timeout
			 // other data server,login,sender,recipient,body must be redefined.
class SMTPGSM
{
	public:
	//
	enum eSMTPCS {ASCII,UTF8};
	enum eSMTPRCPT { TO=0, CC, BCC};
    // SMTP's methods 
    bool SmtpSetServer(char *name_str, int port);
    bool SmtpSetLogin(char *login_str,char *password);
    bool SmtpSetSender(char *address,char *name);
    bool SmtpSetRecipient(enum eSMTPRCPT, int index, char *address,char *name);
	bool SmtpSetSSL(bool onoff);
	bool SmtpTimeout(int seconds);
	bool SmtpSetCS(eSMTPCS cset);
	bool SmtpSetSubject(char *sj);
	bool SmtpSendBody(char *text);
	bool SmtpOpenGprs(int cid,char *apn);
	bool SmtpGprsIsOpen(int cid);
	bool SmtpQueryGprs(int cid);
	bool SmtpCloseGprs(int cid);
	int SmtpGetBodySize();
	private:
	bool ssl;
	int cid;
	unsigned long timeout;
	enum eSMTPCS cset;
	char printbuf[MAX_TEXT_LENGTH+MAX_COMMAND_LENGTH];
};
#endif

