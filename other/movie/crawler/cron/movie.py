#!/usr/bin/python
import MySQLdb
if __name__=='__main__':
	conn = MySQLdb.Connection("202.114.215.73","movie",'eivom123','movie')
	cur = conn.cursor()
	cur.execute("select user.email email, user.phone reminder.content from reminder,user where reminder.user=user.id and reminder.status='A'");
	rows=cur.fetchall();
	for row in rows:
		user=row[0];
		content = row[2];
		header="MIME-Version: 1.0\r\nContent-Type: text/html\r\n"
		subject='Subject: Reminder from HUSTMovie\r\n\r\n'
		server=smtplib.SMTP('smtp.163.com')
		server.set_debuglevel(self._debug)
		server.docmd("EHLO server")
		server.docmd("AUTH LOGIN")
		server.send(base64.encodestring("hankjin"))
		server.getreply()
		server.send(base64.encodestring("851017"))
		server.getreply()
		server.docmd("MAIL FROM:" + "<hankjin@163.com>")
		server.docmd("RCPT TO:<%s>" % user)
		server.docmd("DATA")
		server.send(header)
		server.send(subject)
		server.send(content)
		server.send("\r\n.\r\n")
		server.getreply()
		server.quit()
		print '%s send email' % time.localtime()
